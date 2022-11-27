#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include <synchapi.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    Dialog dialog(this);
    if(dialog.exec()==Dialog::Accepted) {
        readWriteSocketA = dialog.getReadWriteSocketA();
        QObject::connect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvOrder()));
        readWriteSocketB = dialog.getReadWriteSocketB();
        QObject::connect(readWriteSocketB,SIGNAL(readyRead()),this,SLOT(recvLandlordB()));
        ui->setupUi(this);
        setWindowTitle("Landlord PlayerC");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}


bool MainWindow::cardLegal() {

    // this round previous card status
    int previous=0;
    if(thisRoundCardA) previous=thisRoundCardA;
    if(thisRoundCardB) previous=thisRoundCardB;
    int smallest=previous;
    int kind=smallest/10000; smallest-=10000*kind;
    int preSize=smallest/100; smallest-=100*preSize;

    // card to be tested
    toBePlayed.clear();
    for(auto card: yourCard)
        if(card->isChecked()) toBePlayed.push_back(card);
    int size = toBePlayed.size();
    int sum[14];
    for(int i=0;i<=13;i++) sum[i]=0;
    for(auto card: toBePlayed) sum[card->getID()/4]++;

    // special cards

    // 9 王炸
    if(size==2&&toBePlayed[0]->getID()>=52&&toBePlayed[1]->getID()>=52) {
        thisRoundCardC=90000;
        return true;
    }

    // 8 炸弹
    if(size==4) {
        for(int i=0;i<13;i++)
            if(sum[i]==4&&(kind<=7||(kind==8&&i>smallest))) {
                thisRoundCardC=80000+i;
                return true;
            }
    }

    // Case: follow previous player
    if(previous) {
        if(kind>=8) {return false;}
        // 飞机
        else if(kind==7) {
            if(size!=preSize/10*(3+preSize%10)) return false;
            for(int i=0;i<11;i++)
                if(sum[i]>=3&&sum[i+1]>=3) {
                    int cnt = 2;
                    while(sum[i+cnt]>=3) cnt++;
                    if(cnt!=preSize/10) return false;
                    if(i<=smallest) return false;
                    int left = size-cnt*3;
                    if(left==0) {
                        thisRoundCardC=70000+cnt*1000+i;
                        return true;
                    }
                    else if(left==cnt) {
                        thisRoundCardC=70100+cnt*1000+i;
                        return true;
                    }
                    else if(left==cnt*2) {
                        int s=0;
                        for(int j=0;j<=12;j++) if(j<i||j>=i+cnt){
                            if(sum[j]>=2) s+=sum[j]/2;
                        }
                        if(s==cnt) {
                            thisRoundCardC=70200+cnt*1000+i;
                            return true;
                        }
                        else {return false;}
                    }
                    else break;
                }
        }
        // 四带二
        else if(kind==6) {
            if(preSize!=size) return false;
            for(int i=12;i>=0;i--)
                if(sum[i]==4) {
                    if(i<=smallest) return false;
                    if(size==6) {
                        thisRoundCardC=60600+i;
                        return true;
                    }
                    else {
                        int cnt=0;
                        for(int j=0;j<=12;j++)
                            if(j!=i&&sum[j]>=2) cnt+=sum[j]/2;
                        if(cnt==2) {
                            thisRoundCardC=60800+i;
                            return true;
                        }
                        else return false;
                    }
                }
            return false;
        }
        // 连对
        else if(kind==5) {
            if(size!=preSize) return false;
            for(int i=0;i<10;i++) if(sum[i]>0) {
                if(sum[i]!=2||i<=smallest) return false;
                for(int j=i+1;j<i+size/2;j++)
                    if(sum[j]!=2) {return false;}
                thisRoundCardC=50000+size*100+i;
                return true;
            }
        }
        // 顺子
        else if(kind==4) {
            if(size!=preSize) return false;
            for(int i=0;i<=7;i++) if(sum[i]>0) {
                if(sum[i]!=1) return false;
                for(int j=i+1;j<i+size;j++)
                    if(sum[j]!=1) {return false;}
                thisRoundCardC=40000+size*100+i;
                return true;
            }
        }
        // 三带
        else if(kind==3) {
            if(size!=preSize) return false;
            for(int i=0;i<=12;i++)
                if(sum[i]==3) {
                    if(i<=smallest) return false;
                    if(size==3) {
                        thisRoundCardC=30300+i;
                        return true;
                    }
                    else if(size==4) {
                        thisRoundCardC=30400+i;
                        return true;
                    }
                    else {
                        for(int j=0;j<=12;j++)
                            if(sum[j]==2) {
                                thisRoundCardC=30500+i;
                                return true;
                            }
                        return false;
                    }
                    break;
                }
            return false;
        }
        // 对子
        else if(kind==2) {
            if(size!=2) return false;
            for(int i=0;i<=12;i++) if(sum[i]>0) {
                if(sum[i]!=2) return false;
                if(i<=smallest) return false;
                thisRoundCardC=20000+i;
                return true;
            }
        }
        // 单牌
        else if(kind==1) {
            if(size!=1) return false;
            if((toBePlayed[0]->getID())==53) {
                thisRoundCardC=10000+toBePlayed[0]->getID()/4;
                return true;
            }
            if((toBePlayed[0]->getID()/4)<=smallest) return false;
            thisRoundCardC=10000+toBePlayed[0]->getID()/4;
            return true;
        }
    }
    if(previous) return false;

    // Case: new round

    // 7 飞机
    if(size>=6) {
        for(int i=0;i<11;i++)
            if(sum[i]>=3&&sum[i+1]>=3) {
                int cnt = 2;
                while(sum[i+cnt]>=3) cnt++;
                int left = size-cnt*3;
                if(left==0) {
                    thisRoundCardC=70000+cnt*1000+i;
                    return true;
                }
                else if(left==cnt) {
                    thisRoundCardC=70100+cnt*1000+i;
                    return true;
                }
                else if(left==cnt*2) {
                    int s=0;
                    for(int j=0;j<=12;j++) if(j<i||j>=i+cnt){
                        if(sum[j]>=2) s+=sum[j]/2;
                    }
                    if(s==cnt) {
                        thisRoundCardC=70200+cnt*1000+i;
                        return true;
                    }
                }
                else break;
            }
    }

    // 6 四带二
    if(size==6||size==8) {
        for(int i=12;i>=0;i--)
            if(sum[i]==4) {
                if(size==6) {
                    thisRoundCardC=60600+i;
                    return true;
                }
                else {
                    int cnt=0;
                    for(int j=0;j<=12;j++)
                        if(j!=i&&sum[j]>=2) cnt+=sum[j]/2;
                    if(cnt==2) {
                        thisRoundCardC=60800+i;
                        return true;
                    }
                }
            }
    }

    // 5 连对
    if(size>=6&&(size%2==0)) {
        for(int i=0;i<10;i++) if(sum[i]>0) {
            if(sum[i]!=2) break;
            bool flag=true;
            for(int j=i+1;j<i+size/2;j++)
                if(sum[j]!=2) {flag=false;break;}
            if(flag) {
                thisRoundCardC=50000+size*100+i;
                return true;
            }
            break;
        }
    }

    // 4 顺子
    if(size>=5) {
        for(int i=0;i<=7;i++) if(sum[i]>0) {
            if(sum[i]!=1) break;
            bool flag=true;
            for(int j=i+1;j<i+size;j++)
                if(sum[j]!=1) {flag=false;break;}
            if(flag) {
                thisRoundCardC=40000+size*100+i;
                return true;
            }
        }
    }

    // 3 三带
    if(size>=3&&size<=5) {
        for(int i=0;i<=12;i++)
            if(sum[i]==3) {
                if(size==3) {
                    thisRoundCardC=30300+i;
                    return true;
                }
                else if(size==4) {
                    thisRoundCardC=30400+i;
                    return true;
                }
                else {
                    for(int j=0;j<=12;j++)
                        if(sum[j]==2) {
                            thisRoundCardC=30500+i;
                            return true;
                        }
                }
                break;
            }
    }

    // 2 一对
    if(size==2) {
        for(int i=0;i<=12;i++) if(sum[i]>0) {
            if(sum[i]==2) {
                thisRoundCardC=20000+i;
                return true;
            }
            else break;
        }
    }

    // 1 单张
    if(size==1) {
        thisRoundCardC=10000+toBePlayed[0]->getID()/4;
        return true;
    }

    return false;
}

void MainWindow::chooseCard() {
    ui->CC->setText("C");
    ui->play->setVisible(true);
    ui->pass->setVisible(true);
    if(thisRoundCardA||thisRoundCardB) ui->pass->setEnabled(true);
    else ui->pass->setEnabled(false);
    // 点击完要hide
}
void MainWindow::on_play_clicked() {
    if(!cardLegal()) {
        QMessageBox::critical(this,"Error", "出牌不合法");
        return;
    }
    ui->play->hide();
    ui->pass->hide();
    // update UI, send message
    int sum=0; long long tmp=0;

    // 上一次frame_2的牌清空！
        for(auto card:frame2Card) {
            card->deleteLater();
        }
        frame2Card.clear();
        for(auto card:toBePlayed) {
            sum++;
            tmp+=card->getIDnum();
            yourCard.removeOne(card);
            Card* newCard = new Card(card->getID());
            frame2Card.push_back(newCard);
            newCard->setDisabled(true);
            ui->frame_2->layout()->addWidget(newCard);
            card->deleteLater();
        }

    updateCard();
    sendMessage(tmp,sum);
    if(yourCard.empty()) GameEnd(3);
}
void MainWindow::updateCard() {
    QVector<int> cardID;
    for(auto card: yourCard) {
        cardID.push_back(card->getID());
        card->deleteLater();
    }
    yourCard.clear();
    delete yourLayout;
    yourLayout = new QHBoxLayout;
    for(auto id: cardID) {
        Card* newCard = new Card(id);
        yourLayout->addWidget(newCard);
        yourLayout->addSpacing(-50);
        yourCard.push_back(newCard);
    }
    yourLayout->addSpacing(50);
    ui->frame->setLayout(yourLayout);
}


void MainWindow::on_pass_clicked() {
    ui->play->hide();
    ui->pass->hide();
    // 检查thisRoundCardC是不是0！！！！
    thisRoundCardC=0;
    ui->CC->setText("C 不出");
    sendMessage(0,0);
    // ui 不出牌
}



void MainWindow::whoIsLandlord() {
    if(landlord==1) ui->AisLandlord->setText("地主");
    else ui->AisLandlord->setText("农民");
    if(landlord==2) ui->BisLandlord->setText("地主");
    else ui->BisLandlord->setText("农民");
    if(landlord==3) ui->CisLandlord->setText("地主");
    else ui->CisLandlord->setText("农民");
    if(landlord==3) yourLayout->addSpacing(-50);
    if(landlord==2) {countCardB+=3;ui->Bcard->display(countCardB);}
    if(landlord==1) {countCardA+=3;ui->Acard->display(countCardA);}
    for(auto card: landlordCard) {
        card->setPadding(false);
        if(landlord==3) {
            Card* newCard = new Card(card->getID());
            yourCard.push_back(newCard);
            yourLayout->addWidget(newCard);
            yourLayout->addSpacing(-50);
        }
    }
    if(landlord==3) {yourLayout->addSpacing(50);chooseCard();}
}
void MainWindow::chooseLandlord() {
    ui->beLandlord->setVisible(true);
    ui->notLandlord->setVisible(true);
    if(thisRoundCardA||thisRoundCardB) ui->pass->setEnabled(true);
    else ui->pass->setEnabled(false);
}
void MainWindow::on_beLandlord_clicked() {
    landlord=3;
    ui->beLandlord->hide();
    ui->notLandlord->hide();
    ui->CisLandlord->setText("叫地主");
    QByteArray *array =new QByteArray;
    array->clear();
    array->append("yes");
    this->readWriteSocketA->write(array->data());
    this->readWriteSocketA->waitForBytesWritten();
    this->readWriteSocketB->write(array->data());
    this->readWriteSocketB->waitForBytesWritten();
    if(firstone==1) whoIsLandlord();

}
void MainWindow::on_notLandlord_clicked() {

    ui->beLandlord->hide();
    ui->notLandlord->hide();
    ui->CisLandlord->setText("不叫");
    QByteArray *array =new QByteArray;
    array->clear();
    array->append("no");
    this->readWriteSocketA->write(array->data());
    this->readWriteSocketA->waitForBytesWritten();
    this->readWriteSocketB->write(array->data());
    this->readWriteSocketB->waitForBytesWritten();
    if(firstone==1) whoIsLandlord();
}


void MainWindow::recvLandlordA()
{
    qDebug()<<"recvLandlordA!!!!!";
    QString info;
    info += this->readWriteSocketA->readAll();
    QObject::disconnect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvLandlordA()));
    QObject::connect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvMessageA()));
    if(info=="yes") landlord=1,ui->AisLandlord->setText("叫地主");
    if(info=="no") ui->AisLandlord->setText("不叫");
    if(firstone==2) whoIsLandlord();
}

void MainWindow::recvLandlordB()
{
    QString info;
    info += this->readWriteSocketB->readAll();
    QObject::disconnect(readWriteSocketB,SIGNAL(readyRead()),this,SLOT(recvLandlordB()));
    QObject::connect(readWriteSocketB,SIGNAL(readyRead()),this,SLOT(recvMessageB()));
    if(info=="yes") landlord=2,ui->BisLandlord->setText("叫地主");
    if(info=="no") ui->BisLandlord->setText("不叫");

    if(firstone!=3) chooseLandlord();
    else whoIsLandlord();
}

void MainWindow::initUI() {

    qDebug()<<"uiinit";
    // ui init!!!!!
    ui->restartButton->hide();
    ui->exitButton->hide();
    ui->beLandlord->hide();
    ui->notLandlord->hide();
    ui->pass->hide();
    ui->play->hide();
    ui->AA->setText("A");
    ui->BB->setText("B");
    ui->CC->setText("C");
    ui->AisLandlord->setText("未决定");
    ui->BisLandlord->setText("未决定");
    ui->CisLandlord->setText("未决定");

    countCardA=countCardB=17;
    ui->Acard->display(QString::number(countCardA));
    ui->Bcard->display(QString::number(countCardB));

    landlordLayout = new QHBoxLayout;
    for(int i=0;i<54;i++) {
        long long tmp = (long long)1<<i;
        if(tmp&cardLandlord) {
            Card* newCard = new Card(i);
            landlordCard.push_back(newCard);
            newCard->setPadding(true);
            landlordLayout->addWidget(newCard);
        }
    }
    ui->landlord3Card->setLayout(landlordLayout);

    yourLayout = new QHBoxLayout;
    for(int i=0;i<54;i++) {
        long long tmp = (long long)1<<i;
        if(tmp&card) {
            Card* newCard = new Card(i);
            yourCard.push_back(newCard);
            yourLayout->addWidget(newCard);
            yourLayout->addSpacing(-50);
        }
    }
    yourLayout->addSpacing(50);
    ui->frame->setLayout(yourLayout);


    ui->frame_2->setLayout(new QHBoxLayout);

}


void MainWindow::recvMessageA()
{
    qDebug()<<"C enters recvMessageA";
    QString str = this->readWriteSocketA->readAll();
    qDebug()<<"I'm C. A says: "<<str;

    if(str=="#") {confirmRestartA=0;return;}
    if(str=="$") {confirmRestartA=1;return;}

    thisRoundCardA = str.mid(0,5).toInt(); ////////
    long long status = str.mid(5,17).toLongLong();
    int size = str.mid(22,2).toInt();
    // 若为0
    if(size==0) {
        ui->AA->setText("A 不出");
        return;
    }
    ui->AA->setText("A");
    // UI更新
    countCardA -= size; ui->Acard->display(countCardA);

    for(auto card:frame2Card) {
        card->deleteLater();
    }
    frame2Card.clear();
    //QHBoxLayout* frame2Layout = new QHBoxLayout;/////////////////////////
    for(int i=0;i<54;i++) if(((long long)1<<i)&status){
        Card* newCard = new Card(i);
        frame2Card.push_back(newCard);
        ui->frame_2->layout()->addWidget(newCard);
        newCard->setDisabled(true);
    }

    if(countCardA==0) GameEnd(1);
}

void MainWindow::recvMessageB()
{
    qDebug()<<"C enters recvMessageB";
    QString str = this->readWriteSocketB->readAll();
    qDebug()<<"I'm C. B says: "<<str;

    if(str=="#") {confirmRestartB=0;return;}
    if(str=="$") {confirmRestartB=1;return;qDebug()<<"c recv b restart clicked";}

    thisRoundCardB = str.mid(0,5).toInt(); ////////
    long long status = str.mid(5,17).toLongLong();
    int size = str.mid(22,2).toInt();
    // 若为0
    if(size==0) {
        ui->BB->setText("B 不出");
        chooseCard();
        return;
    }
    ui->BB->setText("B");
    // UI更新
    countCardB -= size; ui->Bcard->display(countCardB);

    for(auto card:frame2Card) {
        card->deleteLater();
    }
    frame2Card.clear();
    //QHBoxLayout* frame2Layout = new QHBoxLayout;/////////////////////////
    for(int i=0;i<54;i++) if(((long long)1<<i)&status){
        Card* newCard = new Card(i);
        frame2Card.push_back(newCard);
        ui->frame_2->layout()->addWidget(newCard);
        newCard->setDisabled(true);
    }

    if(countCardB==0) GameEnd(2);
    else chooseCard();
}

void MainWindow::sendMessage(long long status,int size) {
    QByteArray *array =new QByteArray;
    array->clear();
    QString str1 = QString("%1").arg(thisRoundCardC,5,10,QLatin1Char('0')); ////
    QString str2 = QString("%1").arg(status,17,10,QLatin1Char('0'));
    QString str3 = QString("%1").arg(size,2,10,QLatin1Char('0'));
    array->append(str1).append(str2).append(str3);
    this->readWriteSocketA->write(array->data());
    this->readWriteSocketA->waitForBytesWritten();
    this->readWriteSocketB->write(array->data());
    this->readWriteSocketB->waitForBytesWritten();
}

void MainWindow::recvOrder() {
    qDebug()<<"B revOrder from A";
    QString str = this->readWriteSocketA->readAll();
    qDebug()<<str;
    QObject::disconnect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvOrder()));
    QObject::connect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvLandlordA()));
    card = str.mid(0,17).toLongLong();
    cardLandlord = str.mid(17,17).toLongLong();
    firstone = str.mid(str.size()-1,1).toInt();
    landlord = firstone;
    initUI(); // 展示牌
    if(firstone==3) chooseLandlord();
}

void MainWindow::GameEnd(int winner) {
    confirmRestartA = confirmRestartB = confirmRestartC = -1;
    if((winner==3)||(winner!=landlord&&landlord!=3))
        QMessageBox::information(this,tr("info"),tr("胜利！"));
    else QMessageBox::information(this,tr("info"),tr("失败！"));
    ui->restartButton->setVisible(true); ui->restartButton->setEnabled(true);
    ui->exitButton->setVisible(true); ui->exitButton->setEnabled(true);
    ui->restartButton->raise(); ui->exitButton->raise();
    ui->beLandlord->hide();
    ui->notLandlord->hide();
    ui->pass->hide();
    ui->play->hide();
    timer->start(500);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(checkRestart()));
}

void MainWindow::checkRestart() {
    if(confirmRestartA==0||confirmRestartB==0||confirmRestartC==0) {
        QObject::disconnect(timer,SIGNAL(timeout()),this,SLOT(checkRestart()));
        timer->stop();
        QMessageBox::information(this,tr("提示"),tr("已有玩家退出"));
    }
    if(confirmRestartB==1&&confirmRestartA==1&&confirmRestartC==1) {
        QObject::disconnect(timer,SIGNAL(timeout()),this,SLOT(checkRestart()));
        timer->stop();
        initGame();
    }
}

void MainWindow::initGame() {

    QObject::disconnect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvMessageA()));
    QObject::connect(readWriteSocketA,SIGNAL(readyRead()),this,SLOT(recvOrder()));
    QObject::disconnect(readWriteSocketB,SIGNAL(readyRead()),this,SLOT(recvMessageB()));
    QObject::connect(readWriteSocketB,SIGNAL(readyRead()),this,SLOT(recvLandlordB()));

    for(auto card: yourCard) {delete card;}
    for(auto card: landlordCard) {delete card;}
    for(auto card: frame2Card) {delete card;}
    yourCard.clear(); landlordCard.clear(); frame2Card.clear();

    delete landlordLayout;
    delete yourLayout;
    delete ui->frame_2->layout();

    landlord=-1;
    card=cardLandlord=0;
    countCardB=countCardA=17;
    thisRoundCardA=thisRoundCardB=thisRoundCardC=0;

}



void MainWindow::on_restartButton_clicked() {
    confirmRestartC = 1;
    ui->restartButton->setEnabled(false);
    ui->exitButton->setEnabled(false);
    QByteArray *array =new QByteArray;
    array->clear();
    array->append(QString("$"));
    this->readWriteSocketB->write(array->data());
    this->readWriteSocketB->waitForBytesWritten();
    this->readWriteSocketA->write(array->data());
    this->readWriteSocketA->waitForBytesWritten();
    qDebug()<<"c restart clicked";
}

void MainWindow::on_exitButton_clicked() {
    confirmRestartC = 0;
    ui->restartButton->setEnabled(false);
    ui->exitButton->setEnabled(false);
    QByteArray *array =new QByteArray;
    array->clear();
    array->append(QString("#"));
    this->readWriteSocketB->write(array->data());
    this->readWriteSocketB->waitForBytesWritten();
    this->readWriteSocketA->write(array->data());
    this->readWriteSocketA->waitForBytesWritten();
    this->close();
}

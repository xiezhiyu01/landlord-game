#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("dialogC");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::connectA()
{
    A_OK = true;
    if(B_OK == true) return QDialog::accept();
}

void Dialog::connectB()
{
    B_OK = true;
    if(A_OK == true) return QDialog::accept();
}

void Dialog::initClient()
{
    qDebug()<<"trying to connect";
    if(!A_OK) this->readWriteSocketA->connectToHost("183.173.120.192",9999);
    if(!B_OK) this->readWriteSocketB->connectToHost("183.173.120.192",7777);
}

QTcpSocket* Dialog::getReadWriteSocketA() {
    return readWriteSocketA;
}

QTcpSocket* Dialog::getReadWriteSocketB() {
    return readWriteSocketB;
}

void Dialog::on_pushButton_clicked() {
    this->readWriteSocketA = new QTcpSocket;
    QObject::connect(this->readWriteSocketA,SIGNAL(connected()),this,SLOT(connectA()));
    this->readWriteSocketB = new QTcpSocket;
    QObject::connect(this->readWriteSocketB,SIGNAL(connected()),this,SLOT(connectB()));
    timer = new QTimer;
    timer->start(500);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(initClient()));
    ui->pushButton->setEnabled(false);
}

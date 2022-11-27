#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("dialogA");
    qDebug()<<"dialog constructing";
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::acceptConnectionB()
{
    qDebug()<<"a:b connected";
    this->readWriteSocketB = this->listenSocketB->nextPendingConnection();
    this->listenSocketB->close();
    B_OK = true;
    if(C_OK == true) return QDialog::accept();
}

void Dialog::acceptConnectionC()
{
    qDebug()<<"a:c connected";
    this->readWriteSocketC = this->listenSocketC->nextPendingConnection();
    this->listenSocketC->close();
    C_OK = true;
    if(B_OK == true) return QDialog::accept();
}

void Dialog::initServer()
{
    this->listenSocketB =new QTcpServer;
    this->listenSocketB->listen(QHostAddress("183.173.120.192"),8888);
    QObject::connect(this->listenSocketB,SIGNAL(newConnection()),this,SLOT(acceptConnectionB()));

    this->listenSocketC =new QTcpServer;
    this->listenSocketC->listen(QHostAddress("183.173.120.192"),9999);
    QObject::connect(this->listenSocketC,SIGNAL(newConnection()),this,SLOT(acceptConnectionC()));
}

QTcpSocket* Dialog::getReadWriteSocketB() {
    return readWriteSocketB;
}

QTcpSocket* Dialog::getReadWriteSocketC() {
    return readWriteSocketC;
}

void Dialog::on_pushButton_clicked() {
    initServer();
    ui->pushButton->setEnabled(false);
}

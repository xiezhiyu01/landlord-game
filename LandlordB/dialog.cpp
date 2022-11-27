#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    setWindowTitle("dialogB");
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::connectA()
{
    qDebug()<<"a:b connected";
    A_OK = true;
    if(C_OK == true) return QDialog::accept();
}

void Dialog::acceptConnectionC()
{
    qDebug()<<"b:c connected";
    this->readWriteSocketC =this->listenSocketC->nextPendingConnection();
    C_OK = true;
    if(A_OK == true) return QDialog::accept();
}

void Dialog::initServer()
{
    this->listenSocketC = new QTcpServer;
    this->listenSocketC->listen(QHostAddress("183.173.120.192"),7777);
    QObject::connect(this->listenSocketC,SIGNAL(newConnection()),this,SLOT(acceptConnectionC()));
}

void Dialog::initClient()
{
    if(!A_OK) this->readWriteSocketA->connectToHost("183.173.120.192",8888);
}

QTcpSocket* Dialog::getReadWriteSocketA() {
    return readWriteSocketA;
}

QTcpSocket* Dialog::getReadWriteSocketC() {
    return readWriteSocketC;
}

void Dialog::on_pushButton_clicked() {
    initServer();
    this->readWriteSocketA = new QTcpSocket;
    QObject::connect(this->readWriteSocketA,SIGNAL(connected()),this,SLOT(connectA()));
    timer = new QTimer;
    timer->start(500);
    QObject::connect(timer,SIGNAL(timeout()),this,SLOT(initClient()));
    ui->pushButton->setEnabled(false);
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QHBoxLayout>
#include <QMessageBox>

#include "card.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void recvMessageA();
    void recvMessageB();
    void recvOrder();
    void recvLandlordA();
    void recvLandlordB();
    void on_beLandlord_clicked();
    void on_notLandlord_clicked();
    void on_play_clicked();
    void on_pass_clicked();
    void on_restartButton_clicked();
    void on_exitButton_clicked();
    void checkRestart();
private:
    void sendMessage(long long,int);
    void initUI();
    void chooseLandlord();
    void chooseCard();
    void whoIsLandlord();
    bool cardLegal();
    void updateCard();
    void GameEnd(int);
    void initGame();

private:
    Ui::MainWindow *ui;
    QTcpSocket *readWriteSocketA;
    QTcpSocket *readWriteSocketB;
    int firstone;
    int landlord=-1;
    qlonglong card,cardLandlord;
    int countCardA,countCardB; // init
    int thisRoundCardA=0,thisRoundCardB=0,thisRoundCardC=0;
    QTimer* timer = new QTimer;
    QVector<Card*> yourCard,landlordCard,frame2Card;
    QVector<Card*> toBePlayed;
    QHBoxLayout* landlordLayout;
    QHBoxLayout* yourLayout;
    int confirmRestartA,confirmRestartB,confirmRestartC;
    // 初始化！
};
#endif // MAINWINDOW_H

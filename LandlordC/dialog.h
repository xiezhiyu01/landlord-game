#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
    QTcpSocket* getReadWriteSocketA();
    QTcpSocket* getReadWriteSocketB();

public slots:
    void initClient();
    void connectA();
    void connectB();
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QTcpServer *listenSocketC;
    QTcpSocket *readWriteSocketA;
    QTcpSocket *readWriteSocketB;
    bool A_OK = false;
    bool B_OK = false;
    QTimer* timer;
};

#endif // DIALOG_H

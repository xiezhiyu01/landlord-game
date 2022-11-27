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
    void initServer();
    QTcpSocket* getReadWriteSocketA();
    QTcpSocket* getReadWriteSocketC();

public slots:
    void initClient();
    void connectA();
    void acceptConnectionC();
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QTcpServer *listenSocketC;
    QTcpSocket *readWriteSocketA;
    QTcpSocket *readWriteSocketC;
    bool A_OK = false;
    bool C_OK = false;
    QTimer* timer;
};

#endif // DIALOG_H

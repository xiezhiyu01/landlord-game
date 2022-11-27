#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

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
    QTcpSocket* getReadWriteSocketB();
    QTcpSocket* getReadWriteSocketC();

public slots:
    void acceptConnectionB();
    void acceptConnectionC();
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;
    QTcpServer *listenSocketB;
    QTcpServer *listenSocketC;
    QTcpSocket *readWriteSocketB;
    QTcpSocket *readWriteSocketC;
    bool B_OK = false;
    bool C_OK = false;
};

#endif // DIALOG_H

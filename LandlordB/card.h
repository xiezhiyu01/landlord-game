#ifndef CARD_H
#define CARD_H

#include <QAbstractButton>
#include <QPushButton>
#include <QSize>

class Card: public QPushButton {
    Q_OBJECT

public:
    Card(int _id);
    int getID();
    long long getIDnum();
    void setPadding(bool);
    //void paintEvent(QPaintEvent *e) override;

private:
    int id;
    void id2icon();

};


#endif // CARD_H

#include "card.h"
#include <QPainter>
#include <QPaintEvent>
#include <QRect>
#include <QDebug>
#include <QSizePolicy>

Card::Card(int _id) {
    qDebug()<<"card constructing: "<<_id;
    id = _id;
    id2icon();
    this->setCheckable(true);
    this->setIconSize(QSize(215/2,290/2));
    this->resize(QSize(215/2,181));
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Fixed);
    sizePolicy.setVerticalPolicy(QSizePolicy::Fixed);
    this->setSizePolicy(sizePolicy);
    // checked stylesheet???????????
}

void Card::setPadding(bool flag) {
    if(flag) {
        this->setEnabled(false);
        QIcon icon;
        icon.addFile(":/png/cards/PADDING.png",QSize(),QIcon::Disabled,QIcon::Off);
        this->setIcon(icon);
        update();
    }
    else {
        this->setEnabled(true);
        this->setCheckable(true);
        id2icon();
    }
}

void Card::id2icon() {
    QString pre=":/png/cards/",suf=".png",s;
    QString num[13]={"3","4","5","6","7","8","9","10","J","Q","K","A","2"};
    QString col[4]={"C","D","H","S"};
    if(id==52) s="BLACK JOKER"; // 小王
    else if(id==53) s="RED JOKER"; // 大王
    else s=col[id%4]+num[id/4];
    QIcon icon;
    icon.addFile(pre+s+suf);
    icon.addFile(pre+s+suf,QSize(),QIcon::Disabled,QIcon::Off);
    this->setIcon(icon);
}

int Card::getID() {
   return id;
}

long long Card::getIDnum() {
    return ((long long)1<<id);
}


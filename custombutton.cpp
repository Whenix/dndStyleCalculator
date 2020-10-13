#include "custombutton.h"
#include <QDebug>

CustomButton::CustomButton(QString img)
{
    this->imgPath = img;
    QPixmap pixmap;
    bool ret = pixmap.load(this->imgPath);
    if(!ret){
        QString info = QString("img load failed").arg(this->imgPath);
        qDebug() << info;
        return;
    }
    this->setFixedSize(pixmap.width(),pixmap.height());
    this->setStyleSheet("QPushButton{border:0px;}");
    this->setIcon(pixmap);
    this->setIconSize(QSize(pixmap.width(),pixmap.height()));
}



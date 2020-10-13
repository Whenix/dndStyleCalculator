#ifndef CACULATE_H
#define CACULATE_H

#include <QObject>
#include <QTime>
#include <QtGlobal>
#include <QStringList>
#include <mainwindow.h>

class Calculate : public QObject
{
    Q_OBJECT

public:
    explicit Calculate(QObject *parent = nullptr);
    int getRandomNumber(QString &lineBuffer,int sides,int throwTimes);
    bool judgeDiceResult(int number,int threshold);






signals:


};


#endif


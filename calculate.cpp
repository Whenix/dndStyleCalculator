#include "calculate.h"
#include "QDebug"
#include "mainwindow.h"


Calculate::Calculate(QObject *parent) : QObject(parent)
{



}

int Calculate :: getRandomNumber(QString &lineBuffer,int sides=6,int throwTimes=1,int noLimit){
    //qsrand(QTime(0,0,0,000).secsTo(QTime::currentTime()));
    int number=0;
    int total=0;
    lineBuffer+="掷骰结果分别为:";
    for(int i=0;i<throwTimes;i++){
        number = qrand() %(sides+noLimit) +(1-noLimit);
        total +=number;
        lineBuffer+=QString::number(number)+",";
        qDebug() << lineBuffer;
    }
    //退一格消除，然后添加;
    lineBuffer = lineBuffer.left(lineBuffer.size()-1);
    lineBuffer +=";";
    return total;
}


int Calculate :: judgeDiceResult(int number, int threshold){
    float coef = number/threshold -1;

    if(coef <=-0.3f){return -1;}
    else if(coef <0){return 0;}
    else if(coef <0.3f){return 1;}
    else {return 2;}


}

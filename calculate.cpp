#include "calculate.h"
#include "QDebug"
#include "mainwindow.h"


Calculate::Calculate(QObject *parent) : QObject(parent)
{



}
int Calculate :: getRandomNumber(QString &lineBuffer,int sides=6,int throwTimes=1 ){
    //qsrand(QTime(0,0,0,000).secsTo(QTime::currentTime()));
    int number=0;
    lineBuffer += "掷出了";

    for(int i=0;i<throwTimes;i++){
        number += qrand()% sides +1;
        lineBuffer+=QString::number(number)+",";
    }

    lineBuffer = lineBuffer.left(lineBuffer.size()-1);
    lineBuffer +=";";
    return number;
}


bool Calculate :: judgeDiceResult(int number, int threshold){

    if(number >= threshold){
        return true;
    }
    return false;

}

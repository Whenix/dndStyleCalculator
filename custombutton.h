#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPushButton>

class CustomButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CustomButton(QWidget *parent = nullptr);
    CustomButton(QString img="");
    QString imgPath;

signals:

};

#endif // CUSTOMBUTTON_H

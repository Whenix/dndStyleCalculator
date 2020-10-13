#ifndef CACULATE_H
#define CACULATE_H

#include <QObject>

class Caculate : public QObject
{
    Q_OBJECT
public:
    explicit Caculate(QObject *parent = nullptr);

signals:

};

#endif // CACULATE_H

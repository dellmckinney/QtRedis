#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QDebug>
#include <QMap>
#include <QtRedis>
#include <QTime>

class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(QObject *parent = 0);
    ~Test();

    void startTest();

signals:
    
public slots:
    void slotMessage(QtRedis::Reply);

private:
    QtRedis *redis;
};

#endif // TEST_H

#ifndef QTREDIS_H
#define QTREDIS_H

#include <QObject>
#include <QStringList>
#include <QEventLoop>

#include "reader.h"

class QtRedis : public QObject
{
    Q_OBJECT
public:
    explicit QtRedis(QString host, int port, QObject *parent = 0);
    ~QtRedis();

    typedef struct Reply {
        QString type;
        QString message;
        QString pattern;
        QString channel;
        QVariant value;
    } Reply;

    bool openConnection();
    bool closeConnection();
    bool isConnected();

    void publish(QString, QString);
    void subscribe(QString);
    void unsubscribe(QString);
    void psubscribe(QString);
    void punsubscribe(QString);

    Reply command(QString);

    int append(QString key, QString value);
    bool auth(QString password);
    QStringList blpop(QString key, int timeout);
    QStringList brpop(QString key, int timeout);
    QStringList brpoplpush(QString source, QString destination, int timeout);
    int decr(QString key);
    int decrby(QString key,int interval);
    int del(QString key);
    QString dump(QString key);
    bool exists(QString key);
    bool expire(QString key,int seconds);
    bool expireat(QString key, qint64 timestamp);
    QVariant eval(QString script, int numkeys, QString keys, QString arguments);
    QVariant evalsha(QString sha1, int numkeys, QString keys, QString arguments);
    QString get(QString key);
    QString getrange(QString key, int start, int stop);

    int hdel(QString key, QString field);
    bool hexists(QString key, QString field);
    QString hget(QString key, QString field);
    QMap<QString,QVariant> hgetall(QString key);

    int hincrby(QString key, QString field, int increment);
    float hincrbyfloat(QString key, QString field, float increment);
    QStringList hkeys(QString key);
    int hlen(QString key);
    QStringList hmget(QString key, QString field);
    bool hmset(QString key, QMap<QString,QVariant> map);
    bool hset(QString key, QString field, QString value);
    bool hsetnx(QString key, QString field, QString value);
    QStringList hvals(QString key);


    int incr(QString key);
    int incrby(QString key,int interval);
    QStringList keys(QString pattern);
    int lindex(QString key, int index);
    int linsert(QString key, QString position,
                QString pivot, QString value);
    int llen(QString key);
    QString lpop(QString key);
    int lpush(QString key, QString value);
    int lpushx(QString key, QString value);
    QStringList lrange(QString key, int start, int stop);
    int lrem(QString key, int count, QString value);
    int lset(QString key, int index, QString value);
    bool ltrim(QString key, int start, int stop);
    QStringList mget(QString key);
    bool migrate(QString host, int port, QString key, int database, int timeout);
    bool move(QString key, int database);
    bool mset(QMap<QString, QVariant> map);
    QVariant object(QString subcommand, QString arguments);
    bool persist(QString key);
    bool pexpire(QString key, qint64 mseconds);
    bool pexpireat(QString key, qint64 mstimestamp);
    int pttl(QString key);
    bool rename(QString key, QString newkey);
    bool renamex(QString key, QString newkey);
    bool restore(QString key, int ttl, QString value);
    QString rpop(QString key);
    QString rpoplpush(QString source, QString destination);
    int rpush(QString key, QString value);
    int rpushx(QString key, QString value);
    bool sadd(QString key, QString member);
    int scard(QString key);
    QStringList scriptexists(QString script);
    QString scriptflush();
    QString scriptkill();
    QString scriptload(QString script);
    QStringList sdiff(QString keys);
    int sdiffstore(QString destination, QString keys);
    QStringList sinter(QString keys);
    int sinterstore(QString destination, QString keys);
    bool sismember(QString key, QString member);
    QStringList smembers(QString key);
    bool smove(QString source, QString destination, QString member);
    QString spop(QString key);
    QStringList srandmember(QString key, int count);
    int srem(QString key, QString member);
    QStringList sunion(QString keys);
    int sunionstore(QString destination, QString keys);
    bool set(QString key, QString value);
    int setrange(QString key, int offset, QString value);
    QStringList sort(QString key, QString conditions);
    int ttl(QString key);
    QString type(QString key);

signals:
    void connected();
    void disconnected();
    void returnData(QtRedis::Reply);

public slots:
    void connectHost(const QString &host, const quint16 port = 6379);
    void disconnectHost();

private slots:
    void slotConnected();
    void slotDisconnected();
    void response(QVariant);

private:
    Reader *reader;
    QString host;
    int port;
    QStringList responseData;
};

#endif // QTREDIS_H

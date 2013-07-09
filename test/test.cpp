#include "test.h"

Test::Test(QObject *parent) : QObject(parent)
{}

Test::~Test()
{
    delete redis;
}

void Test::slotMessage(QtRedis::Reply reply)
{
    QTime time;
    qDebug() << time.currentTime();
    qDebug() << "Channel:" << reply.channel << "Pattern:" << reply.pattern;
    qDebug() << reply.value.toString();
}

void Test::startTest()
{
    redis = new QtRedis("localhost",6379);
    connect(redis, SIGNAL(returnData(QtRedis::Reply)), this, SLOT(slotMessage(QtRedis::Reply)));

    QtRedis::Reply reply;

    if (!redis->openConnection())
    {
        qDebug() << "Could not connect to server...";
        exit(0);
    }

    qDebug() << "Connected to server...";

    //qDebug() << "AUTH:" << redis->auth("redisZzZ");

    // Set and Get example
    qDebug() << "SET:" << redis->set("key", "\"Hello World\"");
    qDebug() << "GET:" << redis->get("key");

    // Append to Key example
    qDebug() << "SET:" << redis->set("key", "\"Hello\"");
    qDebug() << "EXISTS:" << redis->exists("key");
    qDebug() << "GET:" << redis->get("key");
    qDebug() << "APPEND:" << redis->append("key", "\" World\"");
    qDebug() << "GET:" << redis->get("key");

    // Multi Set and Get example
    QMap<QString,QVariant> keypairs;
    keypairs["key1"] = QString("\"Hello\"");
    keypairs["key2"] = QString("\" world\"");
    qDebug() << "MSET:" << redis->mset(keypairs);
    qDebug() << "MGET:" << redis->mget("key1 key2 nonexisting");

    // Incr, incrby decr, decrby example.
    qDebug() << "SET:" << redis->set("count", "10");
    qDebug() << "INCR:" << redis->incr("count");
    qDebug() << "GET:" << redis->get("count");

    qDebug() << "INCRBY:" << redis->incrby("count",5);
    qDebug() << "GET:" << redis->get("count");

    qDebug() << "DECR:" << redis->decr("count");
    qDebug() << "GET:" << redis->get("count");

    qDebug() << "DECRBY:" << redis->decrby("count",5);
    qDebug() << "GET:" << redis->get("count");

    // SET and GET Range examples
    qDebug() << "SETRANGE:" << redis->setrange("key",6 ,"Redis");
    qDebug() << "GET:" << redis->get("key");

    qDebug() << "GETRANGE:" << redis->getrange("key",-5 ,-1);
    qDebug() << "GETRANGE:" << redis->getrange("key",0 ,4);
    qDebug() << "GETRANGE:" << redis->getrange("key",0 ,-1);


    //Hashmap example
    qDebug() << "HSET:" << redis->hset("hashmap","key1" ,"value1");
    qDebug() << "HSET:" << redis->hset("hashmap","key2" ,"value2");
    qDebug() << "HSET:" << redis->hset("hashmap","key3" ,"value3");
    qDebug() << "HGETALL:";

    QMap<QString,QVariant> hashmap = redis->hgetall("hashmap");
    QMapIterator<QString, QVariant> mi(hashmap);
    while (mi.hasNext())
    {
        mi.next();
        qDebug() << mi.key() << "=" << mi.value().toString();
    }

    qDebug() << "HVALS:" << redis->hvals("hashmap");


    // Raw Command example
    reply = redis->command("GET key");
    qDebug() << "RAW:" << "("<< reply.type << ")" << reply.value.toString();

    redis->subscribe("notifications");

    redis->psubscribe("news.*");

    //reply = redis->command("SUBSCRIBE notifications");
    //qDebug() << "("<< reply.type << ")" << reply.value.toStringList();

}

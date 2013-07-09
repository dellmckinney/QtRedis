#include "qtredis.h"

QtRedis::QtRedis(QString host, int port, QObject *parent) : QObject(parent)
{
    this->host = host;
    this->port = port;

    reader = new Reader(host, port);
    connect(reader, SIGNAL(response(QVariant)), this, SLOT(response(QVariant)));
}

QtRedis::~QtRedis()
{
    delete reader;
}

bool QtRedis::openConnection()
{
    connectHost(host,port);

    QEventLoop loop;
    connect(reader, SIGNAL(connected()), &loop, SLOT(quit()));
    loop.exec();

    if(!reader->isConnected())
    {
        return false;
    }

    return true;
}

void QtRedis::connectHost(const QString &host, const quint16 port)
{
    reader->connectHost(host,port);
    connect(reader, SIGNAL(connected()), this, SLOT(slotConnected()));
}

void QtRedis::disconnectHost()
{
    reader->disconnectHost();
}

void QtRedis::slotConnected()
{
    connect(reader, SIGNAL(disconnected()), this, SLOT(slotDisconnected()));
    emit connected();
}

void QtRedis::slotDisconnected()
{
    emit disconnected();
}

/* Redis Public API */
void QtRedis::publish(QString channel, QString message)
{
    QString cmd;
    cmd.append("PUBLISH ");
    cmd.append(channel);
    cmd.append(" ");
    cmd.append(message);

    reader->sendData(cmd);
}

void QtRedis::subscribe(QString channel)
{
    QString cmd;
    cmd.append("SUBSCRIBE ");
    cmd.append(channel);

    reader->sendData(cmd);
}

void QtRedis::unsubscribe(QString channel)
{
    QString cmd;
    cmd.append("UNSUBSCRIBE ");
    cmd.append(channel);

    reader->sendData(cmd);
}

void QtRedis::psubscribe(QString channel)
{
    QString cmd;
    cmd.append("PSUBSCRIBE ");
    cmd.append(channel);

    reader->sendData(cmd);
}

void QtRedis::punsubscribe(QString channel)
{
    QString cmd;
    cmd.append("PUNSUBSCRIBE ");
    cmd.append(channel);

    reader->sendData(cmd);
}

void QtRedis::response(QVariant response)
{
    QStringList res = response.toStringList();

    Reply reply;

    if (res[1] == "message")
    {
        reply.type = res[0];
        reply.message = res[1];
        reply.channel = res[2];
        reply.value = res[3];

        emit returnData(reply);
    }
    else if (res[1] == "pmessage")
    {
        reply.type = res[0];
        reply.message = res[1];
        reply.pattern = res[2];
        reply.channel = res[3];
        reply.value = res[4];

        emit returnData(reply);
    }
    else
    {
        responseData = res;
    }
}

QtRedis::Reply QtRedis::command(QString cmd)
{
    Reply reply;

    reader->sendData(cmd);

    QEventLoop loop;
    connect(reader, SIGNAL(response(QVariant)), &loop, SLOT(quit()));
    loop.exec();

    if(responseData[0] == "integer")
    {
        reply.type = responseData[0];
        reply.value = responseData[1].toInt();
    }
    else if(responseData[0] == "list")
    {
        reply.type = responseData[0];

        QStringList list;
        for(int i=1; i < responseData.length(); i++)
        {
            list << responseData[i];
        }

        reply.value = list;
    }
    else
    {
        reply.type = responseData[0];
        reply.value = responseData[1];
    }

    return reply;
}

int QtRedis::append(QString key, QString value)
{
    QString cmd("APPEND ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::auth(QString password)
{
    QString cmd("AUTH ");
    cmd.append(password);

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

QStringList QtRedis::blpop(QString key, int timeout)
{
    QString cmd("BLPOP ");

    cmd.append(key);
    cmd.append(" ");

    cmd.append(QString::number(timeout));

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

QStringList QtRedis::brpop(QString key, int timeout)
{
    QString cmd("BRPOP ");

    cmd.append(key);
    cmd.append(" ");

    cmd.append(QString::number(timeout));

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

QStringList QtRedis::brpoplpush(QString source, QString destination, int timeout)
{
    QString cmd("BRPOPLPUSH ");

    cmd.append(source);
    cmd.append(" ");

    cmd.append(destination);
    cmd.append(" ");

    cmd.append(QString::number(timeout));

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::decr(QString key)
{
    QString cmd("DECR ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::decrby(QString key,int interval)
{
    QString cmd("DECRBY ");
    cmd.append(key);

    cmd.append(" ");

    cmd.append(QString::number(interval));

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::del(QString key)
{
    QString cmd("DEL ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QString QtRedis::dump(QString key)
{
    QString cmd("DUMP ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toString();
}

bool QtRedis::exists(QString key)
{
    QString cmd("EXISTS ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

bool QtRedis::expire(QString key,int seconds)
{
    QString cmd("EXPIRE ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(seconds));

    Reply reply = command(cmd);

    return reply.value.toBool();
}

bool QtRedis::expireat(QString key, qint64 timestamp)
{
    QString cmd("EXPIREAT ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(timestamp));

    Reply reply = command(cmd);

    return reply.value.toBool();
}

QVariant QtRedis::eval(QString script, int numkeys, QString keys, QString arguments)
{
    QString cmd("EVAL ");
    cmd.append(script);

    cmd.append(" ");
    cmd.append(QString::number(numkeys));

    cmd.append(" ");
    cmd.append(keys);

    cmd.append(" ");
    cmd.append(arguments);

    Reply reply = command(cmd);

    return reply.value;
}

QVariant QtRedis::evalsha(QString sha1, int numkeys, QString keys, QString arguments)
{
    QString cmd("EVALSHA ");
    cmd.append(sha1);

    cmd.append(" ");
    cmd.append(QString::number(numkeys));

    cmd.append(" ");
    cmd.append(keys);

    cmd.append(" ");
    cmd.append(arguments);

    Reply reply = command(cmd);

    return reply.value;
}

QString QtRedis::get(QString key)
{
    QString cmd("GET ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toString();
}

QString QtRedis::getrange(QString key, int start, int stop)
{
    QString cmd("GETRANGE ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(start));

    cmd.append(" ");
    cmd.append(QString::number(stop));

    Reply reply = command(cmd);

    return reply.value.toString();
}

int QtRedis::hdel(QString key, QString field)
{
    QString cmd("HDEL ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::hexists(QString key, QString field)
{
    QString cmd("HEXISTS ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

QString QtRedis::hget(QString key, QString field)
{
    QString cmd("HGET ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    Reply reply = command(cmd);

    return reply.value.toString();
}

QMap<QString,QVariant> QtRedis::hgetall(QString key)
{
    QString cmd("HGETALL ");
    cmd.append(key);

    Reply reply = command(cmd);

    QMap<QString,QVariant> keypairs;
    QStringList list = reply.value.toStringList();

    for(int i=0; i<list.length(); i++)
    {
        QString k = list[i];
        i++;
        QString v = list[i];

        keypairs[k] = QString(v);
    }

    return keypairs;
}

int QtRedis::hincrby(QString key, QString field, int increment)
{
    QString cmd("HINCRBY ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    cmd.append(" ");
    cmd.append(QString::number(increment));

    Reply reply = command(cmd);

    return reply.value.toInt();
}

float QtRedis::hincrbyfloat(QString key, QString field, float increment)
{
    QString cmd("HINCRBYFLOAT ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    cmd.append(" ");
    cmd.append(QString::number(increment));

    Reply reply = command(cmd);

    return reply.value.toFloat();
}

QStringList QtRedis::hkeys(QString key)
{
    QString cmd("HKEYS ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::hlen(QString key)
{
    QString cmd("HLEN ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::hmget(QString key, QString field)
{
    QString cmd("HMGET ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

bool QtRedis::hmset(QString key, QMap<QString,QVariant> map)
{
    QString cmd("HMSET ");
    cmd.append(key);

    QMapIterator<QString, QVariant> i(map);

    while (i.hasNext())
    {
        i.next();
        cmd.append(" ");
        cmd.append(i.key());
        cmd.append(" ");
        cmd.append(i.value().toString());

    }

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

bool QtRedis::hset(QString key, QString field, QString value)
{
    QString cmd("HSET ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

bool QtRedis::hsetnx(QString key, QString field, QString value)
{
    QString cmd("HSETNX ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(field);

    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

QStringList QtRedis::hvals(QString key)
{
    QString cmd("HVALS ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::incr(QString key)
{
    QString cmd("INCR ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::incrby(QString key,int interval)
{
    QString cmd("INCRBY ");
    cmd.append(key);

    cmd.append(" ");

    cmd.append(QString::number(interval));

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::keys(QString pattern)
{
    QString cmd("KEYS ");
    cmd.append(pattern);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::lindex(QString key, int index)
{
    QString cmd("LINDEX ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(QString::number(index));

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::linsert(QString key, QString position,
            QString pivot, QString value)
{
    QString cmd("LINSERT ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(position);
    cmd.append(" ");
    cmd.append(pivot);
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();

}

int QtRedis::llen(QString key)
{
    QString cmd("LLEN ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QString QtRedis::lpop(QString key)
{
    QString cmd("LPOP ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toString();
}

int QtRedis::lpush(QString key, QString value)
{
    QString cmd("LPUSH ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::lpushx(QString key, QString value)
{
    QString cmd("LPUSHX ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::lrange(QString key, int start, int stop)
{
    QString cmd("LRANGE ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(QString::number(start));
    cmd.append(" ");
    cmd.append(QString::number(stop));

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::lrem(QString key, int count, QString value)
{
    QString cmd("LREM ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(QString::number(count));
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::lset(QString key, int index, QString value)
{
    QString cmd("LSET ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(QString::number(index));
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::ltrim(QString key, int start, int stop)
{
    QString cmd("LTRIM ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(QString::number(start));
    cmd.append(" ");
    cmd.append(QString::number(stop));

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

QStringList QtRedis::mget(QString key)
{
    QString cmd("MGET ");
        cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

bool QtRedis::migrate(QString host, int port, QString key, int database, int timeout)
{
    QString cmd("MIGRATE ");
    cmd.append(host);

    cmd.append(" ");
    cmd.append(QString::number(port));

    cmd.append(" ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(database));

    cmd.append(" ");
    cmd.append(QString::number(timeout));

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

bool QtRedis::move(QString key, int database)
{
    QString cmd("MOVE ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(database));

    Reply reply = command(cmd);

    return reply.value.toBool();
}

bool QtRedis::mset(QMap<QString, QVariant> map)
{
    QString cmd("MSET");

    QMapIterator<QString, QVariant> i(map);
    while (i.hasNext())
    {
        i.next();
        cmd.append(" ");
        cmd.append(i.key());
        cmd.append(" ");
        cmd.append(i.value().toString());

    }

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

QVariant QtRedis::object(QString subcommand, QString arguments)
{
    QString cmd("OBJECT ");
    cmd.append(subcommand);

    cmd.append(" ");
    cmd.append(arguments);

    Reply reply = command(cmd);

    return reply.value;
}

bool QtRedis::persist(QString key)
{
    QString cmd("PERSIST ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

bool QtRedis::pexpire(QString key, qint64 mseconds)
{
    QString cmd("PEXPIRE ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(mseconds));

    Reply reply = command(cmd);

    return reply.value.toBool();
}

bool QtRedis::pexpireat(QString key, qint64 mstimestamp)
{
    QString cmd("PEXPIREAT ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(mstimestamp));

    Reply reply = command(cmd);

    return reply.value.toBool();
}

int QtRedis::pttl(QString key)
{
    QString cmd("PTTL ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::rename(QString key, QString newkey)
{
    QString cmd("RENAME ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(newkey);

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

bool QtRedis::renamex(QString key, QString newkey)
{
    QString cmd("RENAMEX ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(newkey);

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

bool QtRedis::restore(QString key, int ttl, QString value)
{
    QString cmd("RESTORE ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(ttl));

    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

QString QtRedis::rpop(QString key)
{
    QString cmd("RPOP ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toString();
}

QString QtRedis::rpoplpush(QString source, QString destination)
{
    QString cmd("RPOPLPUSH ");
    cmd.append(source);

    cmd.append(" ");
    cmd.append(destination);

    Reply reply = command(cmd);

    return reply.value.toString();
}

int QtRedis::rpush(QString key, QString value)
{
    QString cmd("RPUSH ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

int QtRedis::rpushx(QString key, QString value)
{
    QString cmd("RPUSHX ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::sadd(QString key, QString member)
{
    QString cmd("SADD ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(member);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

int QtRedis::scard(QString key)
{
    QString cmd("SCARD ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::scriptexists(QString script)
{
    QString cmd("SCRIPT EXISTS ");
    cmd.append(script);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

QString QtRedis::scriptflush()
{
    QString cmd("SCRIPT FLUSH");

    Reply reply = command(cmd);

    return reply.value.toString();
}

QString QtRedis::scriptkill()
{
    QString cmd("SCRIPT KILL");

    Reply reply = command(cmd);

    return reply.value.toString();
}

QString QtRedis::scriptload(QString script)
{
    QString cmd("SCRIPT LOAD ");
    cmd.append(script);

    Reply reply = command(cmd);

    return reply.value.toString();
}

QStringList QtRedis::sdiff(QString keys)
{
    QString cmd("SDIFF ");
    cmd.append(keys);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::sdiffstore(QString destination, QString keys)
{
    QString cmd("SDIFFSTORE ");
    cmd.append(destination);

    cmd.append(" ");
    cmd.append(keys);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::sinter(QString keys)
{
    QString cmd("SINTER ");
    cmd.append(keys);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::sinterstore(QString destination, QString keys)
{
    QString cmd("SINTERSTORE ");
    cmd.append(destination);

    cmd.append(" ");
    cmd.append(keys);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::sismember(QString key, QString member)
{
    QString cmd("SISMEMBER ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(member);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

QStringList QtRedis::smembers(QString key)
{
    QString cmd("SMEMBERS ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

bool QtRedis::smove(QString source, QString destination, QString member)
{
    QString cmd("SMOVE ");
    cmd.append(source);

    cmd.append(" ");
    cmd.append(destination);

    cmd.append(" ");
    cmd.append(member);

    Reply reply = command(cmd);

    return reply.value.toBool();
}

QString QtRedis::spop(QString key)
{
    QString cmd("SPOP ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toString();
}

QStringList QtRedis::srandmember(QString key, int count)
{
    QString cmd("SRANDMEMBER ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(count));

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::srem(QString key, QString member)
{
    QString cmd("SREM ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(member);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::sunion(QString keys)
{
    QString cmd("SUNION ");
    cmd.append(keys);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::sunionstore(QString destination, QString keys)
{
    QString cmd("SUNIONSTORE ");
    cmd.append(destination);

    cmd.append(" ");
    cmd.append(keys);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

bool QtRedis::set(QString key, QString value)
{
    QString cmd("SET ");
    cmd.append(key);
    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    if(reply.value.toString() == "OK")
        return true;

    return false;
}

int QtRedis::setrange(QString key, int offset, QString value)
{
    QString cmd("SETRANGE ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(QString::number(offset));

    cmd.append(" ");
    cmd.append(value);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QStringList QtRedis::sort(QString key, QString conditions)
{
    QString cmd("SORT ");
    cmd.append(key);

    cmd.append(" ");
    cmd.append(conditions);

    Reply reply = command(cmd);

    return reply.value.toStringList();
}

int QtRedis::ttl(QString key)
{
    QString cmd("TTL ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toInt();
}

QString QtRedis::type(QString key)
{
    QString cmd("TYPE ");
    cmd.append(key);

    Reply reply = command(cmd);

    return reply.value.toString();
}

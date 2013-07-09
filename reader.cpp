#include "reader.h"

Reader::Reader(QString host, int port)
{
    this->host = host;
    this->port = port;

    socket = new QTcpSocket;
    reading = false;

    connect(socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketConnectionClosed()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

Reader::~Reader()
{
    delete socket;
}

void Reader::connectHost(const QString &host, quint16 port)
{

    if (reading)
        return;

    socket->connectToHost(host, port);

}

bool Reader::isConnected() const
{
    return reading;
}

void Reader::disconnectHost()
{
    socket->disconnectFromHost();
}

void Reader::socketConnected()
{
    reading = true;
    emit connected();
}

void Reader::socketConnectionClosed()
{
    reading = false;
    emit disconnected();
}

void Reader::socketReadyRead()
{
    QString reply("");

    reply.append(socket->readAll());

    readLine(reply);
}

void Reader::socketError(QAbstractSocket::SocketError e)
{
    emit error(e);
}

void Reader::sendData(const QString &data)
{
    if (!reading)
        return;

    QTextStream stream(socket);

    QString test(data);
    int k;

    QChar c,next;
    QStringList parts;
    QString buffer="";
    bool open=false;
    for(k=0;k<test.length();k++)
    {
        c=test.at(k);
        if(open)
        {
            next=k<test.length()-1?test
                       .at(k+1):' ';
            if(c=='\\'&&next=='"')
            {
                buffer+='"';
                k++;
            }
            else if(c=='"')
                open=false;
            else
                buffer+=c;
        }
        else
        {
            if(!c.isSpace())
            {
                if(c=='"')
                    open=true;
                else
                    buffer+=c;
            }
            else if(!buffer.isEmpty())
            {
                parts<<buffer;
                buffer="";
            }
        }
    }

    if(!buffer.isEmpty())
    {
        parts<<buffer;
    }

    QString bin="";
    bin.append(QString("*%1\r\n")
       .arg(parts.length()));
    int i;
    for(i=0;i<parts.length();i++)
    {
        bin.append(QString("$%1\r\n")
           .arg(parts.at(i)
           .length()));

        bin.append(QString("%1\r\n")
           .arg(parts.at(i)));
    }

    stream<<bin;
    stream.flush();
}

void Reader::readLine(const QString &reply)
{
    QChar first=reply.at(0);
    QString value;
    QStringList result;

    if(first=='+')
    {
        value=reply.mid(1);
        value.chop(2);

        result << "string" << value;
    }
    else if(first=='-')
    {
        value=reply.mid(1);
        value.chop(2);

        result << "error" << value;
    }
    else if(first==':')
    {
        value=reply.mid(1);
        value.chop(2);

        result << "integer" << value;

    } else if(first=='$')
    {
        int index=reply.indexOf("\r\n");
        int len=reply.mid(1,index-1)
                     .toInt();
        if(len==-1)
            value="NULL";
        else
            value=reply.mid(index+2,len);
        result<< "bulk" << value;

    } else if(first=='*')
    {
        int index=reply.indexOf("\r\n");
        int count=reply.mid(1,index-1)
                       .toInt();
        int i;
        int len;
        int pos=index+2;
        result << "list";

        for(i=0;i<count;i++)
        {
            index=reply.indexOf("\r\n",pos);
            len=reply.mid(pos+1,index-pos-1)
                     .toInt();
            if(len==-1)
                result<<"NULL";
            else
                result<<reply.mid(index+2,len);

            pos=index+2+len+2;
        }
    }

    emit response(result);
}

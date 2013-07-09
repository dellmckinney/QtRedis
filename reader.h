#ifndef READER_H
#define READER_H

#include <QObject>
#include <QVariant>
#include <QStringList>
#include <QTcpSocket>
#include <QSocketNotifier>
#include <QByteArray>

#include <QDebug>

class Reader : public QObject
{
    Q_OBJECT

public:
    Reader(QString host, int port);
    ~Reader();

    void connectHost(const QString &host, quint16 port);
    bool isConnected() const;
    void disconnectHost();

signals:
    void response(QVariant);
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);

public slots:
    void socketConnected();
    void socketConnectionClosed();
    void socketReadyRead();
    void socketError(QAbstractSocket::SocketError);
    void sendData(const QString &);
    void readLine(const QString &);

private:
    QTcpSocket *socket;
    bool reading;
    QSocketNotifier *notifier;
    QString host;
    int port;
};

#endif // READER_H

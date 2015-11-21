#ifndef EVENTHTTPSERVER_H
#define EVENTHTTPSERVER_H

#include <QObject>
#include <QTcpServer>

class EventHttpServer : public QTcpServer
{
    Q_OBJECT
private:
    QList<QTcpSocket *> clients;

    quint16 port;
    bool listenAny;

public:
    EventHttpServer(quint16 port, bool listenAny, QObject *parent = 0);

    bool start();

    void send(QString event);

private slots:
    void serverNewConnection();

    void sockReadyRead();

    void sockDisconnected();

signals:
    void received(QString event);

    void connectionStatusChanged(bool connected);
};

#endif // EVENTHTTPSERVER_H

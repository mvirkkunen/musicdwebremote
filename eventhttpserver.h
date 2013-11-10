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

    void start();

    void send(QString event);

protected:
    virtual void incomingConnection(int handle);

private slots:
    void sockReadyRead();

    void sockDisconnected();

signals:
    void received(QString event);

    void connectionStatusChanged(bool connected);
};

#endif // EVENTHTTPSERVER_H

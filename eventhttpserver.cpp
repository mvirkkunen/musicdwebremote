#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>

#include "eventhttpserver.h"

EventHttpServer::EventHttpServer(quint16 port, bool listenAny, QObject *parent)
    : QTcpServer(parent), port(port), listenAny(listenAny)
{

}

void EventHttpServer::start() {
    listen(listenAny ? QHostAddress::Any : QHostAddress::LocalHost, port);
}

void EventHttpServer::incomingConnection(int handle)
{
    QTcpSocket *sock = new QTcpSocket(this);

    connect(sock, SIGNAL(readyRead()), this, SLOT(sockReadyRead()));
    connect(sock, SIGNAL(disconnected()), this, SLOT(sockDisconnected()));
    sock->setSocketDescriptor(handle);
}

void EventHttpServer::send(QString event)
{
    QByteArray bytes = QString("event: command\r\ndata: %1\r\n\r\n").arg(event).toUtf8();

    foreach (QTcpSocket *sock, clients)
        sock->write(bytes);
}

void EventHttpServer::sockReadyRead()
{
    QTcpSocket *sock = (QTcpSocket *)sender();

    if (clients.contains(sock))
        return; // discard data from clients

    if (!sock->canReadLine())
        return; // wait until request line

    QString requestLine = QString(sock->readLine(1024));
    QStringList parts = requestLine.split(' ', QString::SkipEmptyParts);
    if (parts.length() < 2) {
        sock->close();
        return;
    }

    QString method = parts[0], path = parts[1];

    QString status = "HTTP/1.1 200 OK";
    QString contentType = "text/plain";
    bool close = true;

    if (method == "GET" && path == "/events") {
        if (clients.empty())
            emit connectionStatusChanged(true);

        clients.append(sock);

        contentType = "text/event-stream";
        close = false;

        qDebug() << "New client";
    } else if (method == "POST" && path.length() > 1) {
        emit received(path.mid(1));
    } else {
        status = "HTTP/1.1 400 Bad Request";
    }

    QTextStream out(sock);

    out << status << "\r\n"
        << "Access-Control-Allow-Origin: *\r\n"
        << "Content-Type: " << contentType << "\r\n";

    if (close)
        out << "Content-Length: 0\r\n";

    out << "\r\n";

    if (close) {
        sock->close();
        sock->deleteLater();
    }
}

void EventHttpServer::sockDisconnected()
{
    QTcpSocket *sock = (QTcpSocket *)sender();

    if (clients.contains(sock)) {
        clients.removeOne(sock);
        qDebug() << "Client quit";

        if (clients.empty())
            emit connectionStatusChanged(false);
    }

    sock->deleteLater();
}

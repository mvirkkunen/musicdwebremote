#include <QObject>
#include <QApplication>
#include <QByteArray>
#include <QHostAddress>
#include <QList>

#include <QxtHttpSessionManager>
#include <QxtAbstractWebService>
#include <QxtWebRequestEvent>
#include <QxtWebPageEvent>
#include <QxtFifo>

#include <QIcon>
#include <QSystemTrayIcon>

#include <QxtGlobalShortcut>

class EventWebService : public QxtAbstractWebService
{
    Q_OBJECT

private:
    QList<QIODevice *> eventFifos;

public:
    EventWebService(QxtAbstractWebSessionManager *manager)
        : QxtAbstractWebService(manager)
    {

    }

    virtual void pageRequestedEvent(QxtWebRequestEvent *event)
    {
        QString path = event->url.path();

        QxtWebPageEvent *response;

        if (event->method == "GET" && path == "/") {
            QIODevice *fifo = new QxtFifo();
            fifo->open(QIODevice::ReadWrite);

            connect(fifo, SIGNAL(destroyed(QObject*)), this, SLOT(fifoDestroyed(QObject*)));

            eventFifos.append(fifo);

            response = new QxtWebPageEvent(-1, event->requestID, fifo);
            response->contentType = QByteArray("text/event-stream");
        } else if (event->method == "POST" && path.length() > 1){
            emit received(path.mid(1));

            response = new QxtWebPageEvent(-1, event->requestID, QByteArray("ok\n"));
            response->contentType = QByteArray("text/plain");
        } else {
            response = new QxtWebPageEvent(-1, event->requestID, QByteArray("bad request\n"));
            response->contentType = QByteArray("text/plain");
            response->status = 400;
            response->statusMessage = QByteArray("Bad Request");
        }

        response->headers.insert("access-control-allow-origin", "*");

        postEvent(response);
    }

    void send(QString event)
    {
        QByteArray bytes = QString("event: command\r\ndata: %1\r\n\r\n").arg(event).toUtf8();

        foreach (QIODevice *fifo, eventFifos)
            fifo->write(bytes);
    }

private slots:
    void fifoDestroyed(QObject *obj)
    {
        eventFifos.removeOne(static_cast<QIODevice *>(obj));
    }

signals:
    void received(QString event);
};

class MusicdRemote : public QObject
{
    Q_OBJECT

    QxtHttpSessionManager *manager;
    EventWebService *service;

    QSystemTrayIcon *trayIcon;
    QList<QString> states;

    QxtGlobalShortcut *shortcutKey;

public:
    MusicdRemote(const QHostAddress &address, quint16 port)
    {
        states << "stop" << "pause" << "play";

        initService(address, port);
        initUI();

        setState("stop");

        trayIcon->show();
        manager->start();
    }

private:
    void initService(const QHostAddress &address, quint16 port)
    {
        manager = new QxtHttpSessionManager();
        service = new EventWebService(manager);

        manager->setStaticContentService(service);
        manager->setListenInterface(address);
        manager->setConnector(QxtHttpSessionManager::HttpServer);
        manager->setPort(port);

        connect(service, SIGNAL(received(QString)),
                this, SLOT(setState(QString)));
    }

    void initUI()
    {
        trayIcon = new QSystemTrayIcon();

        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconClick(QSystemTrayIcon::ActivationReason)));

        shortcutKey = new QxtGlobalShortcut(QKeySequence("Pause"));

        connect(shortcutKey, SIGNAL(activated()),
                this, SLOT(togglePlay()));
    }

private slots:
    void setState(QString state)
    {
        if (states.contains(state))
            trayIcon->setIcon(QIcon(QString(":/icons/%1").arg(state)));
    }

    void iconClick(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            service->send("togglePlay");
    }

    void togglePlay()
    {
        service->send("togglePlay");
    }
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MusicdRemote remote(QHostAddress::LocalHost, 48278);
    
    return app.exec();
}

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QIcon>
#include <QList>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

#include <qxt/qxtglobalshortcut.h>

#include "eventhttpserver.h"
#include "trackinfo.h"
#include "popupwindow.h"

class MusicdRemote : public QObject
{
    Q_OBJECT

    EventHttpServer *server;

    QList<QString> states;

    QMenu *trayMenu;
    QAction *copyTrackAction;
    QSystemTrayIcon *trayIcon;

    PopupWindow *popup;

    TrackInfo trackInfo;

public:
    MusicdRemote(quint16 port, bool listenAny, QMap<QString, QString> mappings)
    {
        states << "stop" << "pause" << "play";

        initUI(mappings);

        connectionStatusChanged(false);

        trayIcon->show();

        server = new EventHttpServer(port, listenAny, this);

        connect(server, SIGNAL(received(QString)),
                this, SLOT(received(QString)));
        connect(server, SIGNAL(connectionStatusChanged(bool)),
                this, SLOT(connectionStatusChanged(bool)));

        popup = new PopupWindow(server, trayIcon);

        server->start();
    }

private:

    void initUI(QMap<QString, QString> mappings)
    {
        trayMenu = new QMenu();

        copyTrackAction = trayMenu->addAction("Copy track info", this, SLOT(copyTrackInfo()));
        trayMenu->addAction("Quit", this, SLOT(quit()));

        trayIcon = new QSystemTrayIcon(QIcon(":/icons/unknown"));
        trayIcon->setContextMenu(trayMenu);

        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconClick(QSystemTrayIcon::ActivationReason)));

        QMapIterator<QString, QString> i(mappings);
        while (i.hasNext()) {
            i.next();

            QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(QKeySequence(i.key()));
            shortcut->setProperty("_command", i.value());

            connect(shortcut, SIGNAL(activated()),
                    this, SLOT(shortcutActivated()));
        }
    }

private slots:
    void connectionStatusChanged(bool connected)
    {
        if (!connected) {
            trayIcon->setIcon(QIcon(":/tray/no_connection"));
            copyTrackAction->setEnabled(false);
        }
    }

    void received(QString message)
    {
        if (message.startsWith("state/")) {
            QString state = message.mid(6);

            if (states.contains(state))
                trayIcon->setIcon(QIcon(QString(":/tray/%1").arg(state)));
        } else if (message.startsWith("command/")) {
            QString command = message.mid(8);

            server->send(command);
        } else if (message.startsWith("track?")) {
            trackInfo.parse(message);

            popup->setTrackInfo(trackInfo);
            copyTrackAction->setEnabled(trackInfo.valid());
        }
    }

    void iconClick(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger)
            popup->showAtButton();
        else if (reason == QSystemTrayIcon::MiddleClick)
            server->send("togglePlay");
    }

    void shortcutActivated()
    {
        QString command = ((QxtGlobalShortcut *)sender())->property("_command").toString();
        if (command != "")
            server->send(command);
    }

    void quit()
    {
        trayIcon->hide();
        QApplication::exit();
    }

    void copyTrackInfo()
    {
        if (trackInfo.valid())
            QApplication::clipboard()->setText(trackInfo.toString());
    }
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("musicdwebremote");

    QMap<QString, QString> mappings;
    bool listenAny = false;

    foreach (QString arg, app.arguments())
    {
        if (arg == "-listenany") {
            listenAny = true;
            continue;
        }

        int p = arg.indexOf('=');
        if (p > 0 && p < arg.length() - 1)
            mappings.insert(arg.left(p), arg.mid(p + 1));
    }

    MusicdRemote remote(48278, listenAny, mappings);
    
    return app.exec();
}

#include <QObject>
#include <QApplication>
#include <QList>

#include <QIcon>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

#include <qxt/qxtglobalshortcut.h>

#include <eventhttpserver.h>

class MusicdRemote : public QObject
{
    Q_OBJECT

    EventHttpServer *server;

    QList<QString> states;

    QMenu *trayMenu;
    QSystemTrayIcon *trayIcon;

    QMap<QxtGlobalShortcut *, QString> shortcuts;

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
        server->start();
    }

private:

    void initUI(QMap<QString, QString> mappings)
    {
        trayMenu = new QMenu();

        addMenuCommand("Play", "togglePlay");
        addMenuCommand("Stop", "stop");
        trayMenu->addSeparator();
        addMenuCommand("Next track", "next");
        addMenuCommand("Previous track", "prev");
        trayMenu->addSeparator();
        trayMenu->addAction("Quit", this, SLOT(quit()));

        connect(trayMenu, SIGNAL(triggered(QAction*)),
                this, SLOT(trayMenuTriggered(QAction *)));

        trayIcon = new QSystemTrayIcon(QIcon(":/icons/unknown"));
        trayIcon->setContextMenu(trayMenu);

        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(iconClick(QSystemTrayIcon::ActivationReason)));

        QMapIterator<QString, QString> i(mappings);
        while (i.hasNext()) {
            i.next();

            QxtGlobalShortcut *shortcut = new QxtGlobalShortcut(QKeySequence(i.key()));
            shortcuts.insert(shortcut, i.value());

            connect(shortcut, SIGNAL(activated()),
                    this, SLOT(shortcutActivated()));
        }
    }

    void addMenuCommand(QString text, QString command)
    {
        QAction *action = new QAction(text, trayMenu);

        action->setData(command);
        trayMenu->addAction(action);
    }

private slots:
    void connectionStatusChanged(bool connected)
    {
        if (!connected)
            trayIcon->setIcon(QIcon(":/icons/unknown"));

        foreach (QAction *action, trayMenu->actions()) {
            if (action->data().type() == QVariant::String)
                action->setEnabled(connected);
        }
    }

    void received(QString message)
    {
        if (message.startsWith("state/")) {
            QString state = message.mid(6);

            if (states.contains(state)) {
                trayIcon->setIcon(QIcon(QString(":/icons/%1").arg(state)));
                trayMenu->actions()[0]->setText((state == "play") ? "Pause" : "Play");
            }
        } else if (message.startsWith("command/")) {
            QString command = message.mid(8);

            server->send(command);
        }
    }

    void iconClick(QSystemTrayIcon::ActivationReason reason)
    {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            server->send("togglePlay");
    }

    void shortcutActivated()
    {
        QString command = shortcuts.value(static_cast<QxtGlobalShortcut *>(sender()));

        if (command != "")
            server->send(command);
    }

    void trayMenuTriggered(QAction *action)
    {
        server->send(action->data().value<QString>());
    }

    void quit()
    {
        QApplication::exit();
    }
};

#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

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

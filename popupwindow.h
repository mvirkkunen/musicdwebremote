#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QBoxLayout>
#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QSystemTrayIcon>

#include <trackinfo.h>
#include <eventhttpserver.h>

class PopupWindow : public QDialog
{
    Q_OBJECT

    EventHttpServer *server;

    QSystemTrayIcon *anchor;

    QBoxLayout *vbox;

    QList<QPushButton *> buttons;

    QPushButton *playButton;
    QLabel *label;

public:
    PopupWindow(EventHttpServer *server, QSystemTrayIcon *anchor, QWidget *parent = 0);

    void setTrackInfo(TrackInfo &trackInfo);
    void showAtButton();

private:
    QPushButton *addButton(QBoxLayout *box, QString command, QString icon, QString toolTip);

    void updatePosition();

private slots:
    void buttonClicked();

    void connectionStatusChanged(bool connected);
    void received(QString message);
};

#endif // POPUPWINDOW_H

#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include <QList>
#include <QDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

#include <trackinfo.h>
#include <eventhttpserver.h>

class PopupWindow : public QDialog
{
    Q_OBJECT

    EventHttpServer *server;

    QList<QPushButton *> buttons;

    QPushButton *playButton;
    QLabel *label;

public:
    PopupWindow(EventHttpServer *server, QWidget *parent = 0);

    void showNear(QRect buttonRect);
    void setTrackInfo(TrackInfo &trackInfo);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    QGridLayout *grid;

    QPushButton *addButton(QString command, int index, QString icon, QString toolTip);

    void ensureWithinScreen();
    QRect ensureWithinScreen(QRect rect);

private slots:
    void buttonClicked();

    void connectionStatusChanged(bool connected);
    void received(QString message);
};

#endif // POPUPWINDOW_H

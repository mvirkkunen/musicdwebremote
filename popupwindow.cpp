#include "popupwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QPainter>
#include <QStringBuilder>
#include <QUrl>

PopupWindow::PopupWindow(EventHttpServer *server, QSystemTrayIcon *anchor, QWidget *parent) :
    QDialog(parent),
    server(server),
    anchor(anchor)
{
    QFile styleFile(":/styles/popupwindow");
    styleFile.open(QFile::ReadOnly);

    setStyleSheet(styleFile.readAll());

    vbox = new QBoxLayout(QBoxLayout::TopToBottom, this);
    vbox->setSpacing(10);

    QBoxLayout *buttonBox = new QBoxLayout(QBoxLayout::LeftToRight);

    playButton = addButton(buttonBox, "togglePlay", "play", "Play");
    addButton(buttonBox, "stop", "stop", "Stop");
    addButton(buttonBox, "rewindOrPrev", "prev", "Previous track");
    addButton(buttonBox, "next", "next", "Next track");

    vbox->addLayout(buttonBox);

    label = new QLabel();
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setWordWrap(true);
    label->setMaximumWidth(320 - vbox->margin());
    label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);

    vbox->addWidget(label);

    setMinimumWidth(320);

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Popup | Qt::FramelessWindowHint);

    connect(server, SIGNAL(received(QString)),
            this, SLOT(received(QString)));
    connect(server, SIGNAL(connectionStatusChanged(bool)),
            this, SLOT(connectionStatusChanged(bool)));

    connectionStatusChanged(false);
}

QPushButton *PopupWindow::addButton(QBoxLayout *box, QString command, QString icon, QString toolTip)
{
    QPushButton *button = new QPushButton(this);
    button->setFlat(true);
    button->setProperty("_command", command);
    button->setIcon(QIcon(QString(":/button/%1").arg(icon)));
    button->setToolTip(toolTip);

    box->addWidget(button);

    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    buttons.append(button);

    return button;
}

void PopupWindow::updatePosition()
{
    QRect anchorRect = anchor->geometry(),
          screen = QApplication::desktop()->availableGeometry(anchorRect.topLeft());

    // Assuming the tray is either at the top or the bottom for now
    bool atTop = (anchorRect.y() < screen.center().y());

    vbox->setDirection(atTop ? QBoxLayout::TopToBottom : QBoxLayout::BottomToTop);

    // Adjust window size to shrink to fit
    adjustSize();

    QRect geom = geometry();

    // Center horizontally
    geom.moveLeft(anchorRect.center().x() - geom.width() / 2);

    // Move above/below tray icon
    if (atTop)
        geom.moveTop(anchorRect.bottom() + 2);
    else
        geom.moveBottom(anchorRect.top() - 2);

    // Ensure window is on screen
    if (geom.right() > screen.right())
        geom.moveRight(screen.right());

    if (geom.left() < screen.left())
        geom.moveLeft(screen.left());

    setGeometry(geom);
}

void PopupWindow::setTrackInfo(TrackInfo &trackInfo)
{
    label->setText(trackInfo.toHTML());

    updatePosition();
}

void PopupWindow::showAtButton()
{
    updatePosition();
    show();
}

void PopupWindow::buttonClicked()
{
    QString command = ((QPushButton *)sender())->property("_command").toString();

    server->send(command);
}

void PopupWindow::connectionStatusChanged(bool connected)
{
    if (connected)
        label->setText("(no track)");
    else
        label->setText("<div style=\"text-align: center; padding: 20px;\">Disconnected.</div>");

    foreach (QPushButton *button, buttons)
        button->setEnabled(connected);

    updatePosition();
}

void PopupWindow::received(QString message)
{
    if (message.startsWith("state/")) {
        bool playing = (message == "state/play");

        playButton->setIcon(QIcon(playing
            ? ":/button/pause"
            : ":/button/play"));
    }
}

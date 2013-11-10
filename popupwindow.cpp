#include "popupwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QFile>
#include <QPainter>
#include <QStringBuilder>
#include <QUrl>

PopupWindow::PopupWindow(EventHttpServer *server, QWidget *parent) :
    QDialog(parent),
    server(server)
{
    QFile styleFile(":/styles/popupwindow");
    styleFile.open(QFile::ReadOnly);

    setStyleSheet(styleFile.readAll());

    grid = new QGridLayout(this);
    grid->setMargin(10);
    grid->setSpacing(10);

    playButton = addButton("togglePlay", 0, "play", "Play");
    addButton("stop", 1, "stop", "Stop");
    addButton("rewindOrPrev", 2, "prev", "Previous track");
    addButton("next", 3, "next", "Next track");

    label = new QLabel();
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setWordWrap(true);
    label->setMaximumWidth(320 - grid->margin());
    grid->addWidget(label, 1, 0, 1, 4);

    setLayout(grid);

    setMinimumWidth(320);

    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Popup | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(server, SIGNAL(received(QString)),
            this, SLOT(received(QString)));
    connect(server, SIGNAL(connectionStatusChanged(bool)),
            this, SLOT(connectionStatusChanged(bool)));

    connectionStatusChanged(false);
}

QPushButton *PopupWindow::addButton(QString command, int index, QString icon, QString toolTip)
{
    QPushButton *button = new QPushButton(this);
    button->setFlat(true);
    button->setProperty("_command", command);
    button->setIcon(QIcon(QString(":/button/%1").arg(icon)));
    button->setToolTip(toolTip);

    grid->addWidget(button, 0, index);

    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));

    buttons.append(button);

    return button;
}

void PopupWindow::showNear(QRect buttonRect)
{
    QRect screen = QApplication::desktop()->availableGeometry(buttonRect.topLeft());

    layout()->update();
    layout()->activate();

    adjustSize();

    QRect geom = geometry();

    // Assuming the tray is either at the top or the bottom for now

    // Center horizontally
    geom.moveLeft(buttonRect.center().x() - geom.width() / 2);

    // Move window above/below tray icon
    if (buttonRect.y() < screen.center().y())
        geom.moveTop(buttonRect.bottom() + 2); // open below
    else
        geom.moveBottom(buttonRect.top() - 2); // open above

    setGeometry(ensureWithinScreen(geom));

    show();
}

void PopupWindow::ensureWithinScreen()
{
    adjustSize();

    setGeometry(ensureWithinScreen(geometry()));
}

QRect PopupWindow::ensureWithinScreen(QRect geom)
{
    QRect screen = QApplication::desktop()->availableGeometry(geom.center());

    // Ensure window is entirely on screen

    if (geom.right() > screen.right())
        geom.moveRight(screen.right());

    if (geom.left() < screen.left())
        geom.moveLeft(screen.left());

    return geom;
}

void PopupWindow::setTrackInfo(TrackInfo &trackInfo)
{
    label->setText(trackInfo.toHTML());

    ensureWithinScreen();
}

void PopupWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter p(this);

    p.setPen(QColor(0, 0, 255, 48));
    p.setBrush(QColor(240, 248, 255, 220));
    p.drawRect(0, 0, width() - 1, height() - 1);

    //border: 1px solid rgba(0, 0, 255, 32);
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

    ensureWithinScreen();

    foreach (QPushButton *button, buttons)
        button->setEnabled(connected);
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

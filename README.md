## musicdwebremote

An application that enables you to remote control [https://github.com/mvirkkunen/musicdweb](musicdweb) via global hotkeys. Also displays a tray icon with the current state.

## Prerequisites

* Qt 4.8
* [http://libqxt.org/](LibQxt) (git version)

## Usage

Starting musicdwebremote without arguments shows state of musicdweb on the system tray and lets you toggle play/pause by clicking on the tray icon.

To assign global hotkeys, list them as arguments in the format "KeyName=command". The key names are listed in [http://qt-project.org/doc/qt-4.8/qt.html#Key-enum](Qt documentation) (the part after Qt::Key_). For example:

  ./musicdwebremote Pause=togglePlay F12=next

Maps the Pause/Break key to "toggle play" and the F12 key to "next track". The available commands in the current version of musicdweb are as follows:

* togglePlay
* play
* pause
* stop
* prev
* next
* rewindOrPrev

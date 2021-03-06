## musicdwebremote

An application that enables you to remote control [musicdweb](https://github.com/mvirkkunen/musicdweb) via global hotkeys. Also displays a tray icon with the current state.

## Prerequisites

* Qt 5.5 (tested, may work on outdated versions)

## Usage

Starting musicdwebremote without arguments shows state of musicdweb on the system tray and lets you toggle play/pause by clicking on the tray icon.

To assign global hotkeys, list them as arguments in the format "KeyName=command". The key names are listed in [Qt's documentation](http://qt-project.org/doc/qt-4.8/qt.html#Key-enum) (the part after Qt::Key_). For example:

    ./musicdwebremote Pause=togglePlay F1=next

Maps the Pause/Break key to "toggle play" and the F1 key to "next track". The available commands in the current version of musicdweb are as follows:

* togglePlay
* play
* pause
* stop
* prev
* next
* rewindOrPrev

## Windows

For use on Windows with dynamic linking, make sure the following DLL files can be found e.g. by copying them into the same directory with the .exe:

* Qt5Core.dll
* Qt5Gui.dll
* Qt5Network.dll
* Qt5Svg.dll
* Qt5Widgets.dll

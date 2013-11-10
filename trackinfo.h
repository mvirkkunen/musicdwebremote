#ifndef TRACKINFO_H
#define TRACKINFO_H

#include <QString>

class TrackInfo
{
private:

    int id;
    QString title;
    QString artist;
    QString album;

public:
    TrackInfo();

    void parse(QString info);

    bool valid();
    QString toString();
    QString toHTML();
};

#endif // TRACKINFO_H

#include "trackinfo.h"

#include <QTextDocument>
#include <QUrl>

TrackInfo::TrackInfo()
{
}

void TrackInfo::parse(QString info)
{
    QUrl url(info);

    id = url.queryItemValue("id").toInt();
    title = QUrl::fromPercentEncoding(url.queryItemValue("title").toUtf8());
    artist = QUrl::fromPercentEncoding(url.queryItemValue("artist").toUtf8());
    album = QUrl::fromPercentEncoding(url.queryItemValue("album").toUtf8());
}

bool TrackInfo::valid()
{
    return id != 0;
}

QString TrackInfo::toString()
{
    if (!valid())
        return "(no track)";

    QString s;

    if (title != "")
        s += title;
    else
        s += "(no title)";

    if (artist != "")
        s += QString(" by %0").arg(artist);

    if (album != "")
        s += QString(" on %0").arg(album);

    return s;
}

QString TrackInfo::toHTML()
{
    if (!valid())
        return "(no track)";

    QString s;

    if (title != "")
        s += QString("<strong>%1</strong>").arg(Qt::escape(title));
    else
        s += "<strong>(no title)</strong>";

    if (artist != "")
        s += QString("<br />by <strong>%1</strong>").arg(Qt::escape(artist));

    if (album != "")
        s += QString("<br />on <strong>%1</strong>").arg(Qt::escape(album));

    return s;
}

#include "trackinfo.h"

#include <QTextDocument>
#include <QUrl>
#include <QUrlQuery>

TrackInfo::TrackInfo()
{
}

void TrackInfo::parse(QString info)
{
    QUrl url(info);
    QUrlQuery query(url.query());

    id = query.queryItemValue("id").toInt();
    title = query.queryItemValue("title");
    artist = query.queryItemValue("artist");
    album = query.queryItemValue("album");
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
        s += QString("<strong>%1</strong>").arg(title.toHtmlEscaped());
    else
        s += "<strong>(no title)</strong>";

    if (artist != "")
        s += QString("<br />by <strong>%1</strong>").arg(artist.toHtmlEscaped());

    if (album != "")
        s += QString("<br />on <strong>%1</strong>").arg(album.toHtmlEscaped());

    return s;
}

/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Vishesh Handa <handa.vish@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "srtparser.h"

#include <QtCore/QFile>
#include <QtCore/QRegExp>

#include <kdebug.h>

SrtParser::SrtParser(const QUrl& url)
{
    QFile file( url.toLocalFile() );
    if( !file.open( QIODevice::ReadOnly ) )
        kDebug() << "The srt file - " << url.toLocalFile() << " could not be opened";

    QTextStream ts( &file );
    while( !ts.atEnd() ) {
        Subtitle sub;
        QString line = ts.readLine();
        QRegExp numberRegex("(\\d*)");
        if( line.contains( numberRegex ) )
            sub.num = numberRegex.cap( 1 ).toInt();

        line = ts.readLine();
        QRegExp timeRegex("(.*)\\s*-->\\s*(.*)");
        if( line.contains( timeRegex ) ) {
            QRegExp reg("(\\d\\d):(\\d\\d):(\\d\\d)(,(.*))?");
            QString startTime = timeRegex.cap(1).trimmed();
            QString endTime = timeRegex.cap(2).trimmed();

            //kDebug() << "Start time : " << startTime;
            if( startTime.contains( reg ) ) {
                //int h = reg.cap(1).toInt();
                //kDebug() << "Hours : " << reg.cap(1);
                sub.start = QTime( reg.cap(1).toInt(), reg.cap(2).toInt(), reg.cap(3).toInt(), reg.cap(5).toInt() );
            }
            if( endTime.contains( reg ) ) {
                sub.end = QTime( reg.cap(1).toInt(), reg.cap(2).toInt(), reg.cap(3).toInt(), reg.cap(5).toInt() );
            }
        }
        //kDebug() << sub.start << " --> " << sub.end;
        //kDebug() << sub.start.msec() << " ### " << sub.end.msec();
        while( !ts.atEnd() ) {
            line = ts.readLine();
            if( line.isEmpty() ) {
                m_subtitleList << sub;
                break;
            }
            //FIXME: An extra \n ?
            sub.text.append( line + "\n");
        }
        sub.text = sub.text.trimmed();
    }
}


QList< Subtitle > SrtParser::toList() const
{
    return m_subtitleList;
}

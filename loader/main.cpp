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

#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KCmdLineOptions>
#include <KDebug>

#include <QtCore/QFileInfo>
#include <QtCore/QFileInfoList>

#include "subtitleloader.h"
#include <QDir>
#include <KUrl>
#include <qdiriterator.h>

int main( int argc, char ** argv ) {

    KAboutData aboutData("nepomuk-subtitle-loader",
                         "nepomuk",
                         ki18n("Nepomuk Subtitle Loader"),
                         "1.0",
                         ki18n("Nepomuk Subtitles Loader"),
                         KAboutData::License_GPL,
                         ki18n("(c) 2010, Nepomuk-KDE Team"),
                         KLocalizedString(),
                         "http://nepomuk.kde.org");
    aboutData.addAuthor(ki18n("Vishesh Handa"), ki18n("Maintainer"), "handa.vish@gmail.com");

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    options.add("+videoFile", ki18n("The Video file"));
    options.add("+subtitleFile", ki18n("The subtitle file in srt format"));
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if( args->count() == 2 ) {
        QUrl videoFile( args->url(0) );
        QUrl subtitleFile = videoFile;

        //Nepomuk2::SubtitleLoader::eraseAllSubtitles();

        Nepomuk2::SubtitleLoader loader( videoFile, subtitleFile );
        loader.erase();
        loader.load();
        //loader.erase();
    }

    // ----
    return 0;
}

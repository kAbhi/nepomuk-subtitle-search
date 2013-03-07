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


#include "subtitleloader.h"
#include "srtparser.h"
#include "nsbo.h"

#include <Nepomuk2/Resource>
#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Variant>

#include <Nepomuk2/Vocabulary/NIE>

#include <KDebug>

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Nepomuk2/File>
#include <Nepomuk2/Tag>

using namespace Nepomuk2::Vocabulary;

Nepomuk2::SubtitleLoader::SubtitleLoader(const QUrl& videoUrl, const QUrl subtitleUrl)
    : m_videoUrl( videoUrl ),
      m_subtitleUrl( subtitleUrl )
{
    m_videoUrl.setScheme("file");
}

void Nepomuk2::SubtitleLoader::load()
{
    kDebug() << "Video File : " << m_videoUrl;
    kDebug() << "Subtitle File : " << m_subtitleUrl;
    SrtParser parser( m_subtitleUrl );

    Nepomuk2::Resource videoRes( m_videoUrl );
    if( !videoRes.exists() ) {
        kDebug() << "Doesn't exist";
        videoRes.addTag( Nepomuk2::Tag("Big Bang") );
    }
    kDebug() << "Is File : " << videoRes.isFile();
    kDebug() << "Resource uri: " << videoRes.uri();
    kDebug() << "File url : " << videoRes.toFile().url();

    foreach( const Subtitle & sub, parser.toList() ) {
        Nepomuk2::Resource res;
        res.addType( NSBO::Subtitle() );
        res.addProperty( NSBO::startTime(), Variant( sub.start ) );
        res.addProperty( NSBO::endTime(), Variant( sub.end ) );
        res.addProperty( NSBO::subtitleFor(), videoRes.uri() );
        kDebug() << res.property( NSBO::subtitleFor() );
        res.addProperty( NIE::plainTextContent(), Variant( sub.text ) );
        kDebug() << sub.num << " Adding - " << sub.text;

        //if( sub.text.contains("Penny") )
        //    kDebug() << sub.start;
    }
}

void Nepomuk2::SubtitleLoader::erase()
{
    //TODO: Avoid using Nepomuk2::Resource. Just query the nie:url
    Nepomuk2::Resource videoRes( m_videoUrl );
    kDebug() << videoRes.isFile();
    kDebug() << videoRes.uri();

    QString query = QString::fromLatin1("select distinct ?r where { ?r %1 %2 . }")
                    .arg( Soprano::Node::resourceToN3( NSBO::subtitleFor() ),
                          Soprano::Node::resourceToN3( videoRes.uri() ) );
    kDebug() << query;
    Soprano::Model * model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        //TODO: Use datamangement APIs
        Nepomuk2::Resource res( it["r"].uri() );
        kDebug() << "Removing " << res.uri();
        res.remove();
    }
}

// static
void Nepomuk2::SubtitleLoader::eraseAllSubtitles()
{
    QString query = QString::fromLatin1("select distinct ?r where { ?r a %1 . }")
                    .arg( Soprano::Node::resourceToN3( NSBO::Subtitle() ) );
    kDebug() << query;

    Soprano::Model * model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    while( it.next() ) {
        Nepomuk2::Resource res( it["r"].uri() );
        kDebug() << "Removing " << res.uri();
        res.remove();
    }
}

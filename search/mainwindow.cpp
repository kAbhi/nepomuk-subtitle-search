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


#include "mainwindow.h"
#include "nsbo.h"

#include <QtCore/QTimer>
#include <QtGui/QVBoxLayout>

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>

#include <Nepomuk2/ResourceManager>
#include <Nepomuk2/Vocabulary/NIE>

#include <Phonon/MediaSource>
#include <Phonon/MediaObject>

#include <KPushButton>
#include <KDebug>

using namespace Nepomuk2::Vocabulary;

MainWindow::MainWindow(QWidget* parent): QWidget(parent)
{
    m_videoPlayer = new Phonon::VideoPlayer( this );
    m_videoPlayer->setVolume( 1.0 );
    KPushButton * button = new KPushButton("Play", this);
    connect( button, SIGNAL(clicked(bool)),
             this, SLOT(start()) );

    m_lineEdit = new KLineEdit( this );
    connect( m_lineEdit, SIGNAL(returnPressed(QString)),
             this, SLOT(start()) );

    QHBoxLayout * hLayout = new QHBoxLayout();
    hLayout->addWidget( m_lineEdit );
    hLayout->addWidget( button );


    QHBoxLayout * hLay = new QHBoxLayout();

    m_fromLineEdit = new KLineEdit( this);
    m_toLineEdit = new KLineEdit( this  );
    m_minSpinBox = new QSpinBox( this );
    m_minSpinBox->setMaximum( 20 );
    m_minSpinBox->setMinimum( 1 );
    m_minSpinBox->setValue( 3 );
    m_maxSpinBox = new QSpinBox( this );
    m_maxSpinBox->setMaximum( 20 );
    m_maxSpinBox->setMinimum( 1 );
    m_maxSpinBox->setValue( 12 );
    m_findButton = new KPushButton("Find",  this);

    connect( m_findButton, SIGNAL(clicked(bool)), this, SLOT(findSubtitle()) );

    hLay->addWidget( m_fromLineEdit );
    hLay->addWidget( m_toLineEdit );
    hLay->addWidget( m_minSpinBox );
    hLay->addWidget( m_maxSpinBox );
    hLay->addWidget( m_findButton );

    m_seekSlider = new Phonon::SeekSlider( this );
    m_seekSlider->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    //m_seekSlider->setGeometry( m_seekSlider->pos().x(), m_seekSlider->pos().y(), m_seekSlider->rect().width(), 50 );

    QVBoxLayout * layout = new QVBoxLayout( this );
    layout->addWidget( m_videoPlayer, 6 );
    layout->addWidget( m_seekSlider );
    layout->addLayout( hLay );
    layout->addLayout( hLayout );

    //layout->addLayout( other, 1 );

    setLayout( layout );
}


QList< Result > MainWindow::generateSingleSubtitleResults(const QString& expression)
{
    if( expression.isEmpty() )
        return QList<Result>();

    QString filterExpression = createFilter( expression, "text" );

    QString query = QString::fromLatin1("select distinct ?url ?st ?end ?text where {"
                                        "?r %1 ?url . ?sub %2 ?r ."
                                        "?sub %3 ?st . ?sub %4 ?end . "
                                        "?sub %5 ?text . "
                                        "%6 } order by ?url ?st ")
                    .arg( Soprano::Node::resourceToN3( NIE::url() ),
                          Soprano::Node::resourceToN3( NSBO::subtitleFor() ),
                          Soprano::Node::resourceToN3( NSBO::startTime() ),
                          Soprano::Node::resourceToN3( NSBO::endTime() ),
                          Soprano::Node::resourceToN3( NIE::plainTextContent() ),
                          filterExpression );

    kDebug() << query;

    Soprano::Model * model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    QList<Result> results;
    while( it.next() ) {
        Result r;
        r.m_filePath = it["url"].uri();
        r.startTime = it["st"].literal().toTime();
        r.endTime = it["end"].literal().toTime();
        r.text = it["text"].literal().toString();
        kDebug() << r.text;
        results << r;
    }

    return results;
}


void MainWindow::play(const Result & res)
{
    QString playingFile = m_videoPlayer->mediaObject()->currentSource().fileName();
    if( playingFile != res.m_filePath.toLocalFile() ) {
        kDebug() << "Loading File : " << playingFile;
        Phonon::MediaSource source( res.m_filePath );
        m_videoPlayer->load( source );
        setWindowTitle( res.m_filePath.toLocalFile() );
    }
    m_seekSlider->setMediaObject( m_videoPlayer->mediaObject() );
    m_videoPlayer->play();

    int startMecs = QTime().msecsTo( res.startTime );//jumpToWord( res, m_lineEdit->text() );
    //kDebug() << "Normal start : " << QTime().msecsTo( res.startTime );
    //kDebug() << "Interpolated start : " << startMecs;
    m_videoPlayer->seek( startMecs );

    int duration = QTime().msecsTo( res.endTime ) - startMecs;
    QTimer::singleShot( duration, this, SLOT( playedVideo() ) );
}


void MainWindow::start()
{
    //Result res;
    //res.m_filePath = QUrl("file:///home/vishesh/The Big Bang Theory/Season 3/22_The Staircase Implementation.avi");

    m_results.clear();
    m_videoPlayer->stop();
    m_results = generateSingleSubtitleResults( m_lineEdit->text() );
    m_pos = 0;

    kDebug() << "Found " << m_results.size() << " results";
    if( !m_results.empty() ) {
        play( m_results.first() );
    }
}

void MainWindow::playedVideo()
{
    kDebug() << "done playing the video";
    m_pos++;
    if( m_pos == m_results.size() ) {
        m_videoPlayer->pause();
    }
    else if( m_pos < m_results.size() ) {
        play( m_results[ m_pos ] );
    }
}

int MainWindow::jumpToWord(const Result& res, const QString& word)
{
    int pos = res.text.indexOf( word );
    int msecs = QTime().msecsTo( res.startTime );
    float duration = res.startTime.msecsTo( res.endTime );
    kDebug() << "Increasing by : " << ( (duration/res.text.length()) * pos );
    return msecs + ( (duration/res.text.length()) * pos );
}

void MainWindow::findSubtitle()
{
   // Create a list of results which contain both start and end
   m_videoPlayer->stop();
   m_results = generateMultipleSubtitleResults();
   m_pos = 0;

   if( !m_results.empty() ) {
       play( m_results.first() );
   }
}

QList< Result > MainWindow::generateMultipleSubtitleResults()
{
    QString fromWords = m_fromLineEdit->text();
    QString toWords = m_toLineEdit->text();

    QString query = QString::fromLatin1("select distinct ?url ?st1 ?st2 ?en1 ?en2 where  {"
                                        "?r nie:url ?url . ?sub1 nsbo:subtitleFor ?r . ?sub2 nsbo:subtitleFor ?r ."
                                        "?sub1 nie:plainTextContent ?text1 . ?sub2 nie:plainTextContent ?text2 . "
                                        "?sub1 nsbo:startTime ?st1 . ?sub1 nsbo:endTime ?en1 . "
                                        "?sub2 nsbo:startTime ?st2 . ?sub2 nsbo:endTime ?en2. "
                                        "%1 %2 } ")
                    .arg( createFilter( fromWords, "text1" ),
                          createFilter( toWords, "text2" ) );

    kDebug() << query;

    // Need to implement the second time restriction
    Soprano::Model * model = Nepomuk2::ResourceManager::instance()->mainModel();
    Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
    QList<Result> results;
    while( it.next() ) {
        Result r;
        r.m_filePath = it["url"].uri().toLocalFile();

        QTime st1 = it["st1"].literal().toTime();
        QTime st2 = it["st2"].literal().toTime();
        QTime en1 = it["en1"].literal().toTime();
        QTime en2 = it["en2"].literal().toTime();

        int duration = en1.msecsTo(st2) / 1000;
        int minTime = m_minSpinBox->value();
        int maxTime = m_maxSpinBox->value();
        //kDebug() << "Duration : " << duration;
        if( minTime <= duration && duration <= maxTime ) {
            r.startTime = st1;
            r.endTime = en2;
            r.text = it["text1"].literal().toString() + it["text2"].literal().toString();

            kDebug() << "Pushing with duration " << duration;
            results << r;
        }
    }

    return results;
}

QString MainWindow::createFilter(const QString& expression, const QString& var)
{
    QString filterExpression;
    QSet<QString> list = expression.split( QChar(' ') ).toSet();
    foreach( const QString & word, list ) {
        filterExpression += QString::fromLatin1("FILTER( regex(str(?%1), '%2', 'i') ) . ")
        .arg( var, word );
    }
    return filterExpression;
}

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
 
 
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <QtGui/QWidget>
 
#include <QtCore/QUrl>
#include <QtCore/QTime>
#include <QtCore/QString>
 
#include <Phonon/VideoPlayer>
#include <Phonon/SeekSlider>
 
#include <KLineEdit>
#include <QSpinBox>
#include <KPushButton>
 
class Result {
public:
    QUrl m_filePath;
    QTime startTime;
    QTime endTime;
    QString text;
};
 
class MainWindow : public QWidget
{
    Q_OBJECT
public:
    MainWindow( QWidget * parent=NULL );
    
private slots:
    void start();
    void playedVideo();
    void findSubtitle();
    
private:
    Phonon::VideoPlayer * m_videoPlayer;
    Phonon::SeekSlider * m_seekSlider;
    KLineEdit * m_lineEdit;
    QList<Result> m_results;
    int m_pos;
    
    QList<Result> generateSingleSubtitleResults( const QString & expression );
    QList<Result> generateMultipleSubtitleResults();
    
    void play( const Result& res );
    
    int jumpToWord( const Result & res, const QString & word );
    
    
    KLineEdit * m_fromLineEdit;
    KLineEdit * m_toLineEdit;
    QSpinBox * m_minSpinBox;
    QSpinBox * m_maxSpinBox;
    KPushButton * m_findButton;
    
    
    QString createFilter( const QString & expression, const QString & var );
};
 
#endif // MAINWINDOW_H

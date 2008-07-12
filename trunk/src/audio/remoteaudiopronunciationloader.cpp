/***************************************************************************
 *   Copyright (C) 2008 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/

#include "remoteaudiopronunciationloader.h"
#include <QUrl>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QDir>


RemoteAudioPronunciationLoader::RemoteAudioPronunciationLoader(SoundSource *soundSource, QObject *parent)
    :AudioPronunciationLoader (parent), m_networkManager(0)
{
    m_soundSource = soundSource;
    m_audioCache.setMaxCost(20);  
    connect(m_soundSource, SIGNAL(soundUrlFound(const QString&,const QUrl&)), 
            this, SLOT(slotSoundUrlFound(const QString&,const QUrl&)));
    
    QDir dir = QDir::tempPath();
    if (!dir.exists("synonym")) {
        if (dir.mkdir("synonym")) {
            dir.cd("synonym");
            dirPath = dir.canonicalPath();
        }
    } else {
        dir.cd("synonym");
        dirPath = dir.canonicalPath();
    }
    
}

RemoteAudioPronunciationLoader::~RemoteAudioPronunciationLoader()
{
    delete m_networkManager;
}


class TempFile : public QFile
{
public:    
    TempFile(const QString &filename) : QFile(filename) {}
    ~TempFile() { remove(); }
};



void RemoteAudioPronunciationLoader::doGetAudio(const QString &word)
{
    if (dirPath.isEmpty())
        return;
    qDebug() << "remote doGetAudio " << word;
    if (m_audioCache.contains(word)) {
        QFile *file = m_audioCache[word];
        emit soundFound(word, Phonon::MediaSource(file->fileName()));
        return;
    }
    m_Word = word;
    m_soundSource->findSoundUrl(word);
}



void RemoteAudioPronunciationLoader::slotSoundLoadingFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError && reply->objectName() == m_Word) {
        QStringList items = reply->url().toString().split(".");
        QString extension;
        if (items.size() > 0) 
            extension = items.last();
        QFile *file = new TempFile(dirPath + "/" + reply->objectName() + "." + extension);
        file->open(QIODevice::WriteOnly);
        file->write(reply->readAll());
        file->close();
        m_audioCache.insert(m_Word, file);
        Phonon::MediaSource source(file->fileName());
        source.setAutoDelete(false);
        soundFound(m_Word, source);
    }
    reply->deleteLater();
}





/*
void RemoteAudioPronunciationLoader::slotSoundLoadingFinished(QNetworkReply *reply)
{
    qDebug() << "slotSoundLoadingFinished " << reply->objectName();
    if (reply->error() == QNetworkReply::NoError && reply->objectName() == m_Word) {
        QBuffer *buffer = new QBuffer();
        buffer->open(QIODevice::ReadWrite);
        buffer->write(reply->readAll());
        buffer->reset();
        m_audioCache.insert(reply->objectName(), buffer);
        Phonon::MediaSource source(buffer);
        soundFound(reply->objectName(), source);
    }
    reply->deleteLater();
}
*/



void RemoteAudioPronunciationLoader::slotSoundUrlFound(const QString &word, const QUrl &url)
{
    if (word == m_Word)
        downloadSound(url); 
}

void RemoteAudioPronunciationLoader::downloadSound(const QUrl &url)
{
    QNetworkRequest request(url);
    if (!m_networkManager) {
        m_networkManager = new QNetworkAccessManager();
        connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slotSoundLoadingFinished(QNetworkReply*)));
    }
    
    QNetworkReply *reply = m_networkManager->get(request);
    reply->setObjectName(m_Word);
}










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
#include  <QNetworkReply>
#include <QBuffer>
#include <QDir>


RemoteAudioPronunciationLoader::RemoteAudioPronunciationLoader(SoundSource *soundSource, QObject *parent)
    :AudioPronunciationLoader (parent), m_soundData(0), m_reply(0)
{
    m_soundSource = soundSource;
    m_networkManager = new QNetworkAccessManager(this);
    
    connect(m_soundSource, SIGNAL(soundUrlFound(const QString&,const QUrl&)), 
            this, SLOT(slotSoundUrlFound(const QString&,const QUrl&)));
}

RemoteAudioPronunciationLoader::~RemoteAudioPronunciationLoader()
{
    delete m_soundData;
}



void RemoteAudioPronunciationLoader::doGetAudio(const QString &word)
{
    qDebug() << "remote doGetAudio";
    m_Word = word;
    delete m_soundData;
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
    }
    m_soundData = new QByteArray();
    m_soundSource->findSoundUrl(word);
}

void RemoteAudioPronunciationLoader::slotReadyRead()
{
    m_soundData->append(m_reply->readAll());
}

void RemoteAudioPronunciationLoader::slotSoundLoadingFinished()
{
    m_soundData->append(m_reply->readAll());
    
    QBuffer *buffer = new QBuffer(m_soundData);
    qDebug() << "Sound buffer size " << buffer->size();
    Phonon::MediaSource source(buffer);
    source.setAutoDelete(true);
    soundFound(m_Word, source);
    m_reply->deleteLater();
    m_reply = 0;
}

void RemoteAudioPronunciationLoader::slotSoundUrlFound(const QString &word, const QUrl &url)
{
    if (word == m_Word)
        downloadSound(url); 
}

void RemoteAudioPronunciationLoader::downloadSound(const QUrl &url)
{
    QNetworkRequest request(url);
    m_reply = m_networkManager->get(request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));    
    connect(m_reply, SIGNAL(finished()), this, SLOT(slotSoundLoadingFinished()));
}










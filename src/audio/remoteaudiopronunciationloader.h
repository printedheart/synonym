/***************************************************************************
 *   Copyright (C) 2007 by Sergejs   *
 *   sergey.melderis@gmail.com   *
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
 ***************************************************************************/

#ifndef REMOTEAUDIOPRONUNCIATIONLOADER_H
#define REMOTEAUDIOPRONUNCIATIONLOADER_H


#include "audiopronunciationloader.h"
#include "soundsource.h"

#include <QString>

#include <phonon/mediasource.h>
#include <QByteArray>

class QNetworkAccessManager;
class QNetworkReply;

class RemoteAudioPronunciationLoader : public AudioPronunciationLoader 
{
Q_OBJECT    
public:
    RemoteAudioPronunciationLoader(SoundSource *soundSource, QObject *parent = 0);
    virtual ~RemoteAudioPronunciationLoader();
    
protected:    
    virtual void doGetAudio(const QString &word);
    
private slots:    
    void slotReadyRead();
    void slotSoundLoadingFinished();
    
    void slotSoundUrlFound(const QString &word, const QUrl &url);
    
private:    
    SoundSource *m_soundSource;
    
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_reply;
    QByteArray *m_soundData;
    
    QString m_Word;
    
    void downloadSound(const QUrl &url);
};


#endif

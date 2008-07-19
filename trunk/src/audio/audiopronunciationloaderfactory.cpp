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
 


#include "audiopronunciationloaderfactory.h"
#include "localaudiopronunciationloader.h"
#include "remoteaudiopronunciationloader.h"
#include "scriptablesoundsource.h"


#include <QSettings>
#include <QVariant>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>


AudioPronunciationLoaderFactory*  AudioPronunciationLoaderFactory::m_instance = 0;

AudioPronunciationLoaderFactory *AudioPronunciationLoaderFactory::instance() 
{
    QMutex mutex;
    QMutexLocker locker(&mutex);
    if (m_instance == 0) {
        m_instance = new AudioPronunciationLoaderFactory();
    }
    return m_instance;
}


AudioPronunciationLoaderFactory::AudioPronunciationLoaderFactory()
    : m_remoteLoader(0), m_soundSource(0)
{
}


AudioPronunciationLoaderFactory::~AudioPronunciationLoaderFactory()
{
}


AudioPronunciationLoader *AudioPronunciationLoaderFactory::createAudioLoader() 
{
    QSettings settings("http://code.google.com/p/synonym/", "synonym");
    if (settings.childGroups().contains("audio")) {
        settings.beginGroup("audio");
        QString loaderType = settings.value("LoaderType").toString();
        if (loaderType == "local") {
            QVariant soundDirectory = settings.value("SoundDirectory");
            return new LocalAudioPronunciationLoader(0, soundDirectory.toString());
        } else {
            if (!m_remoteLoader) {
                if (!m_soundSource) {
                    m_soundSource = new ScriptableSoundSource(0);
                    m_remoteLoader = new RemoteAudioPronunciationLoader(m_soundSource, 0);
                    m_soundSource->setParent(m_remoteLoader);
                }
            }
        }
    } else {
        m_soundSource = new ScriptableSoundSource(0);
        m_remoteLoader = new RemoteAudioPronunciationLoader(m_soundSource, 0);
    }
    return m_remoteLoader;
}

bool AudioPronunciationLoaderFactory::configureScript()
{
    QSettings settings("http://code.google.com/p/synonym/", "synonym");
    settings.beginGroup("audio");
    QVariant script = settings.value("CurrentScript");
    if (script.isNull())
        return false;
    settings.endGroup();
    QFile file(QDir::homePath() + "/.synonym/" + script.toString());
    QFileInfo fileInfo(file);
    if (fileInfo.exists() && fileInfo.isFile() && fileInfo.isReadable()) {
        m_soundSource->setScriptSource(file.fileName());
        return true;
    }        
    return false;
}


void AudioPronunciationLoaderFactory::reconfigure()
{
    if (m_soundSource)
        configureScript();
}


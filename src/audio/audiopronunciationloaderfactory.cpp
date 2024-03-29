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
#include "wordnikloader.h"
#include "merriamwebsterloader.h"


#include <QSettings>
#include <QVariant>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>
#include <QCoreApplication>


AudioPronunciationLoaderFactory*  AudioPronunciationLoaderFactory::m_instance = 0;

AudioPronunciationLoaderFactory *AudioPronunciationLoaderFactory::instance() 
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (m_instance == 0) {
        m_instance = new AudioPronunciationLoaderFactory();
    }
    return m_instance;
}


AudioPronunciationLoaderFactory::AudioPronunciationLoaderFactory()

{
}


AudioPronunciationLoaderFactory::~AudioPronunciationLoaderFactory()
{
}


AudioPronunciationLoader *AudioPronunciationLoaderFactory::createAudioLoader() 
{
    QStringList args = QCoreApplication::arguments();
    if (args.contains("-webster")) {
        qDebug() << "Using MerriamWebster sound loader";
        return new MerriamWebsterLoader(0);
    }
    return new WordnikLoader(0);
}






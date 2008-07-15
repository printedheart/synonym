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
#include "localaudiopronunciationloader.h"

#include <QDir>
#include <QFile>
#include <QStringList>

#include <QRunnable>
#include <QThreadPool>

LocalAudioPronunciationLoader::LocalAudioPronunciationLoader(QObject *parent, const QString &directoryName)
        : AudioPronunciationLoader(parent), m_directoryName(directoryName)
{
    loadAvailableWords();
}


LocalAudioPronunciationLoader::~LocalAudioPronunciationLoader()
{
}


void LocalAudioPronunciationLoader::loadAvailableWords()
{
    class Loader : public QRunnable 
    {
        public:
            Loader(QDir dir, QHash<QString, QString> *hash) : m_dir(dir), m_hash(hash) {}
            virtual ~Loader() {}
            void run() {
                QStringList filters;
                filters << "*.wav" << "*.ogg" << "*.mp3";
                QStringList entries = m_dir.entryList(filters, QDir::Files);
                foreach (QString entry, entries) {
                    QString extension = entry.right(4);
                    entry.remove(entry.lastIndexOf("."), 4);
                    m_hash->insert(entry, extension);
                }    
            }
        private:         
            QDir m_dir;
            QHash<QString, QString> *m_hash;
    };
    
    QDir dir(m_directoryName);
    if (dir.exists()) {
        Loader *loader = new Loader(dir, &m_availableWords);
        loader->setAutoDelete(true);
        QThreadPool::globalInstance()->start(loader, QThread::LowestPriority);
    }
}


void LocalAudioPronunciationLoader::doGetAudio(const QString &word)
{
    qDebug() << "local doGetAudio";
    if (m_availableWords.contains(word)) {
        QFile soundFile(m_directoryName + "/" + word + m_availableWords[word]);
        if (soundFile.exists()) {
            Phonon::MediaSource source(soundFile.fileName());
            soundFound(word, source);
        }
    }
}

bool LocalAudioPronunciationLoader::isAvailable() const
{
    return m_availableWords.size() > 0;
}















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

#include "wordsoundholder.h"

WordSoundHolder::WordSoundHolder(QObject *parent)
    : QObject(parent)
{
}

WordSoundHolder::~WordSoundHolder()
{
    foreach (QFile *file, m_soundCache) {
        file->remove();    
    }
}


QString WordSoundHolder::getSoundFilePath(const QString &word) const
{
    if (m_soundCache.contains(word)) {
        return m_soundCache.value(word)->fileName();
    }

    return QString();

}

void WordSoundHolder::setSoundReady(const QString &word)
{
    if (m_soundCache.contains(word)) {
        QFile *file = m_soundCache.value(word);
        file->close();
        emit soundReady(word);
    }
}

void WordSoundHolder::setSoundError(const QString &word)
{
    if (m_soundCache.contains(word)) {
        QFile *file = m_soundCache.take(word);
        file->close();
        file->remove();
    }
}


QIODevice *WordSoundHolder::getIODeviceForSound(const QString &word, const QString &soundName)
{
    QString tempDirPath = QDir::tempPath();
    QFile *soundFile = new QFile(tempDirPath + "/" + soundName);
    m_soundCache[word] = soundFile;
    if (!soundFile->open(QIODevice::WriteOnly)) {
        qDebug() << "Cannot open file for writing";
        QCoreApplication::quit();
    }
    return soundFile;
}



/***************************************************************************
 *   Copyright (C) 2007 by Sergejs Melderis                                *
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
 ***************************************************************************/

#include "pronunciationsoundholder.h"
#include "pronunciationsoundfactory.h"
#include "pronunciationsoundfactoryimpl.h"
#include <QtCore>


PronunciationSoundHolder::PronunciationSoundHolder(QObject *parent)
    :QObject(parent)
{
    m_soundFactory = new PronunciationSoundFactoryImpl(this);
    m_soundFactory->setSoundReceiver(this);
}

PronunciationSoundHolder::~PronunciationSoundHolder()
{

}


void PronunciationSoundHolder::putPronunciationSound(const QString &word, QIODevice *sound)
{
    m_soundCache[word] = sound;
    emit soundReady(word);    
}

QIODevice* PronunciationSoundHolder::pronunciationSound(const QString &word)
{
    if (m_soundCache.contains(word)) {
        return m_soundCache.value(word);    
    }
    m_soundFactory->findSound(word);
    return 0;
}

void PronunciationSoundHolder::findPronunciation(const QString &word)
{
    m_soundFactory->findSound(word);
}






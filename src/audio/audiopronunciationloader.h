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
#ifndef AUDIOPRONUNCIATIONLOADER_H
#define AUDIOPRONUNCIATIONLOADER_H

#include <QObject>
#include <QString>
#include <phonon/mediasource.h>

/**
    @author Sergey Melderis <sergey.melderis@gmail.com>
*/
class AudioPronunciationLoader : public QObject
{
Q_OBJECT
public:
    AudioPronunciationLoader(QObject *parent);

    ~AudioPronunciationLoader();
    
    void loadAudio(const QString &word);
    
signals:    
    void soundLoaded(const Phonon::MediaSource &soundSource);
    void soundLoaded(const QString &fileName);    
    
    
protected:    
    void soundFound(const QString &word, const Phonon::MediaSource &source);
    
    virtual void doGetAudio(const QString &word) = 0;
    
    QString currentWord() const;
    
private:    
    QString m_currentWord;
};

#endif

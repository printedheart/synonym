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

#ifndef WORDSOUND_HOLDER_H
#define WORDSOUND_HOLDER_H

#include <QtCore>

class WordSoundHolder : public QObject
{
Q_OBJECT
public: 
    WordSoundHolder(QObject *parent);
    virtual ~WordSoundHolder();

    QString getSoundFilePath(const QString &word) const;

    QIODevice* getIODeviceForSound(const QString &word, const QString &soundName);

    void setSoundReady(const QString &word);

    void setSoundError(const QString &word);
signals:
    void soundReady(const QString &word);


private:
    QHash<QString,QFile*> m_soundCache;

};


#endif

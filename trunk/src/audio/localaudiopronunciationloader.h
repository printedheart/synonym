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
 **************************************************************************/
#ifndef LOCALAUDIOPRONUNCIATIONLOADER_H
#define LOCALAUDIOPRONUNCIATIONLOADER_H

#include "audiopronunciationloader.h"

#include <QHash>

/**
    @author Sergey Melderis <sergey.melderis@gmail.com>
*/
class LocalAudioPronunciationLoader : public AudioPronunciationLoader
{    
Q_OBJECT    
public:
    LocalAudioPronunciationLoader(QObject *parent, const QString &directoryName);

    virtual ~LocalAudioPronunciationLoader();
    
protected:    
    virtual void doGetAudio(const QString &word);
    
    
private:    
    QHash<QString, QString> m_availableWords;
    
    QString m_directoryName;
    
    void loadAvailableWords();

};

#endif


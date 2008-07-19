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
#ifndef AUDIOPRONUNCIATIONLOADERFACTORY_H
#define AUDIOPRONUNCIATIONLOADERFACTORY_H

#include "remoteaudiopronunciationloader.h" 
#include "scriptablesoundsource.h"

class AudioPronunciationLoader;

/**
	@author Sergey Melderis <sergey.melderis@gmail.com>
*/
class AudioPronunciationLoaderFactory : public QObject
{
public:
    static AudioPronunciationLoaderFactory* instance();


    ~AudioPronunciationLoaderFactory();
    
    
    AudioPronunciationLoader *createAudioLoader();
    
    void reconfigure();
    
private:    
    AudioPronunciationLoaderFactory();
    static AudioPronunciationLoaderFactory *m_instance;
    
    RemoteAudioPronunciationLoader *m_remoteLoader;
    ScriptableSoundSource *m_soundSource;
    bool configureScript();
};

#endif
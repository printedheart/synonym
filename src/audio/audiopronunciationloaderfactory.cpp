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



AudioPronunciationLoaderFactory::AudioPronunciationLoaderFactory(QObject *parent)
 : QObject(parent)
{
}


AudioPronunciationLoaderFactory::~AudioPronunciationLoaderFactory()
{
}


AudioPronunciationLoader * AudioPronunciationLoaderFactory::createAudioLoader() 
{
    QSettings settings("http://code.google.com/p/synonym/", "synonym");
    if (settings.childGroups().contains("audio")) {
        settings.beginGroup("audio");
        QVariant loaderType = settings.value("LoaderType", "local");
        if (loaderType == "local") {
            QVariant soundDirectory = settings.value("SoundDirectory");
            return new LocalAudioPronunciationLoader(0, soundDirectory.toString());
        } if (loaderType == "remote") {
            QVariant scriptName = settings.value("CurrentScript");
            QVariant scriptFileName = settings.value(scriptName.toString() + "/Filename");
            QFile file(scriptFileName.toString());
            if (file.exists()) {
                ScriptableSoundSource *source = new ScriptableSoundSource(0);
                source->setScriptSource(file.fileName());
                AudioPronunciationLoader *loader = new RemoteAudioPronunciationLoader(source, 0);
                source->setParent(loader);
                return loader;
            }
        }
    }
    return 0;
}




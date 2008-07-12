/***************************************************************************
 *   Copyright (C) 2008 by Sergejs                                         *
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
 *                                                                         *
 ***************************************************************************/
 
#ifndef SCRIPTABLESOUNDSOURCE_H
#define SCRIPTABLESOUNDSOURCE_H
 
 
#include "soundsource.h"
 
#include <QByteArray>
#include <QtScript>

class QNetworkAccessManager;
class QNetworkReply; 


class ScriptableSoundSource : public SoundSource
{
Q_OBJECT
        
public:        
    ScriptableSoundSource(QObject *parent);
    ~ScriptableSoundSource();
    
    
    /**
     * Function is used to request to find an url of the pronunciation
     * sound for the word. Function delegates execution to configured script.
     * 
     */
    virtual void findSoundUrl(const QString &word);
    
    
    void setScript(const QString &script);
    
    
    void setScriptSource(const QString &fileName);
    
    
signals:
    void findSoundUrlLater(const QString &word);    
    
public slots:
        
    /**
     * Public slot for scripts. Initializes downloading of the resource 
     * specified by url.
     */
    void download(const QString &url, const QString &callback);
    
    
    /**
     * Public slot for scripts. Scripts call this slot to indicate
     * that url of the sound is found.
     */
    void urlFound(const QString &url);
    
    
signals:
    
    /**
     * Signal indicates that some lines available after last download 
     * was called.
     */
    void linesAvailable(const QStringList &lines);   
    
        
private slots:    
    void slotReadyRead();
    void stopNetwork();
    
    void _findSoundUrl(const QString &word);
private:    
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_reply;
    QByteArray m_data;
    
    
    QScriptEngine *m_scriptEngine;
    QScriptValue m_script;
    
    QString m_currentWord;
    
    void invokeCallback(QString &callback, QStringList &lines);
    

};

#endif


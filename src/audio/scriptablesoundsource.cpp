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
 
 
#include "scriptablesoundsource.h"
#include <QUrl>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include  <QNetworkReply>
#include <QApplication>


ScriptableSoundSource::ScriptableSoundSource(QObject *parent)
    :SoundSource(parent), m_networkManager(0), m_reply(0)
{
    
    m_scriptEngine = new QScriptEngine(this);
    QScriptValue thisValue = m_scriptEngine->newQObject(this);
    m_scriptEngine->globalObject().setProperty("downloader", thisValue);
    connect(this, SIGNAL(findSoundUrlLater(const QString&)), this, SLOT(_findSoundUrl(const QString&)));
}

ScriptableSoundSource::~ScriptableSoundSource()
{
    delete m_networkManager;
}

    
void ScriptableSoundSource::setScriptSource(const QString &fileName)
{
    qDebug() << "Initializing ScriptableSoundSource from script " << fileName;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file " << fileName;
        return;
    }

    QByteArray data;
    while (!file.atEnd()) {
        data.append(file.readLine());
    }
    file.close();
    QString script(data);
    setScript(script);
}

void ScriptableSoundSource::setScript(const QString &script)
{
    
    m_script = m_scriptEngine->evaluate(script);
    QScriptValue global = m_scriptEngine->globalObject();
    QScriptValue init = global.property("init");
    init.call(QScriptValue());
    if (m_scriptEngine->hasUncaughtException()) {
        m_script = QScriptValue();
    }
}


void ScriptableSoundSource::_findSoundUrl(const QString &word)
{
    if (word != m_currentWord)
        return;
    findSoundUrl(word);
}

void ScriptableSoundSource::findSoundUrl(const QString &word)
{
    qDebug() << "findSoundUrl() start";
    if (!m_script.isValid())
        return;
    m_currentWord = word;
    if (!m_networkManager)
        m_networkManager = new QNetworkAccessManager();
    if (m_reply) {
        stopNetwork();
        emit findSoundUrlLater(word);
        return;
    }
 
    QScriptValue global = m_scriptEngine->globalObject();
    QScriptValue findSoundUrl = global.property("findSoundUrl");
    QScriptValueList args;
    args << QScriptValue(m_scriptEngine, word);
    findSoundUrl.call(QScriptValue(), args);                 
}


void ScriptableSoundSource::download(const QString &urlString, const QString &callback)
{
   // qDebug() << "download(" << urlString << ", " << callback << ")";
    stopNetwork();
    QUrl url(urlString);
    const QNetworkRequest request(url);
    m_reply = m_networkManager->get(request);
    m_reply->setObjectName(callback);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead())); 
    connect(m_reply, SIGNAL(finished()), this, SLOT(stopNetwork()));   
}

void ScriptableSoundSource::urlFound(const QString &urlString)
{
    m_reply->abort();
    QUrl url(urlString);
    if (url.isValid())
        emit soundUrlFound(m_currentWord, QUrl(url));
    
}


void ScriptableSoundSource::invokeCallback(QString &callback, QStringList &lines)
{
    QScriptValue global = m_scriptEngine->globalObject();
    QScriptValue callbackFunc = global.property(callback);
    QScriptValue array = m_scriptEngine->newArray(lines.size());
    for (int i = 0; i < lines.size(); i++)
        array.setProperty(i, QScriptValue(m_scriptEngine, lines[i]));
    QScriptValueList args;
    args << array;
    callbackFunc.call(QScriptValue(), args);                 
}


void ScriptableSoundSource::slotReadyRead()
{
    //qDebug() << "slotReadyRead() start";
    if (!m_reply)
        return;
    
    QStringList lines;
    while (true) {
        QByteArray data = m_reply->readLine();
        if (data.isEmpty()) {
            break;
        }
        QString line(data);
        lines << line;
    }
    QString callback = m_reply->objectName();
    invokeCallback(callback, lines);
    //qDebug() << "slotReadyRead() end";
}

void ScriptableSoundSource::stopNetwork()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = 0;
    }
}













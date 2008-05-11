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



ScriptableSoundSource::ScriptableSoundSource(QObject *parent)
    :SoundSource(parent), m_reply(0)
{
    m_networkManager = new QNetworkAccessManager(this);
    QScriptValue thisValue = m_scriptEngine.newQObject(this);
    m_scriptEngine.globalObject().setProperty("downloader", thisValue);
}

ScriptableSoundSource::~ScriptableSoundSource()
{}

    
void ScriptableSoundSource::setScriptSource(const QString &fileName)
{
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
    m_script = m_scriptEngine.evaluate(script);
    QScriptValue global = m_scriptEngine.globalObject();
    QScriptValue init = global.property("init");
    if (init.isFunction()) qDebug() << "init is function";
    init.call(QScriptValue());
    if (m_scriptEngine.hasUncaughtException()) {
        m_script = QScriptValue();
    }
}


void ScriptableSoundSource::findSoundUrl(const QString &word)
{
    if (!m_script.isValid())
        return;
    
    stopNetwork();
    m_currentWord = word;
    QScriptValue global = m_scriptEngine.globalObject();
    QScriptValue findSoundUrl = global.property("findSoundUrl");
    
    if (findSoundUrl.isFunction()) {
        qDebug() << "findSoundUrl is a function";
    } else {
        qDebug() << "findSoundUrl us not a function";
    }
    QScriptValueList args;
    args << QScriptValue(&m_scriptEngine, word);
    findSoundUrl.call(QScriptValue(), args);                 
}


void ScriptableSoundSource::download(const QString &urlString)
{
    qDebug() << "download(" << urlString << ")";
    stopNetwork();
    QUrl url(urlString);
    const QNetworkRequest request(url);
    m_networkManager->moveToThread(QThread::currentThread());
    m_reply = m_networkManager->get(request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));    
}

void ScriptableSoundSource::urlFound(const QString &urlString)
{
    stopNetwork();
    QUrl url(urlString);
    if (url.isValid())
        emit soundUrlFound(m_currentWord, QUrl(url));
}


void ScriptableSoundSource::slotReadyRead()
{
    QStringList lines;
    while (true) {
        QByteArray data = m_reply->readLine();
        if (data.isEmpty()) {
            break;
        }
        QString line(data);
        lines << line;
    }
    emit linesAvailable(lines);
}

void ScriptableSoundSource::stopNetwork()
{
    if (m_reply) {
        m_reply->abort();
        m_reply->deleteLater();
        m_reply = 0;
    }
}













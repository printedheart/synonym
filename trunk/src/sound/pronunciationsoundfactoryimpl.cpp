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
#include "pronunciationsoundfactoryimpl.h"


#include <QtNetwork>
#include <QtDebug>


PronunciationSoundFactoryImpl::PronunciationSoundFactoryImpl(QObject *parent)
 :  QObject(parent)
{

}


PronunciationSoundFactoryImpl::~PronunciationSoundFactoryImpl()
{
    
}

void PronunciationSoundFactoryImpl::setSoundReceiver(PronunciationSoundHolder *soundHolder)
{
    m_soundHolder = soundHolder;
    m_httpHtml = new QHttp(this);
    m_httpSound = new QHttp(this);

    connect (m_httpHtml, SIGNAL(responseHeaderReceived( const QHttpResponseHeader& )),
             this, SLOT(htmlResponseHeaderReceived(const QHttpResponseHeader&)));
    connect (m_httpSound, SIGNAL(requestFinished(int, bool)),
             this, SLOT(httpSoundRequestFinished(int, bool)));
}


void PronunciationSoundFactoryImpl::findSound(const QString &word)
{
    m_curWord = word;
    QString urlString("http://www.yourdictionary.com/ahd/search?p=" + word + "&searchmode=normal");
    QUrl url(urlString);
    m_httpHtml->setHost(url.host());
    m_httpHtmlGetId = m_httpHtml->get(urlString, &m_htmlBuffer);

}


void PronunciationSoundFactoryImpl::httpHtmlRequestFinished(int requestId, bool error)
{
    if (requestId != m_httpHtmlGetId)
        return;

    if (error) {
        qDebug() << m_httpHtml->errorString();
    }

    QByteArray html = m_htmlBuffer.data();
  //  qDebug() << html;
    int linkIndex = html.indexOf("<a href=\"/ahd/pron/");
    QString link;
    if (linkIndex != -1) {
        int startLink = linkIndex + 9;
        int indexOfTagClose = html.indexOf(">", linkIndex);
        int endLink = indexOfTagClose - 2;
        for (int i = startLink; i <= endLink; i++) {
            link.append(QChar(html[i]));
        }
        qDebug() << "Link: " << link;
        QUrl url("http://www.yourdictionary.com" + link);
        QString soundName = link.section('/', -1);

        soundBuffer = new QBuffer(m_soundHolder);
        m_httpSound->setHost(url.host());
        m_httpSoundGetId = m_httpSound->get(url.path(), soundBuffer);
    } else {
        qDebug() << "Link index not found";
    }

    disconnect (m_httpHtml, SIGNAL(requestFinished(int, bool)),
                this, SLOT(httpHtmlRequestFinished(int, bool)));
    m_htmlBuffer.close();
}


void PronunciationSoundFactoryImpl::httpSoundRequestFinished(int requestId, bool error)
{
    if (requestId != m_httpSoundGetId)
        return;

    soundBuffer->close();
    if (error) {
        qDebug() << m_httpSound->errorString();
       
    } else {
        m_soundHolder->putPronunciationSound(m_curWord, soundBuffer);
    }
}


void PronunciationSoundFactoryImpl::htmlResponseHeaderReceived(const QHttpResponseHeader &resp)
{
    if (resp.statusCode() == 302) {
        connect (m_httpHtml, SIGNAL(requestFinished(int, bool)),
                 this, SLOT(httpHtmlRequestFinished(int, bool)));

        QString location = resp.value("location");
        QString urlString("http://www.yourdictionary.com" + location);
        QUrl url(urlString);
        m_httpHtml->setHost(url.host());
        m_httpHtmlGetId = m_httpHtml->get(url.path(), &m_htmlBuffer);
    }
}







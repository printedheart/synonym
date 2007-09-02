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

#ifndef WORDSOUNDIMPL_H
#define WORDSOUNDIMPL_H


#include <QtCore>
#include "wordsoundinterface.h"


class QHttp;
class QHttpResponseHeader;
class WordSoundImpl : public QObject, public WordSoundInterface
{
Q_OBJECT  
public:

    WordSoundImpl(QObject *parent, WordSoundHolder *soundHolder);
    ~WordSoundImpl();

    void findSoundFile(const QString &word);
    QString m_curWord;
    QHttp *m_httpHtml;
    int m_httpHtmlGetId;
    QHttp *m_httpSound;
    int m_httpSoundGetId;
    QBuffer m_htmlBuffer;

private slots:

    void htmlResponseHeaderReceived(const QHttpResponseHeader &resp);
    void httpHtmlRequestFinished(int requestId, bool error);
    void httpSoundRequestFinished(int requestId, bool error);
};


#endif




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
#ifndef PRONUNCIATIONSOUNDFACTORYIMPL_H
#define PRONUNCIATIONSOUNDFACTORYIMPL_H

#include "pronunciationsoundfactory.h"
#include <QObject>
#include <QBuffer>
#include <QHttpResponseHeader>
class QHttp;


/**
	@author Sergey Melderis <sergey.melderis@gmail.com>
*/
class PronunciationSoundFactoryImpl : public QObject, public PronunciationSoundFactory
{
Q_OBJECT
public:
    PronunciationSoundFactoryImpl(QObject *parent);

    ~PronunciationSoundFactoryImpl();

    void findSound(const QString &word);
    void setSoundReceiver(PronunciationSoundHolder *soundHolder);
private:
    PronunciationSoundHolder *m_soundHolder;

    QString m_curWord;
    QHttp *m_httpHtml;
    int m_httpHtmlGetId;
    QHttp *m_httpSound;
    int m_httpSoundGetId;
    QBuffer m_htmlBuffer;
    QBuffer *soundBuffer;

private slots:
    void htmlResponseHeaderReceived(const QHttpResponseHeader &resp);
    void httpHtmlRequestFinished(int requestId, bool error);
    void httpSoundRequestFinished(int requestId, bool error);
    

};

#endif

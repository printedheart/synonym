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
#include "player.h"

#include <QtCore>
Player::Player()
 : QObject(), m_playProcess(0)
{
}


Player::~Player()
{
}


void Player::play(QIODevice *sound)
{
    
    if (!m_playProcess) {
        m_playProcess = new QProcess();
        connect (m_playProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
                 this, SLOT(playFinished()));
    }
    m_playProcess->close();
    QFile *m_soundFile = new QFile("sound.wav");
    QDir::setCurrent("/tmp");
    if (!m_soundFile->open(QIODevice::ReadWrite)) {
        qDebug() << "Cannot open temp file";
        return;
    }

    if (sound->isOpen()) {
        sound->close();
    }
    sound->open(QIODevice::ReadOnly);
    QByteArray all = sound->readAll();
    m_soundFile->write(all);
    m_soundFile->flush();

    QString program = "aplay";
    QStringList args;
    args  <<  m_soundFile->fileName();
    QProcess *proccess = new QProcess();
    proccess->start(program, args);
}

void Player::playFinished()
{
    m_playProcess->close();
    m_soundFile->remove();
    delete m_soundFile;
}



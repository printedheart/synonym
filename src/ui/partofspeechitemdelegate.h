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
#ifndef PARTOFSPEECHITEMDELEGATE_H
#define PARTOFSPEECHITEMDELEGATE_H

#include <QAbstractItemDelegate>
#include <QListView>

/**
	@author Sergejs <sergey.melderis@gmail.com>
*/
class PartOfSpeechItemDelegate : public QAbstractItemDelegate
{
Q_OBJECT
public:
    PartOfSpeechItemDelegate(QObject *parent = 0);

    ~PartOfSpeechItemDelegate();

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;


    
};


class PartOfSpeechItemView : public QListView
{
Q_OBJECT
        
public:
    PartOfSpeechItemView(QWidget *parent = 0) :
        QListView(parent) {}

    ~PartOfSpeechItemView() {};
protected:
    
    virtual void resizeEvent(QResizeEvent *resizeEvent);
};

#endif

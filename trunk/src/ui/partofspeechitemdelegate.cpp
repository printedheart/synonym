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
#include "partofspeechitemdelegate.h"
#include <QtCore>
#include <QtGui>


PartOfSpeechItemDelegate::PartOfSpeechItemDelegate(QObject *parent)
 : QAbstractItemDelegate(parent)
{
}


PartOfSpeechItemDelegate::~PartOfSpeechItemDelegate()
{
}


// QSize PartOfSpeechItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
// {
//     QAbstractItemView *view = dynamic_cast<QAbstractItemView*>(parent());
//     QFontMetrics fontMetrics = view->fontMetrics();
//     QRect rect = fontMetrics.boundingRect(index.data().toString());
//     int proportion = (rect.width() / (view->width() + 4));
//     if (proportion == 0 || rect.width() > view->width())
//         proportion++;
//
//     return QSize(view->width() - 4, rect.height() * proportion);
// }
/*
QSize PartOfSpeechItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QAbstractItemView *view = dynamic_cast<QAbstractItemView*>(parent());
    QFontMetricsF fontMetrics(view->fontMetrics());
    QString str = index.data().toString();
    QString elided;
    int width = view->viewport()->width() - 12;
    int times = 1;
    bool more;
    QRegExp spaces("^\\s*$");
    do {
        more = false;
        elided = fontMetrics.elidedText(str, Qt::ElideRight, width);
//         if (elided != str) {
//             times++;            
//             
//             str.remove(0, elided.lastIndexOf(QChar(' ')));
//         }
        
        if (elided != str) {
            str.remove(0, elided.lastIndexOf(QChar(' ')) + 1);
            if (!spaces.exactMatch(str)) {
                times++;
                more = true;
            }
        } 
    } while(more);
    
    return QSize(width,
                 (fontMetrics.size(Qt::AlignJustify, index.data().toString())
                         .height() * times) - 3);
}
*/

QSize PartOfSpeechItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QAbstractItemView *view = dynamic_cast<QAbstractItemView*>(parent());
    QFontMetricsF fontMetrics(view->fontMetrics());
    QString str = index.data().toString().trimmed();
    QStringList words = str.split(QChar(' '));
    QRectF boundingRect = fontMetrics.boundingRect(str);
    int width = view->viewport()->width() - 6;
    int times = 0;
    while (words.size() > 0) {
        times++;
        qreal lineWidth = 0;
        bool enoughSpace = true;
        do {
            QString word = words.first();
            qreal wordWidth = fontMetrics.width(word);
            if (wordWidth + lineWidth < width) {
                lineWidth += wordWidth;
                lineWidth += fontMetrics.width(QChar(' '));
                words.removeFirst();
            } else 
                enoughSpace = false;
                
        } while (enoughSpace && words.size() > 0);
    }
    return QSize(width, boundingRect.height() * times - times + 1);
}
        
void PartOfSpeechItemDelegate::paint(QPainter *painter,
                                     const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect(option.rect.topLeft(), option.rect.bottomRight());
    if (option.state & QStyle::State_Selected)
        painter->fillRect(rect, option.palette.highlight());
    
    painter->save();
    painter->setPen(QPen(Qt::gray));
    painter->drawLine(rect.topLeft(), rect.topRight());
    painter->restore();
    
    rect.setLeft(rect.left() + 4);
    rect.setTop(rect.top() + 2);
    rect.setRight(rect.right() - 2);
    painter->drawText(rect, Qt::TextWordWrap , index.data().toString());
}


void PartOfSpeechItemView::resizeEvent(QResizeEvent *resizeEvent)
{
    reset();
    QListView::resizeEvent(resizeEvent);
}


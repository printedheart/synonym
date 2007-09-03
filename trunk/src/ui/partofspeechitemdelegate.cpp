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


PartOfSpeechItemDelegate::PartOfSpeechItemDelegate(PartOfSpeechItemView *parent)
 : QAbstractItemDelegate(parent)
{
}


PartOfSpeechItemDelegate::~PartOfSpeechItemDelegate()
{
}




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
    if (option.state & QStyle::State_Selected) {
        painter->fillRect(rect, option.palette.highlight());
    } else {
        PartOfSpeechItemView *view = dynamic_cast<PartOfSpeechItemView*>(parent());
        if (index == view->indexUnderMouse()) {
            QBrush brush(QColor(218, 218, 218));
            painter->fillRect(rect, brush);
        }
    }
    
    painter->save();
    painter->setPen(QPen(Qt::gray));
    painter->drawLine(rect.topLeft(), rect.topRight());
    painter->restore();
    
    rect.setLeft(rect.left() + 4);
    rect.setTop(rect.top() + 2);
    rect.setRight(rect.right() - 2);
    painter->drawText(rect, Qt::TextWordWrap , index.data().toString());
}


PartOfSpeechItemView::PartOfSpeechItemView(QWidget *parent):
    QListView(parent)
{
    
}
    

void PartOfSpeechItemView::resizeEvent(QResizeEvent *resizeEvent)
{
    reset();
    QListView::resizeEvent(resizeEvent);
}


void PartOfSpeechItemView::mouseMoveEvent(QMouseEvent *event)
{
    QAbstractItemView::mouseMoveEvent(event);
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        m_indexUnderMouse = index;
        update(index);
        QModelIndex above = index.sibling(index.row() - 1, index.column());
        if (above.isValid()) {
            update(above);
        }
        QModelIndex below = index.sibling(index.row() + 1, index.column());
        if (below.isValid()) {
            update(below);
        }
    }
    
    bool mouseCloseToEdge = event->y() > viewport()->height() - 1 || event->y() < 1;
    if (!index.isValid() || mouseCloseToEdge) {
        QModelIndex above = index.sibling(m_indexUnderMouse.row() - 1, m_indexUnderMouse.column());
        if (above.isValid()) {
            update(above);
        }
        QModelIndex below = index.sibling(m_indexUnderMouse.row() + 1, m_indexUnderMouse.column());
        if (below.isValid()) {
            update(below);
        }
        QModelIndex old = m_indexUnderMouse;
        m_indexUnderMouse = QModelIndex();
        update(old);
        emit entered(QModelIndex());
    }

}

void PartOfSpeechItemView::wheelEvent(QWheelEvent*event)
{
    QAbstractItemView::wheelEvent(event);
    QModelIndex index = indexAt(event->pos());
    if (index.isValid()) {
        m_indexUnderMouse = index;
    } else {
        m_indexUnderMouse = QModelIndex();
    }
    reset();
}


void PartOfSpeechItemView::highlightItem(const QModelIndex &index)
{
    if (index.isValid()) {
        m_indexUnderMouse = index;
        update(index);
    }    
}

void PartOfSpeechItemView::clearHighlighting()
{
    if (m_indexUnderMouse.isValid()) {
        QModelIndex temp = m_indexUnderMouse;
        m_indexUnderMouse = QModelIndex();
        update(temp);
    }
}
    

QModelIndex  PartOfSpeechItemView::indexUnderMouse() const
{
    return m_indexUnderMouse;
}






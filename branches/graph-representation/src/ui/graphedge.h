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
#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include <QGraphicsItem>
#include <QPen>

class GraphicsNode;
/**
    @author Sergejs <sergey.melderis@gmail.com>
*/



class GraphEdge : public QGraphicsItem
{
public:
    GraphEdge();
    GraphEdge(GraphicsNode *sourceNode, GraphicsNode *destNode);

    ~GraphEdge();

    enum { Type = UserType + 111 };
    int type() const { return Type; }

    void setSource(GraphicsNode *source);
    GraphicsNode* source() const;

    void setDest(GraphicsNode *dest);
    GraphicsNode* dest() const;

    void adjust();
    
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    
    void setToolTip(const QString &toolTip);
    
    void setPen(QPen pan);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
private:
    GraphicsNode *m_source, *m_dest;
    QPointF m_sourcePoint, m_destPoint;
    QPen m_pen;
    
    /* Tooltips */
    QGraphicsPathItem *m_toolTip;
    QGraphicsLineItem *m_pointer;
    QGraphicsEllipseItem *m_connector;
    QString m_toolTipString;
    void createToolTip();
    void adjustToolTipPos();
};

#endif

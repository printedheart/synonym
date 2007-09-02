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


class GraphNode;        
/**
	@author Sergejs <sergey.melderis@gmail.com>
*/



class GraphEdge : public QGraphicsItem
{
public:
    GraphEdge();
    GraphEdge(GraphNode *sourceNode, GraphNode *destNode);

    ~GraphEdge();

    enum { Type = UserType + 111 };
    int type() const { return Type; }

    void setSource(GraphNode *source);
    GraphNode* source() const;

    void setDest(GraphNode *dest);
    GraphNode* dest() const;

    void adjust();
    
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
protected:

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    GraphNode *m_source, *m_dest;
    QPointF m_sourcePoint, m_destPoint;
};

#endif

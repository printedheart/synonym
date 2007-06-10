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
#include "graphedge.h"
#include "graphnode.h"

#include <math.h>
#include <QtGui>        


GraphEdge::GraphEdge()
    :m_source(0), m_dest(0)
{

}
        
GraphEdge::GraphEdge(GraphNode *sourceNode, GraphNode *destNode)
    :m_source(sourceNode), m_dest(destNode)
{
    m_source->addEdge(this);
    m_dest->addEdge(this);
    adjust();
}


GraphEdge::~GraphEdge()
{
}

void GraphEdge::setSource(GraphNode *source)
{
    m_source = source;
    source->addEdge(this);
}

GraphNode* GraphEdge::source() const
{
    return m_source;
}



void GraphEdge::setDest(GraphNode *dest)
{
    m_dest = dest;
    dest->addEdge(this);
}
    
GraphNode* GraphEdge::dest() const
{
    return m_dest;
}

void GraphEdge::adjust()
{
    if (!m_source || !m_dest)
        return;

    qreal nodeRadius = 5.0;
    QLineF line(mapFromItem(m_source, 0, 0), mapFromItem(m_dest, 0, 0));
    qreal length = line.length();

    
    //QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);

    prepareGeometryChange();
    m_sourcePoint = line.pointAt(nodeRadius / length); //line.p1() + edgeOffset;
    m_destPoint = line.pointAt(1 - nodeRadius / length);// line.p2() -  edgeOffset;
}






QPainterPath GraphEdge::shape() const
{
    return QGraphicsItem::shape();
}

QRectF GraphEdge::boundingRect() const
{
    if (!m_source || !m_dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth) / 2.0;

    return QRectF(m_sourcePoint, QSizeF(m_destPoint.x() - m_sourcePoint.x(),
                  m_destPoint.y() - m_sourcePoint.y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}


void GraphEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!m_source || !m_dest)
        return;
    // Draw the line itself
    QLineF line(m_sourcePoint, m_destPoint);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawLine(line);
}


void GraphEdge::calculateForces()
{
    if (!m_source || !m_dest)
        return;

    QLineF line(mapFromItem(m_source, 0, 0), mapFromItem(m_dest, 0, 0));
    qreal dx = line.dx();
    qreal dy = line.dy();
    
    qreal distance2 = dx * dx + dy * dy;
    qreal distance = sqrt(distance2);
    //qDebug() << "Edge distance " << distance;
    static int REST_DISTANCE = 75;
    qreal force = 0.2 * (distance - REST_DISTANCE);
        
    m_source->applyForce(force * (dx / distance), force * ( dy / distance));
    m_dest->applyForce(-force * (dx / distance), -force * (dy / distance));
}






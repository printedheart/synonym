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
#include "edge.h"
#include "node.h"
#include <QPointF>

Edge::Edge(const QString &id, DataNode *sourceNode, DataNode *destNode, QObject *parent)
    :QObject(parent), m_id(id), m_source(sourceNode), m_dest(destNode)
{
    m_source->addEdge(this);
    m_dest->addEdge(this);
}




Edge::~Edge()
{
}

DataNode *Edge::sourceNode() const
{
    return m_source;
}

void Edge::setSourceNode(DataNode *node)
{
    m_source = node;
}

DataNode *Edge::destNode() const
{
    return m_dest;
}

void Edge::setDestNode(DataNode *node)
{
    m_dest = node;
}

QString Edge::id() const
{
    return m_id;
}
/*
void Edge::adjust()
{
    if (!m_source || !m_dest)
        return;

    QGraphicsItem *source = dynamic_cast<QGraphicsItem*>(m_source);
    QGraphicsItem *dest = dynamic_cast<QGraphicsItem*>(m_dest);
    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();
    QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
    
    prepareGeometryChange();
    QPointF sourcePoint = line.p1() + edgeOffset;
    QPointF destPoint = line.p2() - edgeOffset;
    setLine(QLineF(sourcePoint, destPoint));
}
*/





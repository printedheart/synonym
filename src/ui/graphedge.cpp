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
    :m_source(0), m_dest(0), m_toolTip(0)
{

}
        
GraphEdge::GraphEdge(GraphNode *sourceNode, GraphNode *destNode)
    :m_source(sourceNode), m_dest(destNode), m_toolTip(0)
{
    QColor c;
    c.setHsv(0.0, 0.0, 150);
    m_pen = QPen(c, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
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

    prepareGeometryChange();
    m_sourcePoint = line.pointAt(nodeRadius / length); 
    m_destPoint = line.pointAt(1 - nodeRadius / length);
    
    if (m_toolTip && m_toolTip->isVisible()) {
        adjustToolTipPos();
    }
}




QPainterPath GraphEdge::shape() const
{
   if (!m_source || !m_dest) {
       return QGraphicsItem::shape();
   }
    
   //shape() is used for mouse hover events. 
   //We want to make the region a little wider then the line.
   QLineF line(m_sourcePoint, m_destPoint);
   QLineF line1(0, 0, m_sourcePoint.y() > m_destPoint.y() ? 10.0 : -10.0, 0);
   qreal angle = line.angle(line1);
   QMatrix matrix;
   matrix.rotate(angle);
   QLineF rotatedLine = matrix.map(line);
   
   QPolygonF polygon;
   polygon << QPointF(rotatedLine.x1() - 2, rotatedLine.y1() + 1)
           << QPointF(rotatedLine.x1() - 2, rotatedLine.y2() - 1)
           << QPointF(rotatedLine.x2() + 2, rotatedLine.y2() - 1)
           << QPointF(rotatedLine.x2() + 2, rotatedLine.y1() + 1);
   
   QPolygonF shape = matrix.inverted().map(polygon);
   
   QPainterPath path;
   path.addPolygon(shape);
   return path;
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
    
    QLineF line(m_sourcePoint, m_destPoint);
    painter->setPen(m_pen);
    painter->drawLine(line);
}

void GraphEdge::setPen(QPen pen)
{
    m_pen = pen;
}

void GraphEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!m_toolTip) {
        if (!m_toolTipString.isEmpty())
            createToolTip();
    }
    
    if (m_toolTip) {
        adjustToolTipPos();        
        m_pointer->setVisible(true);
        m_toolTip->setVisible(true);
        m_toolTip->update();
        m_pointer->update();
    }
}
void GraphEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_toolTip) {
        m_toolTip->setVisible(false);
        m_pointer->setVisible(false);
        m_toolTip->update();
        m_pointer->update();
    }
}

void GraphEdge::createToolTip()
{
    m_toolTip = new QGraphicsPathItem();
    scene()->addItem(m_toolTip);
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(m_toolTipString, m_toolTip);
     
    QRectF rect = textItem->boundingRect();
    rect.setLeft(rect.left() - 5);
    rect.setRight(rect.right() + 5);

    // Make a rectangle with nice rounded corners.
    qreal width = 12.0;
    qreal half = width / 2;
    QPainterPath roundRectPath;
    roundRectPath.moveTo(rect.left() + half, rect.bottom());
    roundRectPath.arcTo(rect.left(), rect.bottom() - width, width, width, - 90.0, - 90.0);
    roundRectPath.lineTo(rect.left(), rect.top() + half);
    roundRectPath.arcTo(rect.left(), rect.top(), width, width,  180, - 90.0);
    roundRectPath.lineTo(rect.right() - half, rect.top());
    roundRectPath.arcTo(rect.right() - width, rect.top(), width, width, 90.0, - 90.0);
    roundRectPath.lineTo(rect.right(), rect.bottom() - half);
    roundRectPath.arcTo(rect.right() - width, rect.bottom() - width, width, width, 0.0, - 90.0);
    roundRectPath.closeSubpath();
    m_toolTip->setPath(roundRectPath);
    QPen pen;
    pen.setWidth(1);
    m_toolTip->setPen(pen);
    m_toolTip->setBrush(QBrush(Qt::white));    
    m_toolTip->setVisible(true);
    m_toolTip->setZValue(4);
    textItem->setZValue(5);
    m_pointer = new QGraphicsLineItem(this);
    m_pointer->setZValue(3);
    m_connector = new QGraphicsEllipseItem(QRect(-3, -3, 6, 6), m_pointer);
    m_connector->setPen(pen);
    m_connector->setBrush(QBrush(Qt::white));        
}

void GraphEdge::setToolTip(const QString &toolTip)
{
    m_toolTipString = toolTip;
}

void GraphEdge::adjustToolTipPos()
{
    QLineF line(m_sourcePoint, m_destPoint);
    QPointF middle = line.pointAt(0.5);
        
    QPointF toolTipPos = middle;
    toolTipPos.setX(toolTipPos.x() + 30);
    toolTipPos.setY(toolTipPos.y() - 20);
    m_toolTip->setPos(mapFromScene(toolTipPos));
        
        
        
    QPointF pointerPos;
    pointerPos.setY(toolTipPos.y() + m_toolTip->path().boundingRect().height() / 2);
    pointerPos.setX(toolTipPos.x());
        
    m_pointer->setLine(middle.x(), middle.y(), pointerPos.x(), pointerPos.y());
    m_connector->setPos(m_pointer->mapFromParent(middle));
}





/***************************************************************************
 *   Copyright (C) 2007 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
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
#include "wordnetutil.h"


#include <math.h>
#include <QtGui>        

GraphicsEdge::GraphicsEdge(const QString &id, GraphicsNode *source,
                           GraphicsNode *dest, WordGraph *graph)
    : m_id(id), m_source(source), m_dest(dest), m_graph(graph), m_directed(false), 
      m_connector(0), m_pointer(0), m_toolTip(0)     
{
    m_source->m_edges << this;
    m_dest->m_edges << this;
    
    QColor c;
    c.setHsv(0, 0, 140);
    m_pen = QPen(c, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin); 
    
    setAcceptsHoverEvents(true);
    m_type = Relationship::Undefined;
}
        

GraphicsEdge::GraphicsEdge(const GraphicsEdge &o)
    : QGraphicsItem(),  m_id(o.id()), m_source(o.source()), m_dest(o.dest()), 
      m_graph(o.graph()), 
      m_connector(0), m_pointer(0), m_toolTip(0),
      m_pen(o.m_pen), m_type(o.relationship())                    
{
    setAcceptsHoverEvents(true);
}

GraphicsEdge::~GraphicsEdge()
{
    // QGraphicsItem claims to delete all its children
}

GraphicsEdge * GraphicsEdge::clone() const
{
    return new GraphicsEdge(*this);
}

QString GraphicsEdge::id() const
{
    return m_id;
}

WordGraph * GraphicsEdge::graph() const
{
    return m_graph;
}


GraphicsNode* GraphicsEdge::source() const
{
    return m_source;
}

    
GraphicsNode* GraphicsEdge::dest() const
{
    return m_dest;
}

GraphicsNode* GraphicsEdge::adjacentNode(GraphicsNode *node) const
{
    if (m_source == node)
        return m_dest;
    if (m_dest == node)
        return m_source;
    return 0;
}

void GraphicsEdge::adjust()
{
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




QPainterPath GraphicsEdge::shape() const
{
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


QRectF GraphicsEdge::boundingRect() const
{
    qreal penWidth = 1;
    qreal extra = (penWidth) / 2.0;

    return QRectF(m_sourcePoint, QSizeF(m_destPoint.x() - m_sourcePoint.x(),
                  m_destPoint.y() - m_sourcePoint.y()))
            .normalized()
            .adjusted(-extra, -extra, extra, extra);
}


void GraphicsEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QLineF line(m_sourcePoint, m_destPoint);
    painter->setPen(m_pen);
    painter->drawLine(line);
}

void GraphicsEdge::setPen(QPen pen)
{
    m_pen = pen;
}

void GraphicsEdge::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!m_toolTip) {
        if (relationship() != Relationship::Undefined) {
            createToolTip();
        }
    }
    
    if (m_toolTip) {
        setZValue(3);
        adjustToolTipPos();        
        m_pointer->setVisible(true);
        m_toolTip->setVisible(true);
        m_toolTip->update();
        m_pointer->update();
    }
}
void GraphicsEdge::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_toolTip) {
        setZValue(1);
        m_toolTip->setVisible(false);
        m_pointer->setVisible(false);
        m_toolTip->update();
        m_pointer->update();
    }
}

void GraphicsEdge::createToolTip()
{
    
    int sourcePos = source()->data(POS).toInt();
    int destPos = dest()->data(POS).toInt();
    QString text;
    if (Relationship::applies(relationship(), sourcePos)) {
        text = Relationship::toString(relationship(), sourcePos);
    } else if (Relationship::applies(relationship(), destPos)) {
        text = Relationship::toString(relationship(), destPos);
    }
    if (text.isEmpty())
        return;
    
    m_toolTip = new QGraphicsPathItem(this);
    QGraphicsSimpleTextItem *textItem = new QGraphicsSimpleTextItem(text, m_toolTip);
     
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
    m_toolTip->setZValue(5);
    textItem->setZValue(6);
    
    m_pointer = new QGraphicsLineItem(this);
    m_pointer->setZValue(3);
    m_connector = new QGraphicsEllipseItem(QRect(-3, -3, 6, 6), m_pointer);
}

void GraphicsEdge::setToolTip(const QString &toolTip)
{
    m_toolTipString = toolTip;
}

void GraphicsEdge::adjustToolTipPos()
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


void GraphicsEdge::setRelationship(Relationship::Type type)
{
    m_type = type;
    if (type == Relationship::Undefined) {
        m_pen.setStyle(Qt::SolidLine);
    } else {
        QVector<qreal> dashes;
        dashes << 10 << 10;
        m_pen.setDashPattern(dashes); 
        if (type == Relationship::Antonym) {
            QColor c;
            c.setHsv(6, 243, 214);
            m_pen.setColor(c);
            m_pen.setWidthF(0.6);
        } else {
            QColor c;
            c.setHsv(0, 0, 140);
            m_pen.setColor(c);
        }
    }
}
    
Relationship::Type GraphicsEdge::relationship() const
{
    return m_type;
}


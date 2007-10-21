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
#include "graphnode.h"
#include "graphedge.h"
#include "node.h"
#include "graphscene.h"        
#include <QtGui>
#include <QtSvg>
#include <cmath> 
                
GraphicsNode::GraphicsNode(GraphScene *scene)
    :QGraphicsItem(0), m_mass(1.0)
{
    m_scene = scene;   
    m_intermedPos.setX(2.0);
}

GraphicsNode::~GraphicsNode()
{
}


void GraphicsNode::addEdge(GraphEdge *edge)
{
     m_edges.append(edge);
}
    
QList<GraphEdge*> GraphicsNode::edges() const
{ 
    return m_edges;
}

bool GraphicsNode::advance()
{
     if (qAbs(m_newPos.x() - pos().x()) > 0.1 || qAbs(m_newPos.y() - pos().y()) > 0.1) {
         setPos(m_newPos);
         return true;
     }

    return false;
}

void GraphicsNode::setNewPos(QPointF newPos)
{
    m_newPos = newPos;
}

QVariant GraphicsNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
        case ItemPositionChange:
            foreach (GraphEdge *edge, m_edges)
                edge->adjust();
            m_scene->itemMoved();
            break;
        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}


QString GraphicsNode::id() const
{
    return dataNode()->id();
}

void GraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    if (event->button() == Qt::LeftButton) {
        if (event->buttonDownScenePos(Qt::LeftButton) == event->scenePos()) {
            m_scene->propogateClickEvent(this);
        }
    }
 
}



/**
    WordGraphicsNode definition
*/
WordGraphicsNode::WordGraphicsNode(GraphScene *scene)
    :GraphicsNode(scene), m_wordNode(0)
{
    setZValue(2);
    m_font = QFont("Dejavu Sans", 8, QFont::Normal);
}

WordGraphicsNode::~WordGraphicsNode()
{}

void WordGraphicsNode::setNode(Node *node)
{
    if (node) {
        m_wordNode = qobject_cast<WordNode *>(node);
        m_font.setPointSize(m_wordNode->fixed() ? 14 : 8);
    }
        
    else
        m_wordNode = 0;
}

Node * WordGraphicsNode::dataNode() const
{
    return m_wordNode;
}

QPainterPath WordGraphicsNode::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

QRectF WordGraphicsNode::boundingRect() const
{
    if (!m_wordNode)
        return QRectF();
    
    QFontMetrics metrics(m_font);
    QRectF rect = metrics.boundingRect(m_wordNode->word());
        
    if (dataNode()->fixed()) 
        return rect.translated(- rect.width() / 2, rect.height() / 8);
    
    GraphEdge *edge = edges().first();
    GraphicsNode *other = 0;
    if (edge->source() != this)
        other = edge->source();
    else
        other = edge->dest();
    
    qreal angle = atan2(other->mapFromScene(pos()).y(), other->mapFromScene(pos()).x());
    qreal width = rect.width();
    qreal dx = -width / 2 + (width / 2) * cos(angle); 
    
    qreal dy = 0.0;
    if (angle > 0)
        dy = (rect.height() / 3) * sin(angle);  
    
    rect.translate(dx, dy);
    return rect;
}

void WordGraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    
    Q_UNUSED(widget);
    
    painter->setClipRect( option->exposedRect );
    painter->setFont(m_font);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_wordNode->word());
     
    
   // painter->drawRect(boundingRect());
}

bool WordGraphicsNode::advance()
{
    if (m_wordNode->fixed())
        return false;

    return GraphicsNode::advance();
}

void WordGraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    
    if (event->button() == Qt::LeftButton) {
        if (event->buttonDownScenePos(Qt::LeftButton) == event->scenePos()) {
            m_scene->propogateClickEvent(this);
        }
    }
 
}

void WordGraphicsNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_mousePressPos = event->pos();
    QGraphicsItem::mousePressEvent(event);
}





/**
    MeaningGraphicsNode definition
*/

static const QColor colors[4] = { Qt::red, Qt::green, Qt::blue, Qt::yellow };

int MeaningGraphicsNode::m_radius  = 5;

MeaningGraphicsNode::MeaningGraphicsNode(GraphScene *scene)
    :GraphicsNode(scene), m_node(0), m_toolTip(0), m_defItem(0)
{
    setZValue(1);        
}

MeaningGraphicsNode::~MeaningGraphicsNode()
{
    delete m_defItem;
    delete m_toolTip;
}

void MeaningGraphicsNode::setNode(Node *node)
{
    if (node) {
        m_node = qobject_cast<MeaningNode *>(node);
    } else
        m_node = 0;

}

Node * MeaningGraphicsNode::dataNode() const
{
    return m_node;
}

QRectF MeaningGraphicsNode::boundingRect() const
{
    if (!m_node)
        return QRectF();
        
    qreal adjust = 3;
    return QRectF(
                  -m_radius - adjust, -m_radius - adjust,
                   m_radius * 2 + adjust * 2, m_radius * 2 + adjust * 2);
}


QPainterPath MeaningGraphicsNode::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void MeaningGraphicsNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    if (!m_node)
        return;

    painter->setClipRect( option->exposedRect );
    //painter->setPen(colors[m_node->partOfSpeech()]);
    painter->setBrush(QBrush(colors[m_node->partOfSpeech() - 1]));
    painter->setPen(Qt::black);
    painter->drawEllipse(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
}

void MeaningGraphicsNode::setActivated(bool activated)
{
    if (activated) {
        showToolTip(pos());
    } else {
        hideToolTip();
    }
}

void MeaningGraphicsNode::showToolTip(const QPointF &pos)
{
    if (!m_toolTip)
        createToolTip();
    if (!m_toolTip->scene()) {
        scene()->addItem(m_toolTip);
    } 
    QPointF scenePos = pos;
    QRectF tooTipRect = m_toolTip->boundingRect();
    
    scenePos.setX(scenePos.x() + 30);
    scenePos.setY(scenePos.y() - tooTipRect.height() - 20);
    m_toolTip->setPos(mapFromScene(scenePos));
    m_toolTip->setVisible(true);
    m_toolTip->update();
    setZValue(3);
    
    QPointF toolTipLeftPos = mapFromItem(m_toolTip, m_toolTip->boundingRect().bottomLeft());
    QLineF line(mapFromParent(this->pos()), toolTipLeftPos);
    QPointF startPoint = line.pointAt(m_radius / line.length());
    
    QLineF pointerLine = QLineF(startPoint, toolTipLeftPos);
    pointerLine.setLength(pointerLine.length() + 10);
    m_pointer->setLine(pointerLine);
    m_pointer->setVisible(true);
    m_pointer->update();
    
    
}

void MeaningGraphicsNode::hideToolTip()
{
    setZValue(1);
    m_toolTip->setVisible(false);
    m_pointer->setVisible(false);    
}
    

void MeaningGraphicsNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{    
    showToolTip(event->scenePos());
    m_scene->signalMouseHovered(this);
}

void MeaningGraphicsNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    hideToolTip();   
    m_scene->signalMouseHoverLeaved(this);
}

void MeaningGraphicsNode::createToolTip()
{
    if (!m_toolTip) {
        m_toolTip = new QGraphicsPathItem(this);
        m_defItem = new QGraphicsTextItem(m_toolTip, 0);
    }
           
    // Prepare definition dext
    QString definition = m_node->meaning().section(';', 0, 0);
    QString examples = m_node->meaning().section(';', 1);
    definition.remove(0, 1);
    examples.remove(examples.length() - 2, 1);
    QString defHtml = definition;
    defHtml.append("<br>-------------------------------<br>");
    defHtml.append(examples);
    m_defItem->document()->setHtml(defHtml);
    m_defItem->setTextWidth(160);
    QRectF rect = m_defItem->boundingRect();
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
    m_defItem->setZValue(6);
    
    m_pointer = new QGraphicsLineItem(this);
    m_pointer->setZValue(4);
}


void MeaningGraphicsNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
}

void MeaningGraphicsNode::adjustToolTipPos()
{
    
}



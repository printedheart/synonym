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
                
GraphNode::GraphNode(GraphScene *scene)
	:QGraphicsItem(0), m_mass(1.0), m_visited(false)
{
    m_scene = scene;   
    m_intermedPos.setX(2.0);
}

GraphNode::~GraphNode()
{
}

QList<GraphNode*> GraphNode::neighbors() const
{
    QList<GraphNode*> neighbors;
    foreach (GraphEdge *edge, edges()) {
        if (edge->source() == this)
            neighbors.append(edge->dest());
        else if (edge->dest() == this)
            neighbors.append(edge->source());
    }
    return neighbors;
    
}

bool GraphNode::advance()
{
     if (qAbs(m_newPos.x() - pos().x()) > 0.1 || qAbs(m_newPos.y() - pos().y()) > 0.1) {
         setPos(m_newPos);
         return true;
     }

    return false;
}

double GraphNode::mass() const
{
    return m_mass;
}

void GraphNode::applyForce(qreal fx, qreal fy)
{
    m_force += QPointF(fx, fy);
}

QPointF GraphNode::force() const
{
    return m_force;
}

void GraphNode::discardForce()
{
    m_force.setX(0.0);
    m_force.setY(0.0);
}

void GraphNode::setNewPos(QPointF newPos)
{
    m_newPos = newPos;
}

QVariant GraphNode::itemChange(GraphicsItemChange change, const QVariant &value)
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


void GraphNode::addAnimationPos(const QPointF &point)
{
    m_buffer[m_lastIndex++] = point;
}

QPointF & GraphNode::lastAnimationPos() 
{
    return m_buffer[m_lastIndex - 1];
}

QPointF  GraphNode::takeFirstAnimationPos()
{
    return m_buffer[m_firstIndex++];
}
    


int GraphNode::animationPosCount()
{
    return m_lastIndex - m_firstIndex;
}

void GraphNode::resetAnimation()
{
    m_firstIndex = 0;
    m_lastIndex = 0;    
}






/**
    PhraseGraphNode definition
*/
PhraseGraphNode::PhraseGraphNode(GraphScene *scene)
    :GraphNode(scene), m_phraseNode(0)
{
    setZValue(2);
    m_font = QFont("Dejavu Sans", 8, QFont::Normal);
}

PhraseGraphNode::~PhraseGraphNode()
{}

void PhraseGraphNode::setDataNode(DataNode *node)
{
    if (node) {
        m_phraseNode = qobject_cast<PhraseNode *>(node);
        m_font.setPointSize(m_phraseNode->fixed() ? 14 : 8);
    }
        
    else
        m_phraseNode = 0;
}

DataNode * PhraseGraphNode::dataNode() const
{
    return m_phraseNode;
}

QPainterPath PhraseGraphNode::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

QRectF PhraseGraphNode::boundingRect() const
{
    if (!m_phraseNode)
        return QRectF();
    
    QFontMetrics metrics(m_font);
    QRectF rect = metrics.boundingRect(m_phraseNode->phrase());
        
    if (dataNode()->fixed()) 
        return rect.translated(- rect.width() / 2, rect.height() / 8);
    
    GraphEdge *edge = edges().first();
    GraphNode *other = 0;
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

void PhraseGraphNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    
    Q_UNUSED(widget);
    
    painter->setClipRect( option->exposedRect );
    painter->setFont(m_font);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_phraseNode->phrase());
     
    
   // painter->drawRect(boundingRect());
}

bool PhraseGraphNode::advance()
{
    if (m_phraseNode->fixed())
        return false;

    return GraphNode::advance();
}

void PhraseGraphNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    
    if (event->button() == Qt::LeftButton) {
        if (event->buttonDownScenePos(Qt::LeftButton) == event->scenePos())
            m_scene->propogateClickEvent(this);
    }
 
}

void PhraseGraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_mousePressPos = event->pos();
    QGraphicsItem::mousePressEvent(event);
}





/**
    MeaningGraphNode definition
*/

static const QColor colors[4] = { Qt::red, Qt::green, Qt::blue, Qt::yellow };

MeaningGraphNode::MeaningGraphNode(GraphScene *scene)
    :GraphNode(scene), m_node(0), m_toolTip(0), m_defItem(0)
{
    setZValue(1);        
}

MeaningGraphNode::~MeaningGraphNode()
{
    delete m_defItem;
    delete m_toolTip;
}

void MeaningGraphNode::setDataNode(DataNode *node)
{
    if (node) {
        m_node = qobject_cast<MeaningNode *>(node);
    } else
        m_node = 0;

}

DataNode * MeaningGraphNode::dataNode() const
{
    return m_node;
}

QRectF MeaningGraphNode::boundingRect() const
{
    if (!m_node)
        return QRectF();
        
    qreal adjust = 3;
    return QRectF(
                  -5 - adjust, -5 - adjust,
                   10 + adjust * 2, 10 + adjust * 2);
}


QPainterPath MeaningGraphNode::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void MeaningGraphNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    if (!m_node)
        return;

    painter->setClipRect( option->exposedRect );
    //painter->setPen(colors[m_node->partOfSpeech()]);
    painter->setBrush(QBrush(colors[m_node->partOfSpeech() - 1]));
    painter->setPen(Qt::black);
    painter->drawEllipse(-5, -5, 10, 10);
}




void MeaningGraphNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{    
    if (!m_toolTip)
        createToolTip();
    if (!m_toolTip->scene()) {
        scene()->addItem(m_toolTip);
    } 
    QPointF scenePos = mapToScene(event->pos());
    scenePos.setX(scenePos.x() + 20);
    m_toolTip->setPos(mapFromScene(scenePos));
    m_toolTip->setVisible(true);
    m_toolTip->update();
    setZValue(3);
}

void MeaningGraphNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setZValue(1);
    m_toolTip->setVisible(false);    
}

void MeaningGraphNode::createToolTip()
{
    if (!m_toolTip) {
        m_toolTip = new QGraphicsPathItem(this);
        m_defItem = new QGraphicsTextItem(m_toolTip, 0);
    }
           
    // Prepare definition dext
    qDebug() << m_node->meaning();
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
    m_toolTip->setZValue(4);
    m_defItem->setZValue(5);
}


void MeaningGraphNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
}



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
#include "wordgraph.h"
#include "wordnetutil.h"
#include "graphedge.h"
#include "graphscene.h"        
#include <QtGui>
#include <QtSvg>
#include <cmath>
                
                
#include "graphalgorithms.h"                
GraphicsNode::GraphicsNode(const QString &id, WordGraph *graph)
    : QGraphicsItem(), m_id(id), m_graph(graph), m_mass(1.0) 
{
    m_intermedPos.setX(2.0);
    
    setCursor(Qt::PointingHandCursor);
    setFlag(QGraphicsItem::ItemIsMovable);
}

GraphicsNode::~GraphicsNode()
{
}

GraphicsNode::GraphicsNode(const GraphicsNode &o)
    : QGraphicsItem(), m_font(o.m_font), m_id(o.id()), m_graph(o.graph()), m_mass(o.mass()), m_edges(o.edges())
{
    setPos(o.pos());
    setFlags(o.flags());
    setCursor(o.cursor());
    setData(LEVEL, o.data(LEVEL));
}



QString GraphicsNode::id() const
{
    return m_id;
}

    
QSet<GraphicsEdge*> GraphicsNode::edges() const
{ 
    return m_edges;
}

QSet<GraphicsNode*> GraphicsNode::neighbors() const
{
    QSet<GraphicsNode*> set; 
    findNeighbors(this, m_edges.begin(), m_edges.end(), set);
    return set;    
}

QSet<GraphicsNode*> GraphicsNode::outNeighbors() const
{
    QSet<GraphicsNode*> set; 
    foreach (GraphicsEdge *edge, m_edges) {
        if (edge->source() == this)
            set << edge->dest();
    }
    return set;
}


unsigned int GraphicsNode::degree() const
{
    return edges().size();
}


WordGraph * GraphicsNode::graph() const
{
    return m_graph;
}

bool GraphicsNode::advance(bool force)
{
    if (!flags().testFlag(QGraphicsItem::ItemIsMovable))
        return false;
    qreal tolerance = 0.05;
    if (force || (qAbs(m_newPos.x() - pos().x()) > tolerance && qAbs(m_newPos.y() - pos().y()) > tolerance)) {
        setPos(m_newPos);
        return true;
    }
    return false;
}

void GraphicsNode::setNewPos(QPointF newPos)
{
    m_newPos = newPos;
}

void GraphicsNode::addToNewPos(QPointF &p)
{
    m_newPos += p;
}

void GraphicsNode::addToNewPos(double dx, double dy)
{
    m_newPos.setX(m_newPos.x() + dx);
    m_newPos.setY(m_newPos.y() + dy);
}

QPointF& GraphicsNode::newPos()
{
    return m_newPos;
}



QVariant GraphicsNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (!scene())
        return value;
    
    switch (change) {
        case ItemPositionChange:
            foreach (GraphicsEdge *edge, m_edges) {
                edge->adjust();
            }
            break;
        default:
            break;
    };

    return QGraphicsItem::itemChange(change, value);
}


void GraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (event->buttonDownScenePos(Qt::LeftButton) == event->scenePos()) {
            graphScene()->signalClickEvent(this);
        }
    }
    QGraphicsItem::mouseReleaseEvent(event);
 
}

void GraphicsNode::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
    graphScene()->itemMoved();
    p = event->scenePos();
}


GraphScene * GraphicsNode::graphScene() const
{
    Q_ASSERT_X(scene() != 0, "graphScene()", id().toLatin1().data());
    if (scene()) 
        return qobject_cast<GraphScene*>(scene());
    
    return 0;
}


void GraphicsNode::setFont(const QFont &font)
{
    m_font = font;
}
        


/**
    WordGraphicsNode definition
*/
WordGraphicsNode::WordGraphicsNode(const QString &id, WordGraph *graph)
    :GraphicsNode(id, graph)
{
    setZValue(2);
}

WordGraphicsNode::WordGraphicsNode(const WordGraphicsNode &o)
    : GraphicsNode(o)
{        
    setData(WORD, o.data(WORD));
}

WordGraphicsNode * WordGraphicsNode::clone() const
{
    return new WordGraphicsNode(*this);
}

WordGraphicsNode::~WordGraphicsNode()
{}


QPainterPath WordGraphicsNode::shape() const
{
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}

QRectF WordGraphicsNode::boundingRect() const
{
    if (m_rectf.isNull() || qAbs(pos().x() - m_prevPos.x()) > 1.0) {
        const_cast<WordGraphicsNode*>(this)->calculateBoundingRect();    
    }  
    return m_rectf;
}

void WordGraphicsNode::calculateBoundingRect()
{
        
    if (!flags().testFlag(QGraphicsItem::ItemIsMovable)) {
        QFont font = m_font;
        font.setPointSize(qRound(m_font.pointSize() * 1.5));
        QFontMetrics metrics(font);
        QRectF rect = metrics.boundingRect(data(WORD).toString());    
        m_rectf = rect.translated(- rect.width() / 2, rect.height() / 8);
    } else {
        QFontMetrics metrics(m_font);
        QRectF rect = metrics.boundingRect(data(WORD).toString());    
        GraphicsEdge *edge = *(edges().begin());
        GraphicsNode *other = edge->adjacentNode(this);
    
        qreal angle = atan2(other->mapFromScene(pos()).y(), other->mapFromScene(pos()).x());
        qreal width = rect.width();
        qreal dx = -width / 2 + (width / 2) * cos(angle); 
        
        qreal dy = 0.0;
        if (angle > 0)
            dy = (rect.height() / 3) * sin(angle);  
        
        rect.translate(dx, dy);
        m_rectf = rect;
        m_prevPos = pos();        
    }
}

void WordGraphicsNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(widget);
    
    painter->setClipRect(option->exposedRect);
    
    painter->setFont(m_font);
    
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawText(boundingRect(), Qt::AlignCenter, data(WORD).toString());
}



void WordGraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
    
    if (event->button() == Qt::LeftButton) {
        if (event->buttonDownScenePos(Qt::LeftButton) == event->scenePos()) {
            graphScene()->signalClickEvent(this);
        }
    }
 
}

void WordGraphicsNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_mousePressPos = event->pos();
    QGraphicsItem::mousePressEvent(event);
}

void WordGraphicsNode::setFont(const QFont &font)
{
    GraphicsNode::setFont(font);
    calculateBoundingRect();
}



/**
    MeaningGraphicsNode definition
*/

const QColor MeaningGraphicsNode::DEFAULT_COLORS[4] = { QColor(213, 28, 8), QColor(152, 169, 47), QColor(222, 172, 47), QColor(86, 20, 122) };
static const int DEFAULT_RADIUS = 5;

MeaningGraphicsNode::MeaningGraphicsNode(const QString &id, WordGraph *graph)
    :GraphicsNode(id, graph), m_toolTip(0), m_defItem(0), m_pointer(0), m_radius(DEFAULT_RADIUS), m_boundingRect(QRect())
{
    setZValue(1);
    setAcceptsHoverEvents(true);        
    calculateBoundingRect();
}

MeaningGraphicsNode::MeaningGraphicsNode(const MeaningGraphicsNode &o)
    : GraphicsNode(o), m_toolTip(0), m_defItem(0), m_pointer(0), 
                   m_radius(o.m_radius), m_boundingRect(o.m_boundingRect)
{
    setZValue(1);
    setAcceptsHoverEvents(true);
    setData(POS, o.data(POS));
    setData(MEANING, o.data(MEANING));
}

MeaningGraphicsNode::~MeaningGraphicsNode()
{
}


MeaningGraphicsNode * MeaningGraphicsNode::clone() const
{
    return new MeaningGraphicsNode(*this);
}


QRectF MeaningGraphicsNode::boundingRect() const
{
    return m_boundingRect;
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
    painter->setClipRect( option->exposedRect );
    painter->setBrush(QBrush(circleColor()));
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
    graphScene()->signalMouseHovered(this);
}

void MeaningGraphicsNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hideToolTip();   
    graphScene()->signalMouseHoverLeaved(this);
}


static const int TOOL_TIP_WIDTH = 160;


void MeaningGraphicsNode::createToolTip()
{
    if (!m_toolTip) {
        m_toolTip = new QGraphicsPathItem(this);
        m_defItem = new QGraphicsTextItem(m_toolTip);
    }
    m_defItem->setFont(m_font);
           
    // Prepare definition dext
    QString meaning = data(MEANING).toString();
    QString samplesStr;
    QList<QVariant> samples = data(SAMPLES).toList();
    for (int i = 0; i < samples.size(); i++) {
        samplesStr.append("\"").append(samples[i].toString()).append("\"");
        if (i < samples.size() - 1)
            samplesStr.append("; ");
    }
    
    QString defHtml = meaning;
    
    
    if (samplesStr.length() > 0) {
        int dashWidth = QFontMetrics(m_font).width(QChar('-'));
        int dashesCount = TOOL_TIP_WIDTH / dashWidth;
        defHtml.append("<br>");
        for (int i = 0; i < dashesCount - 1; i++) 
            defHtml.append("-");
        defHtml.append("<br>");
        defHtml.append(samplesStr);
    }
    /* Debugging info
    defHtml.append(id());
    defHtml.append("<br>");
    defHtml.append("level: ");
    defHtml.append(data(LEVEL).toString());
    defHtml.append("<br>");
    defHtml.append("neighbors: ");
    defHtml.append(QString::number(neighbors().size()));
    */
    
    m_defItem->document()->setHtml(defHtml);
    m_defItem->setTextWidth(TOOL_TIP_WIDTH);
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


void MeaningGraphicsNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_toolTip && m_toolTip->isVisible()) {
        hideToolTip();   
        graphScene()->signalMouseHoverLeaved(this);
    }
    GraphicsNode::mouseReleaseEvent(event);
}


void MeaningGraphicsNode::calculateBoundingRect()
{
    qreal adjust = 3;
    m_boundingRect = QRectF(
                            -m_radius - adjust, -m_radius - adjust,
                            m_radius * 2 + adjust * 2, m_radius * 2 + adjust * 2); 
}

QColor MeaningGraphicsNode::circleColor() const
{
    if (m_color.isValid())
        return m_color;
    return DEFAULT_COLORS[data(POS).toInt() - 1];
}

void MeaningGraphicsNode::setCircleColor(const QColor &color)
{
    m_color = color;
}

void MeaningGraphicsNode::setCircleRadius(int radius)
{
    m_radius = radius;
    calculateBoundingRect();        
}








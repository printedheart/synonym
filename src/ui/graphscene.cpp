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
#include "graphscene.h"
#include "graphedge.h"
#include "graphnode.h"
#include "tglayout.h"
#include "math.h"
                
#include <QtGui>
#include <QtCore>
#include <QtSvg>
                       
#include <QtDebug>

GraphScene::GraphScene(Layout *layout, QObject *parent)
 : QGraphicsScene(parent), m_layout(layout),  m_timerId(0), m_timerInterval(10), 
                  m_enableLayout(true), 
                  m_centralNode(0), m_activeNode(0), m_grabbedNode(0)
{
    m_soundIconRenderer = new QSvgRenderer(QString(":resources/Sound-icon.svg"), this);
    m_layout->setScene(this);
}


GraphScene::~GraphScene()
{
    m_layout->stop();
    delete m_layout;
}

void GraphScene::itemMoved()
{
    if (!m_enableLayout)
        return;
    if (!m_timerId) {
        m_timerId = startTimer(m_timerInterval);
    }  
}

void GraphScene::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    
    if (!m_enableLayout) {
        return;
    }
    layout();
}


void GraphScene::layout()
{
    QList<GraphicsNode*> nodes = graphNodes();
    QList<GraphicsEdge*> edges = graphEdges();
    if (nodes.size() == 0)
        return;
    
    bool needsLayout = m_layout->layout();
    if (!needsLayout) {
        killTimer(m_timerId);
        m_timerId = 0;
        adjustEdges();
     }
   
}  


QList<GraphicsNode *> GraphScene::graphNodes()
{
    if (m_nodes.size() == 0) {
        foreach (QGraphicsItem *item, items()) {
            if (item->type() == GraphicsNode::PhraseType || item->type() == GraphicsNode::MeaningType) {
                GraphicsNode *node = static_cast<GraphicsNode*>(item);
                m_nodes << node;
            }
        }
    }
    return m_nodes;
}

QList<GraphicsEdge*> GraphScene::graphEdges()
{
    if (m_edges.size() == 0) {
        foreach (QGraphicsItem *item, items()) {
            if (GraphicsEdge *edge = qgraphicsitem_cast<GraphicsEdge *>(item))
                m_edges << edge;
        }
    }
    return m_edges;
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_timerInterval = 10;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    if (m_grabbedNode) {
        QGraphicsSvgItem *svgItem = qgraphicsitem_cast<QGraphicsSvgItem*>(m_grabbedNode);
        if (svgItem) {
            emit soundButtonClicked();
            m_grabbedNode = 0;
            return;
        }
        m_layout->startDamper();
        adjustEdges();
        QTimer::singleShot(10, this, SLOT(itemMoved()));
    }
    
    m_grabbedNode = 0;
}


void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{

    QList<QGraphicsItem*> eventItems = items(mouseEvent->scenePos());
    foreach (QGraphicsItem *item, eventItems) {
        QGraphicsSvgItem *svgItem = qgraphicsitem_cast<QGraphicsSvgItem*>(item);
        if (svgItem) {
            //emit soundButtonClicked();
            m_grabbedNode = svgItem;
            return;
        }
         GraphicsNode *node = dynamic_cast<GraphicsNode*>(item);
         if (node) {
              m_layout->stopDamper();
              int nodesCount = graphNodes().size();
              m_timerInterval = qMax(20   , 12 * (nodesCount / 10));
             m_grabbedNode = node;
             itemMoved();
         }
    }   
        
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_layout->resetDamper();
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}


void GraphScene::setLayout(bool enable)
{
    m_enableLayout = enable;
    if (!enable)
        m_layout->stop();
    else 
        m_layout->resetDamper();
}

void GraphScene::signalClickEvent(GraphicsNode *graphNode)
{
    emit nodeClicked(graphNode->id());
}


void GraphScene::setCentralNode(GraphicsNode *node)
{
    m_centralNode = node;
}

GraphicsNode* GraphScene::centralNode() const
{
    return m_centralNode;
}


void GraphScene::displaySoundIcon()
{
    QList<QGraphicsItem*> childItem = m_centralNode->childItems();
    foreach (QGraphicsItem *item, childItem) {
        if (qgraphicsitem_cast<QGraphicsSvgItem*>(item))
            return; // sound icon is already displayed
    }
    
    QGraphicsSvgItem *soundIcon = new QGraphicsSvgItem(m_centralNode);
    soundIcon->setSharedRenderer(m_soundIconRenderer);
    
    QRectF centralNodeRect = m_centralNode->boundingRect();
    soundIcon->scale(0.15, 0.15);
    QPointF soundPos(centralNodeRect.right() - 5,
                     centralNodeRect.top() + 10);
   
    soundIcon->setPos(soundPos);
    soundIcon->setAcceptsHoverEvents(true);
    soundIcon->setCursor(Qt::PointingHandCursor);
}


void GraphScene::setActivated(const QString &id)
{
    if (m_activeNode && m_activeNode->id() == id) {
        return;
    }
    
    if (m_activeNode) {
        m_activeNode->setActivated(false);
    }
    
    m_activeNode = 0;
    QList<GraphicsNode*> nodes = graphNodes();
    foreach (GraphicsNode *node, nodes) {
        if (node->id() == id) {
            node->setActivated(true);
            m_activeNode = node;
            break;
        }
    }
}
    
void GraphScene::signalMouseHovered(GraphicsNode *graphNode)
{
    emit nodeMouseHovered(graphNode->id());
}

void GraphScene::signalMouseHoverLeaved(GraphicsNode *graphNode)
{
    emit nodeMouseHoverLeaved(graphNode->id());
}

void GraphScene::addItem(QGraphicsItem *item)
{
    QGraphicsScene::addItem(item);
    m_nodes.clear();
    m_edges.clear();
}
    
void GraphScene::removeItem(QGraphicsItem *item)
{
    QGraphicsScene::removeItem(item);
    m_nodes.clear();
    m_edges.clear();
}


void GraphScene::adjustEdges()
{
    QList<GraphicsEdge*> edges = graphEdges();
    foreach (GraphicsEdge *edge, edges)
        edge->adjust();
    update(sceneRect());
}





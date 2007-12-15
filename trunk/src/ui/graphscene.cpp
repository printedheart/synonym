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
#include "layout.h"
#include "math.h"
                
#include <QtGui>
#include <QtCore>
#include <QtSvg>
                       
#include <QtDebug>

GraphScene::GraphScene(QObject *parent)
 : QGraphicsScene(parent), m_timerId(0), m_timerInterval(10), 
                  m_enableLayout(true), m_restartLayout(false), m_activeNode(0), m_centralNode(0)
{
    //m_soundIconRenderer = new QSvgRenderer(QString("/home/serega/devel/synonym/src/pics/Sound-icon.svg"), this);
    m_layout = new ForceDirectedLayout();

}


GraphScene::~GraphScene()
{
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
    
    if (!m_enableLayout)
        return;
    layout();
}


void GraphScene::layout()
{
    QList<GraphicsNode*> nodes = graphNodes();
    QList<GraphicsEdge*> edges = graphEdges();
    bool needsLayout = m_layout->layout(nodes, edges, m_restartLayout);
    if (!needsLayout) {
        killTimer(m_timerId);
        m_timerId = 0;
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
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
    m_timerInterval = 10;
    if (m_timerId >= 1) {
        killTimer(m_timerId);
        m_timerId = 0;
    }
    
    itemMoved();
    m_restartLayout = false;
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseGrabberItem()) {
        m_restartLayout = true;
        if (m_timerInterval <= 10) {
            m_timerInterval = 30;
            if (m_timerId) {
                killTimer(m_timerId);
                m_timerId = 0;
                itemMoved();
            }
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void GraphScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    QList<QGraphicsItem*> eventItems = items(mouseEvent->scenePos());
    foreach (QGraphicsItem *item, eventItems) {
        QGraphicsSvgItem *svgItem = qgraphicsitem_cast<QGraphicsSvgItem*>(item);
        if (svgItem) {
            emit soundButtonClicked(m_centralNode->id());
        }
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}


void GraphScene::setLayout(bool enable)
{
    m_enableLayout = enable;
    if (!enable)
        m_layout->stop();
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
    QGraphicsSvgItem *soundIcon = new QGraphicsSvgItem(m_centralNode);
    soundIcon->setSharedRenderer(m_soundIconRenderer);
  //  addItem(soundIcon);
    
    QRectF centralNodeRect = m_centralNode->boundingRect();
    soundIcon->scale(0.15, 0.15);
    QPointF soundPos(centralNodeRect.right() + 10,
                     centralNodeRect.top() + 5);
   
    soundIcon->setPos(soundPos);
    //soundIcon->setAcceptsHoverEvents(true);
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



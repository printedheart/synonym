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
#include "graphscene.h"
#include "graphedge.h"
#include "graphnode.h"
#include "layout.h"
#include "node.h"
                
#include "math.h"
                
#include <QtGui>
#include <QtCore>
                       
#include <QtDebug>
#include <QtSvg>

GraphScene::GraphScene(QObject *parent)
 : QGraphicsScene(parent), m_timerId(0), m_timerInterval(10), m_enableLayout(true), m_restartLayout(false), m_activeNode(0)
{
    m_soundIconRenderer = new QSvgRenderer(QString("/home/serega/devel/synonym/src/pics/Sound-icon.svg"), this);
    m_layout = new ForceDirectedLayout4();

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
    if (!m_enableLayout)
        return;
    Q_UNUSED(event);
    layout();
}


void GraphScene::layout()
{
    QList<GraphicsNode *> nodes;
    QList<GraphEdge *> edges;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == GraphicsNode::PhraseType || item->type() == GraphicsNode::MeaningType) {
            GraphicsNode *node = static_cast<GraphicsNode*>(item);
            nodes << node;
        } else if(GraphEdge *edge = qgraphicsitem_cast<GraphEdge *>(item))
            edges << edge;
    }


    bool needsLayout = m_layout->layout(nodes, edges, m_restartLayout);
    if (!needsLayout) {
        killTimer(m_timerId);
        m_timerId = 0;
    }

}  


QList<GraphicsNode *> GraphScene::graphNodes() const
{
    QList<GraphicsNode*> nodes;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == GraphicsNode::PhraseType || item->type() == GraphicsNode::MeaningType) {
            GraphicsNode *node = static_cast<GraphicsNode*>(item);
            nodes << node;
        }
        
    }
    return nodes;
}

void GraphScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_timerInterval = 10;
    if (m_timerId >= 1) {
        killTimer(m_timerId);
        m_timerId = 0;
        itemMoved();
    }
    
    if (mouseGrabberItem())
        itemMoved();
    m_restartLayout = false;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
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
            emit soundButtonClicked(m_centralNode->dataNode()->id());
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

void GraphScene::propogateClickEvent(GraphicsNode *graphNode)
{
    Node *dataNode = graphNode->dataNode();
    QString id = dataNode->id();
    emit nodeClicked(id);
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
        if (node->dataNode()->id() == id) {
            node->setActivated(true);
            m_activeNode = node;
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



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
        
static const double PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * PI;

GraphScene::GraphScene(QObject *parent)
 : QGraphicsScene(parent), m_timerId(0), m_timerInterval(10), m_calculate(true), m_restartLayout(false)
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
    if (!m_calculate)
        return;
    if (!m_timerId) {
    //    qDebug() << m_timerInterval;
        m_timerId = startTimer(m_timerInterval);
    }
    
}

void GraphScene::timerEvent(QTimerEvent *event)
{
    if (!m_calculate)
        return;
    Q_UNUSED(event);
    calculateForces();
}

void GraphScene::doInitialLayout(GraphNode *rootNode)
{
    rootNode->setPos(0.0, 0.0);
    layoutNodes(rootNode, 0);
}
static int EDGE_DISTANCE = 100;

void GraphScene::layoutNodes(GraphNode *node, GraphNode *parentNode)
{
    QList<GraphNode*> neighbors = node->neighbors();
    
    QList<GraphNode*> children;
    foreach (GraphNode *neighbor, neighbors) {
        if (neighbor != parentNode && !neighbor->visited())
            children.append(neighbor);
    }
    
    double angleIncrement;
    double phi = 0.0;
    if (!parentNode) {
        angleIncrement = TWO_PI / children.size();
        phi = 0.0;
    } else {
        qreal y = node->scenePos().y();
        qreal x = node->scenePos().x();
        qreal angle = atan2(y , x);
        phi =  angle - (PI / 3);
        angleIncrement = (PI / 3) * 2 / (children.size());
        phi += angleIncrement;
    }
    
    foreach (GraphNode *child, children) {
        child->visit();
        
        QPointF childPos(EDGE_DISTANCE * cos(phi), EDGE_DISTANCE * sin(phi));
        childPos += node->scenePos();
        child->setPos(childPos);
        phi += angleIncrement;
    }

    foreach (GraphNode *child, children) {
        layoutNodes(child, node);
    }
}

void GraphScene::calculateForces()
{
    QList<GraphNode *> nodes;
    QList<GraphEdge *> edges;
    foreach (QGraphicsItem *item, items()) {
        if (item->type() == GraphNode::PhraseType || item->type() == GraphNode::MeaningType) {
            GraphNode *node = static_cast<GraphNode*>(item);
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


QList<GraphNode *> GraphScene::graphNodes() const
{
    QList<GraphNode*> nodes;
    foreach (QGraphicsItem *item, items()) {
        if (GraphNode *node = qgraphicsitem_cast<PhraseGraphNode *>(item)) {
            nodes << node;
        } else if (GraphNode *node = qgraphicsitem_cast<MeaningGraphNode *>(item)) {
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
            m_timerInterval = 20;
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


void GraphScene::setCalculate(bool calculate)
{
    m_calculate = calculate;
    if (!calculate)
        m_layout->stop();
}

void GraphScene::propogateClickEvent(PhraseGraphNode *graphNode)
{
    DataNode *dataNode = graphNode->dataNode();
    QString id = dataNode->id();
    emit nodeClicked(id);
}


void GraphScene::setCentralNode(GraphNode *node)
{
    m_centralNode = node;
}

GraphNode* GraphScene::centralNode() const
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




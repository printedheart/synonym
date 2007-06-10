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
#include "../model/node.h"
                
#include "math.h"
                
#include <QtGui>
#include <QtCore>
                       
#include <QtDebug>

        
static const double PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * PI;

GraphScene::GraphScene(QObject *parent)
 : QGraphicsScene(parent), m_timerId(0), m_timerInterval(1), m_calculate(true)
{
}


GraphScene::~GraphScene()
{
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
//         if (GraphNode *node = qgraphicsitem_cast<PhraseGraphNode *>(item)) {
//             node->discardForce();
//             nodes << node;
//         } else if (GraphNode *node = qgraphicsitem_cast<MeaningGraphNode *>(item)) {
//             node->discardForce();
//             nodes << node;
        if (item->type() == GraphNode::PhraseType || item->type() == GraphNode::MeaningType) {
            GraphNode *node = static_cast<GraphNode*>(item);
            node->discardForce();
            nodes << node;
        } else if(GraphEdge *edge = qgraphicsitem_cast<GraphEdge *>(item))
            edges << edge;
    }


 //   qDebug() << "Items " << items().size() << " nodes " << nodes.size() << " edges " << edges.size();
    doSpringAlgorithm(nodes, edges);
    foreach (GraphNode *node, nodes) {
        qreal xvel = node->force().x();
        qreal yvel = node->force().y();
      //  qDebug() << xvel << "  " << yvel;
        if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
            xvel = yvel = 0;

        QPointF newPos = node->pos() + QPointF(xvel, yvel);
        newPos.setX(qMin(qMax(newPos.x(), sceneRect().left() + 10), sceneRect().right() - 10));
        newPos.setY(qMin(qMax(newPos.y(), sceneRect().top() + 10), sceneRect().bottom() - 10));
        node->setNewPos(newPos);
    }
    
    
    
    bool itemsMoved = false;
    foreach (GraphNode *node, nodes) {
        if (mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }

//    update(sceneRect());
   // qDebug() << (itemsMoved ? " moved " : " not moved ");
    if (!itemsMoved) {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}  

void GraphScene::doSpringAlgorithm(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    foreach (GraphNode *aNode, nodes) {
        //if (scene()->mouseGrabbedItem() == aNode)
          //  continue;
        foreach (GraphNode *bNode, nodes) {
            // Find the distance between nodes
            if (aNode == bNode) continue;

        /*    QList<GraphEdge *> aNodeEdges = aNode->edges();
            bool neighbors = false;
            foreach (GraphEdge *edge, aNodeEdges) {
                if (edge->source() == bNode || edge->dest() == bNode) {
                    neighbors = true;
                    break;
                }
            }
            if (neighbors)
                continue;
          */  
            QLineF line(aNode->pos(), bNode->pos());
            qreal dx = line.dx();
            qreal dy = line.dy();
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            if (distance > 0) {
                qreal repulsive =  5000 * aNode->mass() * bNode->mass()  / distance2;
                qreal xvel = repulsive * dx / distance;
                qreal yvel = repulsive * dy / distance;
                //qDebug() << xvel << "  " << yvel;
 
                aNode->applyForce(- xvel, - yvel);
                bNode->applyForce( xvel, yvel);
            }
        }
    }
    
   // foreach (GraphEdge *edge, edges)
     //   edge->calculateForces();
    foreach (GraphEdge *edge, edges) {
        if (!edge->source() || !edge->dest())
            continue;

        QLineF line(edge->source()->mapToScene(0, 0), edge->dest()->mapToScene(0, 0));
        qreal dx = line.dx();
        qreal dy = line.dy();
    
        qreal distance2 = dx * dx + dy * dy;
        qreal distance = sqrt(distance2);
    //qDebug() << "Edge distance " << distance;
        static int REST_DISTANCE = 75;
        qreal force = 0.2 * (distance - REST_DISTANCE);
        
        edge->source()->applyForce(force * (dx / distance), force * ( dy / distance));
        edge->dest()->applyForce(-force * (dx / distance), -force * (dy / distance));
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
    m_timerInterval = 1;
    if (m_timerId >= 100) {
        killTimer(m_timerId);
        m_timerId = 0;
        itemMoved();
    }
    
    if (mouseGrabberItem())
        itemMoved();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void GraphScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseGrabberItem()) {
        if (m_timerInterval < 100) {
            m_timerInterval = 100;
            if (m_timerId) {
                killTimer(m_timerId);
                m_timerId = 0;
                itemMoved();
            }
        }
    }

    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void GraphScene::setCalculate(bool calculate)
{
    m_calculate = calculate;
}



void GraphScene::drawItems(QPainter *painter,
               int numItems,
               QGraphicsItem *items[],
               const QStyleOptionGraphicsItem options[],
                       QWidget *widget) {

    QGraphicsScene::drawItems(painter, numItems, items, options, widget);
}


void GraphScene::propogateClickEvent(PhraseGraphNode *graphNode)
{
    DataNode *dataNode = graphNode->dataNode();
    QString id = dataNode->id();
    emit nodeClicked(id);
}
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
#include "layout.h"
#include "math.h"

#include <QtGui>
#include <QtCore>

ForceDirectedLayout::ForceDirectedLayout()
{}


ForceDirectedLayout::~ForceDirectedLayout()
{}

bool ForceDirectedLayout::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart)
{
    return layout(nodes, edges);
}

bool ForceDirectedLayout::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    foreach (GraphNode *node, nodes) 
        node->discardForce();
    foreach (GraphNode *aNode, nodes) {
        foreach (GraphNode *bNode, nodes) {
            // Find the distance between nodes
            if (aNode == bNode) continue;
         bool haveRepulsion = true;
            QList<GraphNode*> _aNeighbors = aNode->neighbors();
            foreach (GraphNode *n, _aNeighbors) {
                if (n == bNode)
                    haveRepulsion = false;
            }
            if (!haveRepulsion) {
                continue;
            }
            QLineF line(aNode->pos(), bNode->pos());
            qreal dx = line.dx();
            qreal dy = line.dy();
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            if (distance > 0) {
                /**********************/
//                 bool haveCommonNeighbor = false;
//                 QList<GraphNode*> aNeighbors = aNode->neighbors();
//                 QList<GraphNode*> bNeighbors = bNode->neighbors();
// 
//                 foreach (GraphNode *aNeighbor, aNeighbors) {
//                     foreach (GraphNode *bNeighbor, bNeighbors) {
//                         if (aNeighbor == bNeighbor) {
//                             haveCommonNeighbor = true;
//                             break;
//                         }
//                     }
//                 }
// 
//                 /*********************/
//                 qreal repulsive;
//                 if (haveCommonNeighbor)
//                     repulsive = REPULSION * 0.5 * aNode->mass() * bNode->mass() / distance2;
//                 else
//                     repulsive =  REPULSION * 2 * aNode->mass() * bNode->mass()  / distance2;
                qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                qreal xvel = repulsive * dx / distance;
                qreal yvel = repulsive * dy / distance;
                //qDebug() << xvel << "  " << yvel;
 
                aNode->applyForce(- xvel, - yvel);
                bNode->applyForce( xvel, yvel);
            }
        }
    }
    
    foreach (GraphEdge *edge, edges) {
        if (!edge->source() || !edge->dest())
            continue;

      //  QLineF line(edge->source()->mapToScene(0, 0), edge->dest()->mapToScene(0, 0));

        QLineF line(edge->source()->pos(), edge->dest()->pos());
        qreal dx = line.dx();
        qreal dy = line.dy();
    
        qreal distance2 = dx * dx + dy * dy;
        qreal distance = sqrt(distance2);
        if (distance > 0) {
    //qDebug() << "Edge distance " << distance;
            qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
            edge->source()->applyForce(force * (dx / distance), force * ( dy / distance));
            edge->dest()->applyForce(-force * (dx / distance), -force * (dy / distance));
        }
    }


    QGraphicsScene *scene = nodes.first()->scene();
    QRectF sceneRect = scene->sceneRect();
    foreach (GraphNode *node, nodes) {
        qreal xvel = node->force().x();
        qreal yvel = node->force().y();
        //qDebug() << xvel << "  " << yvel;
        if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
            xvel = yvel = 0;

        QPointF newPos = node->pos() + QPointF(xvel/* * 1.5 */, yvel /* * 1.5 */);
        newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
        newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
        node->setNewPos(newPos);
    }


    bool itemsMoved = false;
    foreach (GraphNode *node, nodes) {
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }


    return itemsMoved;
}

/*

ForceDirectedLayout2::ForceDirectedLayout2()
{}  


ForceDirectedLayout2::~ForceDirectedLayout2()
{}

void ForceDirectedLayout2::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    foreach (GraphNode *aNode, nodes) {
        //if (scene()->mouseGrabbedItem() == aNode)
          //  continue;
        foreach (GraphNode *bNode, nodes) {
            // Find the distance between nodes
            if (aNode == bNode) continue;
            QLineF line(aNode->pos(), bNode->pos());
            qreal dx = line.dx();
            qreal dy = line.dy();
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            if (distance > 0) {
                qreal repulsive =  REPULSION * aNode->mass() * bNode->mass()  / distance;
                qreal xvel = repulsive * dx / distance;
                qreal yvel = repulsive * dy / distance;
                //qDebug() << xvel << "  " << yvel;
 
                aNode->applyForce(- xvel, - yvel);
                bNode->applyForce( xvel, yvel);
            }
        }
    }
    

    foreach (GraphEdge *edge, edges) {
        if (!edge->source() || !edge->dest())
            continue;

        QLineF line(edge->source()->mapToScene(0, 0), edge->dest()->mapToScene(0, 0));
        qreal dx = line.dx();
        qreal dy = line.dy();
    
        qreal distance2 = dx * dx + dy * dy;
        qreal distance = sqrt(distance2);
        
    //qDebug() << "Edge distance " << distance;
        qreal spring = STIFFNESS * log(distance / REST_DISTANCE);
        qreal xSpring = spring * dx / distance;
        qreal ySpring = spring * dy / distance;

        edge->source()->applyForce(xSpring, ySpring);
        edge->dest()->applyForce(-xSpring, - ySpring);
    }
}


*/



ForceDirectedLayout3::ForceDirectedLayout3()
{}


ForceDirectedLayout3::~ForceDirectedLayout3()
{}
bool ForceDirectedLayout3::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart)
{
    return layout(nodes, edges);
}
bool ForceDirectedLayout3::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    
    if (nodes.size() == 0) {
        return false;
    }

    QGraphicsScene *scene = nodes.first()->scene();

    QMultiHash<GraphNode*, QPointF> pointMap;
    foreach (GraphNode *node, nodes) {
        pointMap.insert(node, node->pos());
    }
    
    bool done = false;
    
    //For debugging
    QTime t;
    t.start();
    
    while (!done) {
        done = true;

        QHash<GraphNode*, QList<QPointF> >::const_iterator aIter;
        QHash<GraphNode*, QList<QPointF> >::const_iterator bIter;
         
        //for ( aIter = pointMap.constBegin(); aIter != pointMap.constEnd(); ++aIter) {
        foreach (GraphNode *aNode, nodes) {
            //GraphNode *aNode = aIter.key();
            //for (bIter = pointMap.constBegin(); bIter != pointMap.constEnd(); ++bIter) {
            foreach (GraphNode *bNode, nodes) {
//                GraphNode *bNode = bIter.key();
                if (aNode == bNode) continue;
                QLineF line(pointMap.value(aNode), pointMap.value(bNode));
                qreal dx = line.dx();
                qreal dy = line.dy();
                qreal distance2 = dx * dx + dy * dy;
                qreal distance = sqrt(distance2);
                if (distance > 0) {

                    qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                    qreal xvel = repulsive * dx / distance;
                    qreal yvel = repulsive * dy / distance;
                //qDebug() << xvel << "  " << yvel;
 
                    aNode->applyForce(- xvel, - yvel);
                    bNode->applyForce( xvel, yvel);
                }                
            }
        }
        

        foreach (GraphEdge *edge, edges) {
            if (!edge->source() || !edge->dest())
                continue;

            QLineF line(pointMap.value(edge->source()),
                        pointMap.value(edge->dest()));
            qreal dx = line.dx();
            qreal dy = line.dy();
    
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
        
//    qDebug() << "Edge distance " << distance;
           // qreal spring = STIFFNESS * log(distance / REST_DISTANCE);
            if (distance > 0) {
                qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
                edge->source()->applyForce(force * (dx / distance), force * ( dy / distance));
                edge->dest()->applyForce(-force * (dx / distance), -force * (dy / distance));
            }

        }


        QMultiHash<GraphNode*, QPointF >::iterator iter = pointMap.begin();
        for (; iter != pointMap.end(); iter++) {
            GraphNode *node = iter.key();
    
            qreal xvel = node->force().x();
            qreal yvel = node->force().y();
            if (qAbs(xvel) < 1.0 && qAbs(yvel) < 1.0)
                xvel = yvel = 0;

           QPointF previous = pointMap.value(node);
           QPointF newPos = previous + QPointF(xvel/* * 1.5 */, yvel /* * 1.5 */);
          // newPos.setX(qMin(qMax(newPos.x(), scene->sceneRect().left() + 10), scene->sceneRect().right() - 10));
          // newPos.setY(qMin(qMax(newPos.y(), scene->sceneRect().top() + 10), scene->sceneRect().bottom() - 10));
    
            if (previous != newPos)
                done = false;
            
            pointMap.insert(node, newPos);
            node->discardForce();

        }
        break;

    }

    qDebug("Time elapsed: %d ms", t.elapsed());
        
 //   int pointCount = pointMap[nodes.first()].size();
   // qDebug("Point count: %d", pointCount);

    
    foreach (GraphNode *node, nodes) {
        node->setNewPos(pointMap.value(node));
    }
    
//     for (int i = 0; i < pointCount - 1; i++) {
//         for (iter = pointMap.begin(); iter != pointMap.end(); iter++) {
//             
//             QPointF p = iter.value().takeFirst();
//             iter.key()->setPos(p);
//         }
//         scene->update(scene->sceneRect());
//     }

//     return true;

    bool itemsMoved = false;
    foreach (GraphNode *node, nodes) {
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }


    return itemsMoved;
}





ForceDirectedLayout4::ForceDirectedLayout4()
{
    m_abort = false;
    
    connect (this, SIGNAL(finished()), this, SLOT (wakeUp()));
}

void ForceDirectedLayout4::wakeUp()
{
    m_aborted.wakeAll();
}


ForceDirectedLayout4::~ForceDirectedLayout4()
{}

bool ForceDirectedLayout4::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart) 
{
    if (restart) {
        m_abort = true;
        if (isRunning()) {
            return true;
        }
        return _layout(nodes, edges);
    }
    m_abort = false;
    return layout(nodes, edges);
}

void ForceDirectedLayout4::stop()
{
    m_abort = true;
}
    
    

bool ForceDirectedLayout4::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    //qDebug() << "l";
    if (nodes.size() == 0) {
        return false;
    }
    QGraphicsScene *scene = nodes.first()->scene();
        
    if (nodes.first()->animationPosCount() == 0) {
        m_mutex.lock();
        if (m_nodes.size() != nodes.size() || m_edges.size() != edges.size()) {
            m_nodes = nodes;
            m_edges = edges;
        }
        
        foreach (GraphNode *node, m_nodes) 
            node->addAnimationPos(node->pos());
        
        finishedLayout = false;
        m_mutex.unlock();
        start(QThread::LowestPriority);
        
    }
    
    
    if (m_nodes.first()->animationPosCount() < 5 && !finishedLayout)
        return true;

    m_mutex.lock();
    int nodesCount = m_nodes.size();
    int nodesToTake = nodesCount / 20 > 0 ? nodesCount / 20 : 1;
    for (int i = 0; i < nodesCount; i++) {
        GraphNode *node = m_nodes.at(i);
        if (node->animationPosCount() > nodesToTake) {
            for (int n = 0; n < nodesToTake; n++)
                node->takeFirstAnimationPos();
            node->setNewPos(node->takeFirstAnimationPos());
        } else if (node->animationPosCount() > 0)
            node->setNewPos(node->takeFirstAnimationPos());
    }
    m_mutex.unlock();
    
    bool itemsMoved = false;
    for (int i = 0; i < nodesCount; i++) {
        GraphNode *node = m_nodes.at(i);
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }

    return itemsMoved || nodes.first()->animationPosCount() > 0 ||  isRunning();
}


 
void ForceDirectedLayout4::run()
{
    QGraphicsScene *scene = m_nodes.first()->scene();
    bool done = false;
    int count = 0;
    int nodesCount = m_nodes.size();
    while (!done) {
        done = true;
        finishedLayout = false;
        count++;
        
        for (int a = 0; a < nodesCount; a++) {   
            GraphNode *aNode = m_nodes.at(a);
            if (m_abort) {
                for (int i = 0; i < nodesCount; i++)
                    m_nodes.at(i)->resetAnimation();
                return;
            }
            
            for (int b = 0; b < nodesCount; b++) {
                GraphNode *bNode = m_nodes.at(b);
                if (aNode == bNode) continue;
                QLineF line(aNode->lastAnimationPos(), bNode->lastAnimationPos());
                qreal dx = line.dx();
                qreal dy = line.dy();
                qreal distance2 = dx * dx + dy * dy;
                qreal distance = sqrt(distance2);
                if (distance > 0) {

                    qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                    qreal xvel = repulsive * dx / distance;
                    qreal yvel = repulsive * dy / distance;
 
                    aNode->applyForce(- xvel, - yvel);
                    bNode->applyForce( xvel, yvel);
                }
            }
        }
        

        foreach (GraphEdge *edge, m_edges) {
            GraphNode *source = edge->source();
            GraphNode *dest = edge->dest();
            if (!source || !dest)
                continue;

            QLineF line(source->lastAnimationPos(), dest->lastAnimationPos());
            qreal dx = line.dx();
            qreal dy = line.dy();
    
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            
            if (distance > 0) {
                qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
                source->applyForce(force * (dx / distance), force * ( dy / distance));
                dest->applyForce(-force * (dx / distance), -force * (dy / distance));
            }

        }


        for (int i = 0; i < nodesCount; i++) {
            GraphNode *node = m_nodes.at(i);
            if (!node->flags().testFlag(QGraphicsItem::ItemIsMovable)) {
                node->addAnimationPos(node->pos());
                continue;
            }
             
            qreal xvel = node->force().x();
            qreal yvel = node->force().y();
            if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
                xvel = yvel = 0;

            QPointF previous = node->lastAnimationPos();
            QPointF newPos = previous + QPointF(xvel, yvel);
            
            newPos.setX(qMin(qMax(newPos.x(), scene->sceneRect().left() + 10), scene->sceneRect().right() - 10));
            newPos.setY(qMin(qMax(newPos.y(), scene->sceneRect().top() + 10), scene->sceneRect().bottom() - 10));
            
            if (previous != newPos)
                done = false;
            
            node->addAnimationPos(newPos);
            node->discardForce();
        }
        
        
        if (!firstRemoved && m_nodes.first()->animationPosCount() == 2 && !done) {
            m_mutex.lock();
            for (int i = 0; i < nodesCount; i++) {
                GraphNode *node = m_nodes.at(i);
                node->takeFirstAnimationPos();
            }
            firstRemoved = true;         
            m_mutex.unlock();    
        }
        
    }
    
    if (!firstRemoved) {
        m_mutex.lock();
        for (int i = 0; i < nodesCount; i++) {
            GraphNode *node = m_nodes.at(i);
            node->takeFirstAnimationPos();
        }
        m_mutex.unlock();    
    }
    finishedLayout = true;
}


bool ForceDirectedLayout4::_layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    //qDebug() << "_l";
    QList<GraphNode*>::const_iterator aIter;
    QList<GraphNode*>::const_iterator bIter;
    
    for (aIter = nodes.constBegin(); aIter != nodes.constEnd(); aIter++) {
        (*aIter)->discardForce();
        (*aIter)->resetAnimation();
    }
    for (aIter = nodes.constBegin(); aIter != nodes.constEnd(); aIter++) {
        GraphNode *aNode = *aIter;
        for (bIter = nodes.constBegin(); bIter != nodes.constEnd(); bIter++) {
            GraphNode *bNode = *bIter;
            
            // Find the distance between nodes
            if (aNode == bNode) continue;
            QLineF line(aNode->pos(), bNode->pos());
            qreal dx = line.dx();
            qreal dy = line.dy();
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            if (distance > 0) {
                qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                qreal xvel = repulsive * dx / distance;
                qreal yvel = repulsive * dy / distance;
                aNode->applyForce(- xvel, - yvel);
                bNode->applyForce( xvel, yvel);
            }
        }
    }
    
    foreach (GraphEdge *edge, edges) {
        if (!edge->source() || !edge->dest())
            continue;

        QLineF line(edge->source()->pos(), edge->dest()->pos());
        qreal dx = line.dx();
        qreal dy = line.dy();
    
        qreal distance2 = dx * dx + dy * dy;
        qreal distance = sqrt(distance2);
        if (distance > 0) {
            qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
            edge->source()->applyForce(force * (dx / distance), force * ( dy / distance));
            edge->dest()->applyForce(-force * (dx / distance), -force * (dy / distance));
        }
    }
        
    QGraphicsScene *scene = nodes.first()->scene();
    QRectF sceneRect = scene->sceneRect();
    QList<GraphNode*>::const_iterator iter;
    for (iter = nodes.constBegin(); iter != nodes.constEnd(); iter++) {
        GraphNode *node = *iter;
        qreal xvel = node->force().x();
        qreal yvel = node->force().y();
        if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
            xvel = yvel = 0;

        QPointF newPos = node->pos() + QPointF(xvel, yvel);
        newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
        newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
        node->setNewPos(newPos);
    }

    bool itemsMoved = false;
    for (iter = nodes.constBegin(); iter != nodes.constEnd(); iter++) {    
        if (scene->mouseGrabberItem() != *iter)
            if ((*iter)->advance())
                itemsMoved = true;
    }

    return itemsMoved;
        
}


















ForceDirectedLayout5::ForceDirectedLayout5(QObject *parent)
    :QThread(parent)
{
    connect(this, SIGNAL(finished()), this, SLOT(wakeUp()));
    m_abort = false;
    m_hasPoints = false;
}

ForceDirectedLayout5::~ForceDirectedLayout5()
{
}



bool ForceDirectedLayout5::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart) 
{
 /*   if (restart) {
    m_mutex.lock();
    m_abort = true;
    if (isRunning()) {
    Q_ASSERT(m_iterationTime != -1);
    m_aborted.wait(&m_mutex, m_iterationTime == -1 ? 5 : m_iterationTime + 5);
}
    Q_ASSERT(!isRunning());
    foreach (GraphNode *node, nodes) 
    node->resetAnimationPoints();
        
    m_abort = false;
    m_mutex.unlock();
}
 */ 
    return layout(nodes, edges);
}

void ForceDirectedLayout5::wakeUp()
{
    m_finished.wakeAll();
}
    
    

bool ForceDirectedLayout5::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    if (nodes.size() == 0) {
        return false;
    }
    QGraphicsScene *scene = nodes.first()->scene();
    
    // If hash map does not have the first node, means all nodes are different.
    GraphNode *firstNode = nodes.first();
    if (!m_points.contains(firstNode)) {
        m_mutex.lock();
        if (isRunning()) {
            m_finished.wait(&m_mutex);
        }
        
    
        m_nodes = nodes;
        m_edges = edges;
        m_points.clear();
        
        foreach (GraphNode *node, m_nodes) {
            QLinkedList<QPointF> list;
            list << node->pos();
            m_points[node] = list;
        }
                
        
        finishedLayout = false;
        m_mutex.unlock();
        start();
    }
    
     
    m_mutex.lock();
    if (m_points.constBegin().value().count() < 20 && !finishedLayout)
        m_enoughPoints.wait(&m_mutex);
    m_mutex.unlock();
    
    QHash<GraphNode*, QLinkedList<QPointF> >::iterator iter;
    for (iter = m_points.begin(); iter != m_points.end(); ++iter) {
        if (iter.value().count() > 0) {
            iter.key()->setNewPos(iter.value().takeFirst());
        }
    }
    
    QHash<GraphNode*, QLinkedList<QPointF> >::const_iterator const_iter;
    bool itemsMoved = false;
    for (const_iter = m_points.constBegin(); const_iter != m_points.constEnd(); ++const_iter) {
        GraphNode *node = const_iter.key();
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }

    
    return itemsMoved ||  isRunning();
}



void ForceDirectedLayout5::run()
{
    qDebug() << "run() start";
    QTime t;
    t.start();
    QGraphicsScene *scene = m_nodes.first()->scene();
    bool done = false;
    int count = 0;
    
    while (!done) {
        done = true;
        finishedLayout = false;
        count++;
        
        QHash<GraphNode*, QLinkedList<QPointF> >::const_iterator aIter;
        QHash<GraphNode*, QLinkedList<QPointF> >::const_iterator bIter;
        for (aIter = m_points.constBegin(); aIter != m_points.constEnd(); ++aIter) {    
        GraphNode *aNode = aIter.key();
            if (m_abort) {
                m_aborted.wakeAll();
                return;
            }
            
            for (bIter = m_points.constBegin(); bIter != m_points.constEnd(); ++bIter) {    
                GraphNode *bNode = bIter.key();
                if (aNode == bNode) continue;
                QLineF line(aIter.value().last(), bIter.value().last());
                qreal dx = line.dx();
                qreal dy = line.dy();
                qreal distance2 = dx * dx + dy * dy;
                qreal distance = sqrt(distance2);
                if (distance > 0) {

                    qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                    qreal xvel = repulsive * dx / distance;
                    qreal yvel = repulsive * dy / distance;
                        //qDebug() << xvel << "  " << yvel;
 
                    aNode->applyForce(- xvel, - yvel);
                    bNode->applyForce( xvel, yvel);
                }
            }
        }
        

        foreach (GraphEdge *edge, m_edges) {
            GraphNode *source = edge->source();
            GraphNode *dest = edge->dest();
            if (!source || !dest)
                continue;

            QLineF line(m_points[source].last(), m_points[dest].last());
            qreal dx = line.dx();
            qreal dy = line.dy();
    
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            
            if (distance > 0) {
                qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
                source->applyForce(force * (dx / distance), force * ( dy / distance));
                dest->applyForce(-force * (dx / distance), -force * (dy / distance));
            }

        }


        QHash<GraphNode*, QLinkedList<QPointF> >::iterator i;
        for (i = m_points.begin(); i != m_points.end(); i++) {
            GraphNode *node = i.key();
            if (!node->flags().testFlag(QGraphicsItem::ItemIsMovable)) {
                i.value().append(node->pos());
                continue;
            }
             
            qreal xvel = node->force().x();
            qreal yvel = node->force().y();
            if (qAbs(xvel) < 0.01 && qAbs(yvel) < 0.01)
                xvel = yvel = 0;

            QPointF previous = i.value().last();
            QPointF newPos = previous + QPointF(xvel, yvel);
            
            newPos.setX(qMin(qMax(newPos.x(), scene->sceneRect().left() + 10), scene->sceneRect().right() - 10));
            newPos.setY(qMin(qMax(newPos.y(), scene->sceneRect().top() + 10), scene->sceneRect().bottom() - 10));
            
            if (previous != newPos)
                done = false;
            
            i.value().append(newPos);
            node->discardForce();
        }
        
        
        if (count == 2 && !done && !firstRemoved) {
            m_mutex.lock();
            for (i = m_points.begin(); i != m_points.end(); ++i) {
                i.value().takeFirst();
            }
            firstRemoved = true;         
            m_mutex.unlock();    
        }
        
        if (m_points.begin().value().count() == 20)
            m_enoughPoints.wakeAll();
         
    }
    
    if (!firstRemoved) {
        m_mutex.lock();
        QHash<GraphNode*, QLinkedList<QPointF> >::iterator i;
        for (i = m_points.begin(); i != m_points.end(); ++i) {
          //  Q_ASSERT(node->pointCount() > 0);
            i.value().takeFirst();    
        }
        m_mutex.unlock();    
    }
    finishedLayout = true;
    qDebug() << "end";
    qDebug("Time elapsed: %d ms", t.elapsed());
    qDebug() << count;
    
    m_enoughPoints.wakeAll();
}













ForceDirectedLayout6::ForceDirectedLayout6(QObject *parent)
    :QThread(parent)
{
    connect(this, SIGNAL(finished()), this, SLOT(wakeUp()));
    m_abort = false;
    m_hasPoints = false;
}

ForceDirectedLayout6::~ForceDirectedLayout6()
{
}



bool ForceDirectedLayout6::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart) 
{
 /*   if (restart) {
    m_mutex.lock();
    m_abort = true;
    if (isRunning()) {
    Q_ASSERT(m_iterationTime != -1);
    m_aborted.wait(&m_mutex, m_iterationTime == -1 ? 5 : m_iterationTime + 5);
}
    Q_ASSERT(!isRunning());
    foreach (GraphNode *node, nodes) 
    node->resetAnimationPoints();
        
    m_abort = false;
    m_mutex.unlock();
}
 */ 
    return layout(nodes, edges);
}

void ForceDirectedLayout6::wakeUp()
{
    m_finished.wakeAll();
}
        

bool ForceDirectedLayout6::layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges)
{
    if (nodes.size() == 0) {
        return false;
    }
    QGraphicsScene *scene = nodes.first()->scene();
    
    if (nodes.first()->layoutPath().pointCount() == 0) {
        m_mutex.lock();
        if (m_nodes.size() != nodes.size() ||  m_edges.size() != edges.size()) {
        
            if (isRunning()) {
                m_finished.wait(&m_mutex);
            }
        
            m_nodes = nodes;
            m_edges = edges;
            
            foreach (GraphNode *node, m_nodes) {
                node->layoutPath().reset();
                node->layoutPath().addPoint(node->pos());
            }
            
            finishedLayout = false;
            m_mutex.unlock();
            start();
        }
    }
    
     
    m_mutex.lock();
    if (m_nodes.first()->layoutPath().pointCount() < 20 && !finishedLayout)
        m_enoughPoints.wait(&m_mutex);
    m_mutex.unlock();
    
    
    foreach (GraphNode *node, m_nodes) {
        if (node->layoutPath().pointCount() > 0)
            node->setNewPos(node->layoutPath().takeFirstPoint());
    }
    
    bool itemsMoved = false;
    foreach (GraphNode *node, m_nodes) {
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }

    
    return itemsMoved ||  isRunning();
}



void ForceDirectedLayout6::run()
{
    qDebug() << "run() start";
    QTime t;
    t.start();
    QGraphicsScene *scene = m_nodes.first()->scene();
    bool done = false;
    int count = 0;
    
    while (!done) {
        done = true;
        finishedLayout = false;
        count++;
        
        QList<GraphNode*>::iterator aIter;
        QList<GraphNode*>::iterator bIter;
        for (aIter = m_nodes.begin(); aIter != m_nodes.end(); ++aIter) {    
            GraphNode *aNode = (*aIter);
            if (m_abort) {
                m_aborted.wakeAll();
                return;
            }
            
            for (bIter = m_nodes.begin(); bIter != m_nodes.end(); ++bIter) {    
                GraphNode *bNode = (*bIter);
                if (aNode == bNode) continue;
                QLineF line(aNode->layoutPath().lastPoint(), bNode->layoutPath().lastPoint());
                qreal dx = line.dx();
                qreal dy = line.dy();
                qreal distance2 = dx * dx + dy * dy;
                qreal distance = sqrt(distance2);
                if (distance > 0) {

                    qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                    qreal xvel = repulsive * dx / distance;
                    qreal yvel = repulsive * dy / distance;
                        //qDebug() << xvel << "  " << yvel;
 
                    aNode->applyForce(- xvel, - yvel);
                    bNode->applyForce( xvel, yvel);
                }
            }
        }
        

        foreach (GraphEdge *edge, m_edges) {
            GraphNode *source = edge->source();
            GraphNode *dest = edge->dest();
            if (!source || !dest)
                continue;

            QLineF line(source->layoutPath().lastPoint(), dest->layoutPath().lastPoint());
            qreal dx = line.dx();
            qreal dy = line.dy();
    
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            
            if (distance > 0) {
                qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
                source->applyForce(force * (dx / distance), force * ( dy / distance));
                dest->applyForce(-force * (dx / distance), -force * (dy / distance));
            }

        }


        foreach (GraphNode *node, m_nodes) {
            if (!node->flags().testFlag(QGraphicsItem::ItemIsMovable)) {
                node->layoutPath().addPoint(node->pos());
                continue;
            }
             
            qreal xvel = node->force().x();
            qreal yvel = node->force().y();
            if (qAbs(xvel) < 0.01 && qAbs(yvel) < 0.01)
                xvel = yvel = 0;

            QPointF previous = node->layoutPath().lastPoint();
            QPointF newPos = previous + QPointF(xvel, yvel);
            
            newPos.setX(qMin(qMax(newPos.x(), scene->sceneRect().left() + 10), scene->sceneRect().right() - 10));
            newPos.setY(qMin(qMax(newPos.y(), scene->sceneRect().top() + 10), scene->sceneRect().bottom() - 10));
            
            if (previous != newPos)
                done = false;
            
            node->layoutPath().addPoint(newPos);
            node->discardForce();
        }
        
        
        if (count == 2 && !done && !firstRemoved) {
            m_mutex.lock();
            foreach (GraphNode *node, m_nodes) {
                node->layoutPath().takeFirstPoint();
            }
            firstRemoved = true;         
            m_mutex.unlock();    
        }
        
        if (m_nodes.first()->layoutPath().pointCount() == 25)
            m_enoughPoints.wakeAll();
         
    }
    
    if (!firstRemoved) {
        m_mutex.lock();
        foreach (GraphNode *node, m_nodes) {
            node->layoutPath().takeFirstPoint();
        }
        m_mutex.unlock();    
    }
    finishedLayout = true;
    qDebug() << "end";
    qDebug("Time elapsed: %d ms", t.elapsed());
    qDebug() << count;
    
    m_enoughPoints.wakeAll();
}















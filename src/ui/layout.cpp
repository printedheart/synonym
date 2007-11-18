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
#include "graphscene.h"
#include "math.h"
#include "graphalgorithms.h"
#include <QtGui>
#include <QtCore>

#if(0)
ForceDirectedLayout::ForceDirectedLayout()
{}


ForceDirectedLayout::~ForceDirectedLayout()
{}

bool ForceDirectedLayout::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart)
{
    return layout(nodes, edges);
}

bool ForceDirectedLayout::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges)
{
    foreach (GraphicsNode *node, nodes) 
        node->discardForce();
    foreach (GraphicsNode *aNode, nodes) {
        foreach (GraphicsNode *bNode, nodes) {
            // Find the distance between nodes
            if (aNode == bNode) continue;
         bool haveRepulsion = true;
            QList<GraphicsNode*> _aNeighbors = aNode->neighbors();
            foreach (GraphicsNode *n, _aNeighbors) {
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
//                 QList<GraphicsNode*> aNeighbors = aNode->neighbors();
//                 QList<GraphicsNode*> bNeighbors = bNode->neighbors();
// 
//                 foreach (GraphicsNode *aNeighbor, aNeighbors) {
//                     foreach (GraphicsNode *bNeighbor, bNeighbors) {
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
    
    foreach (GraphicsEdge *edge, edges) {
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
    foreach (GraphicsNode *node, nodes) {
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
    foreach (GraphicsNode *node, nodes) {
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

void ForceDirectedLayout2::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges)
{
    foreach (GraphicsNode *aNode, nodes) {
        //if (scene()->mouseGrabbedItem() == aNode)
          //  continue;
        foreach (GraphicsNode *bNode, nodes) {
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
    

    foreach (GraphicsEdge *edge, edges) {
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
bool ForceDirectedLayout3::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart)
{
    return layout(nodes, edges);
}
bool ForceDirectedLayout3::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges)
{
    
    if (nodes.size() == 0) {
        return false;
    }

    QGraphicsScene *scene = nodes.first()->scene();

    QMultiHash<GraphicsNode*, QPointF> pointMap;
    foreach (GraphicsNode *node, nodes) {
        pointMap.insert(node, node->pos());
    }
    
    bool done = false;
    
    //For debugging
    QTime t;
    t.start();
    
    while (!done) {
        done = true;

        QHash<GraphicsNode*, QList<QPointF> >::const_iterator aIter;
        QHash<GraphicsNode*, QList<QPointF> >::const_iterator bIter;
         
        //for ( aIter = pointMap.constBegin(); aIter != pointMap.constEnd(); ++aIter) {
        foreach (GraphicsNode *aNode, nodes) {
            //GraphicsNode *aNode = aIter.key();
            //for (bIter = pointMap.constBegin(); bIter != pointMap.constEnd(); ++bIter) {
            foreach (GraphicsNode *bNode, nodes) {
//                GraphicsNode *bNode = bIter.key();
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
        

        foreach (GraphicsEdge *edge, edges) {
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


        QMultiHash<GraphicsNode*, QPointF >::iterator iter = pointMap.begin();
        for (; iter != pointMap.end(); iter++) {
            GraphicsNode *node = iter.key();
    
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

    
    foreach (GraphicsNode *node, nodes) {
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
    foreach (GraphicsNode *node, nodes) {
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }


    return itemsMoved;
}


#endif


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



void ForceDirectedLayout4::prepareLayout(GraphicsNode *rootNode)
{
    qDebug() << "prepareLayout()" << rootNode->id();
    rootNode->setPos(0.0, 0.0);
    QSet<GraphicsNode*> visitSet;
    layoutNodes(rootNode, 0, visitSet); 
}

void ForceDirectedLayout4::layoutNodes(GraphicsNode *node, GraphicsNode *parentNode,
                                          QSet<GraphicsNode*> &visitSet)
{
    QSet<GraphicsEdge*> edges = node->edges();
    QSet<GraphicsNode*> neighbors = node->neighbors();
    
    QList<GraphicsNode*> children;
    foreach (GraphicsNode *neighbor, neighbors) {
        if (neighbor != parentNode && !visitSet.contains(neighbor) && neighbor->scene())
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
    
    foreach (GraphicsNode *child, children) {
        visitSet << child;
        
        QPointF childPos(REST_DISTANCE * cos(phi), REST_DISTANCE * sin(phi));
        childPos += node->scenePos();
        child->setPos(childPos);
        phi += angleIncrement;
    }

    foreach (GraphicsNode *child, children) {
        layoutNodes(child, node, visitSet);
    } 
}


bool ForceDirectedLayout4::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart) 
{
    if (restart) {
        m_abort = true;
        if (isRunning()) {
            return true;
        }
        return layoutSerial(nodes, edges);
    }
    m_abort = false;
    return layout(nodes, edges);
}

void ForceDirectedLayout4::stop()
{
    m_abort = true;
}
    
    

bool ForceDirectedLayout4::layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges)
{
    if (nodes.size() == 0) {
        return false;
    }
    QGraphicsScene *scene = nodes.first()->scene();
   
    if (m_animations.empty() || m_animations.constBegin()->pointCount() == 0) { 
        m_mutex.lock();
        Q_ASSERT(!isRunning());
        if (m_nodes.size() != nodes.size() || m_edges.size() != edges.size()) {
            m_nodes = nodes;
            m_edges = edges;
            GraphicsNode *rootNode = qobject_cast<GraphScene*>(scene)->centralNode();
            prepareLayout(rootNode);
        }
        m_animations.clear();
        foreach (GraphicsNode *node, m_nodes)
            m_animations[node].addPoint(node->pos());
        
        finishedLayout = false;
        m_mutex.unlock();
        start(QThread::LowestPriority);
        
        //  We want to get some points as soon as possible
        QThread::currentThread()->usleep(10000);
    }
    
    if (m_animations.constBegin()->pointCount() < 10 && !finishedLayout)
        return true;
    
    m_mutex.lock();
    int nodesCount = m_nodes.size();
    int nodesToTake = nodesCount / 20 > 0 ? nodesCount / 20 : 1;
    for (int i = 0; i < nodesCount; i++) {
        GraphicsNode *node = m_nodes.at(i);
        NodeAnimation & animation = m_animations[node];
        if (animation.pointCount() > nodesToTake) {
            for (int n = 0; n < nodesToTake; n++) {
                animation.takeFirstPoint();
            }
            node->setNewPos(animation.takeFirstPoint());
        } else if (animation.pointCount() > 0) {
            node->setNewPos(animation.takeFirstPoint());
        }
    }
    m_mutex.unlock();
    
    bool itemsMoved = false;
    for (int i = 0; i < nodesCount; i++) {
        GraphicsNode *node = m_nodes.at(i);
        if (scene->mouseGrabberItem() != node)
            if (node->advance())
                itemsMoved = true;
    }

    return itemsMoved || m_animations.constBegin()->pointCount() > 0 ||  isRunning();
    
}

void ForceDirectedLayout4::run()
{
    typedef QHash<GraphicsNode*, NodeAnimation>::iterator AnimationIterator;
    qDebug() << "run()";
    QGraphicsScene *scene = m_nodes.first()->scene();
    bool done = false;
    int count = 0;
    int nodesCount = m_nodes.size();
    while (!done) {
        done = true;
        finishedLayout = false;
        count++;
        
        AnimationIterator aIter;
        AnimationIterator end = m_animations.end();
        for (aIter = m_animations.begin(); aIter != end; ++aIter) {
            GraphicsNode *aNode = aIter.key();
            
            if (m_abort) {
                foreach (NodeAnimation animation, m_animations) {
                    animation.reset();
                }
                return;
            }
            AnimationIterator bIter;
            for (bIter = m_animations.begin(); bIter != end; ++bIter) {
                GraphicsNode *bNode = bIter.key();
                if (aNode == bNode) continue;
                
                QLineF line(aIter.value().lastPoint(), bIter.value().lastPoint());
                qreal dx = line.dx();
                qreal dy = line.dy();
                qreal distance2 = dx * dx + dy * dy;
                qreal distance = sqrt(distance2);
                if (distance > 0) {

                    qreal repulsive = REPULSION * aNode->mass() * bNode->mass() / distance2;
                    qreal xvel = repulsive * dx / distance;
                    qreal yvel = repulsive * dy / distance;
 
                    aIter.value().force += QPointF(-xvel, -yvel);
                    bIter.value().force += QPointF(xvel, yvel); 
                }
            }
        }
        

        foreach (GraphicsEdge *edge, m_edges) {
            GraphicsNode *source = edge->source();
            GraphicsNode *dest = edge->dest();
            if (!source || !dest)
                continue;

            NodeAnimation &sourceAnimation = m_animations[source];
            NodeAnimation &destAnimation = m_animations[dest];
            
            QLineF line(sourceAnimation.lastPoint(), destAnimation.lastPoint());
            qreal dx = line.dx();
            qreal dy = line.dy();
    
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            
            if (distance > 0) {
                qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
                sourceAnimation.force += QPointF(force * (dx / distance), force * ( dy / distance));
                destAnimation.force += QPointF(-force * (dx / distance), -force * (dy / distance));
            }

        }


        AnimationIterator iter;
        for (iter = m_animations.begin(); iter != m_animations.end(); ++iter) {
            GraphicsNode *node = iter.key();
            if (!node->flags().testFlag(QGraphicsItem::ItemIsMovable)) {
                m_animations[node].addPoint(node->pos());
                continue;
            }
             
            NodeAnimation &animation = m_animations[node];
            QPointF resultForce = m_animations[node].force;
            qreal xvel = animation.force.x();
            qreal yvel = animation.force.y();
            if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
                xvel = yvel = 0;

            QPointF previous = animation.lastPoint();
            QPointF newPos = previous + QPointF(xvel, yvel);
            
            newPos.setX(qMin(qMax(newPos.x(), scene->sceneRect().left() + 10), scene->sceneRect().right() - 10));
            newPos.setY(qMin(qMax(newPos.y(), scene->sceneRect().top() + 10), scene->sceneRect().bottom() - 10));
            
            if (previous != newPos)
                done = false;
            
            animation.addPoint(newPos);
            animation.force.setX(0.0);
            animation.force.setY(0.0);
        }
        
        if (!firstRemoved && m_animations.constBegin()->pointCount() == 2 && !done) {
            m_mutex.lock();
            foreach (NodeAnimation animation, m_animations) {
                animation.takeFirstPoint();
            }
            firstRemoved = true;         
            m_mutex.unlock();    
        }
        
    }
    
    if (!firstRemoved) {
        m_mutex.lock();
        foreach (NodeAnimation animation, m_animations) {
            animation.takeFirstPoint();
        }
        m_mutex.unlock();    
    }
    finishedLayout = true;
    qDebug() << "finished run";
}


bool ForceDirectedLayout4::layoutSerial(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges)
{
    QHash<GraphicsNode*, QPointF> resultForces;
    QList<GraphicsNode*>::const_iterator aIter;
    QList<GraphicsNode*>::const_iterator bIter;
    
    for (aIter = nodes.constBegin(); aIter != nodes.constEnd(); aIter++) {
        GraphicsNode *aNode = *aIter;
        for (bIter = nodes.constBegin(); bIter != nodes.constEnd(); bIter++) {
            GraphicsNode *bNode = *bIter;
            
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
                resultForces[aNode] += QPointF(-xvel, -yvel);
                resultForces[bNode] += QPointF(xvel, yvel);
            }
        }
    }
    
    foreach (GraphicsEdge *edge, edges) {
        if (!edge->source() || !edge->dest())
            continue;

        QLineF line(edge->source()->pos(), edge->dest()->pos());
        qreal dx = line.dx();
        qreal dy = line.dy();
    
        qreal distance2 = dx * dx + dy * dy;
        qreal distance = sqrt(distance2);
        if (distance > 0) {
            qreal force = -(STIFFNESS * (REST_DISTANCE - distance));
        
            resultForces[edge->source()] += QPointF(force * (dx / distance), force * ( dy / distance));
            resultForces[edge->dest()] += QPointF(-force * (dx / distance), -force * (dy / distance));
        }
    }
        
    QGraphicsScene *scene = nodes.first()->scene();
    QRectF sceneRect = scene->sceneRect();
    QList<GraphicsNode*>::const_iterator iter;
    for (iter = nodes.constBegin(); iter != nodes.constEnd(); iter++) {
        GraphicsNode *node = *iter;
        qreal xvel = resultForces[node].x();
        qreal yvel = resultForces[node].y();
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














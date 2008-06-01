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
#include "layout.h"
#include "graphscene.h"
#include "math.h"
#include "graphalgorithms.h"
#include <QtGui>
#include <QtCore>

static const double PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * PI;


ForceDirectedLayout::ForceDirectedLayout(QObject *parent)
    :QThread(parent)
{
    centralNode = 0;
    m_restDistance = 100;
    connect (this, SIGNAL(finished()), this, SLOT (wakeUp()));
}

void ForceDirectedLayout::wakeUp()
{
    m_aborted.wakeAll();
}


ForceDirectedLayout::~ForceDirectedLayout()
{}

void ForceDirectedLayout::stop()
{
    m_state = ABORT_LOOP;
}

// Sort of a hack. When algorithm has run through all
// nodes at least once, then no nodes should have a null point.
static bool needsPreLayout(GraphicsNode *rootNode)
{
    foreach (GraphicsNode *node, rootNode->neighbors()) {
        if (node->pos() == QPointF()) 
            return true;
    }
    return false;
}

void ForceDirectedLayout::layoutNodes(GraphicsNode *node,
                        QSet<GraphicsNode*> &visitSet)
{
    QSet<GraphicsNode*> neighbors = 
            node == node->graphScene()->centralNode() ? node->neighbors() : node->outNeighbors();
    
    QList<GraphicsNode*> children;
    foreach (GraphicsNode *neighbor, neighbors) {
        if (!visitSet.contains(neighbor) && neighbor->scene())
            children.append(neighbor);
    }
    
    double angleIncrement;
    double phi = 0.0;
    if (node->graphScene()->centralNode() == node) {
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
        
        QPointF childPos(m_restDistance * cos(phi),
                         m_restDistance * sin(phi));
        childPos += node->scenePos();
        child->setPos(childPos);
        phi += angleIncrement;
    }

    foreach (GraphicsNode *child, children) {
        layoutNodes(child, visitSet);
    } 
}



// This helper method greatly speeds up the layouting algorithm by 
// arranging nodes in a tree-like layout.
void ForceDirectedLayout::preLayout(GraphicsNode *rootNode)
{
    rootNode->setPos(0.0, 0.0);
    rootNode->setFlag(QGraphicsItem::ItemIsMovable, false);
    QSet<GraphicsNode*> visitSet;
    layoutNodes(rootNode, visitSet); 
}


bool ForceDirectedLayout::layout() 
{
    if (m_scene->mouseGrabberItem()) {
        switch (m_state) {

            case CALC_ANIMATION:
                m_state = ABORT_LOOP;
                return true;
                
            case ABORT_LOOP:
                return true;
            case FINISHED:
                startThread();
                return true;
            case CALC_FORCES:    
                return layoutSerial();
        }
    }
    return layoutParallel();
}





bool ForceDirectedLayout::layoutParallel()
{
    QList<GraphicsNode*> nodes = m_scene->graphNodes();
    QList<GraphicsEdge*> edges = m_scene->graphEdges();

    if (nodes.size() == 0) {
        return false;
    }
    GraphicsNode *rootNode = m_scene->centralNode();
    
    
    if (centralNode != rootNode) {
        m_abortMutex.lock();
        if (isRunning()) {
            stop();
            m_aborted.wait(&m_abortMutex);
        }
        m_abortMutex.unlock();
            
        centralNode = rootNode;
        m_edges = edges;
                
        if (needsPreLayout(centralNode))
            preLayout(centralNode);
        m_animations.clear();
        foreach (GraphicsNode *node, nodes)
            m_animations[node].addPoint(node->pos());
        startThread();
    
    } else if (nodes.size() != m_animations.size() || edges.size() != m_edges.size()) {
        m_abortMutex.lock();
        if (isRunning()) {
            stop();
            m_aborted.wait(&m_abortMutex);
        }
        m_abortMutex.unlock();
        foreach (GraphicsNode *node, nodes) {
            if (!m_animations.contains(node)) {
                m_animations.insert(node, NodeAnimation());
            }
        }
        
        QSet<GraphicsNode*> nodesSet = QSet<GraphicsNode*>::fromList(nodes);
        QMutableHashIterator<GraphicsNode*, NodeAnimation> mIter(m_animations);
        while (mIter.hasNext()) {
            mIter.next();
            if (!nodesSet.contains(mIter.key())) {
                mIter.remove();
            }
        }
        
        QHash<GraphicsNode*, NodeAnimation>::iterator iter;
        QHash<GraphicsNode*, NodeAnimation>::iterator end = m_animations.end();
        for (iter = m_animations.begin(); iter != end; ++iter) {
            iter.value().reset();
            iter.value().addPoint(iter.key()->pos());
        }
        
        m_edges = edges;
        startThread();
    } else if (m_animations.empty() || m_animations.constBegin()->pointCount() == 0) { 
        m_abortMutex.lock();
        if (isRunning()) {
            stop();
            m_aborted.wait(&m_abortMutex);
        }
        m_abortMutex.unlock();
        m_animations.clear();
        foreach (GraphicsNode *node, nodes)
            m_animations[node].addPoint(node->pos());
        startThread();
    } 
    
    if (m_animations.constBegin()->pointCount() < 5 && isRunning()) {
        return true;
    }
    
    
    // If there are too many nodes drawing becomes very slow.
    // To speed up the process we will skip a step or more of the animation.
    int nodesCount = nodes.size();
    int nodesToTake = nodesCount / 20 > 0 ? nodesCount / 20 : 1;
    
    bool itemsMoved = false;
    QGraphicsItem *grabberItem = m_scene->mouseGrabberItem();
    
    for (int i = 0; i < nodesCount; i++) {
        GraphicsNode *node = nodes.at(i);
        NodeAnimation & animation = m_animations[node];
        if (animation.pointCount() > nodesToTake) {
            for (int n = 0; n < nodesToTake; n++) {
                animation.takeFirstPoint();
            }
            node->setNewPos(animation.takeFirstPoint());
        } else if (animation.pointCount() > 0) {
            node->setNewPos(animation.takeFirstPoint());
        }
        
        if (grabberItem != node)
            if (node->advance(itemsMoved))
                itemsMoved = true;
    }

    return itemsMoved || m_animations.constBegin()->pointCount() > 0 ||   isRunning();
    
}

void ForceDirectedLayout::calculateAnimation()
{
    m_state = CALC_ANIMATION;
    typedef QHash<GraphicsNode*, NodeAnimation>::iterator AnimationIterator;
    bool done = false;
    int count = 0;
    while (!done) {
        done = true;
        count++;
        
        AnimationIterator aIter;
        AnimationIterator end = m_animations.end();
        for (aIter = m_animations.begin(); aIter != end; ++aIter) {
            GraphicsNode *aNode = aIter.key();
            
            if (m_state == ABORT_LOOP) {
                m_abortMutex.lock();
                foreach (NodeAnimation animation, m_animations) {
                    animation.reset();
                }
                m_abortMutex.unlock();
                m_state = FINISHED;
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

            NodeAnimation &sourceAnimation = m_animations[source];
            NodeAnimation &destAnimation = m_animations[dest];
            
            QLineF line(sourceAnimation.lastPoint(), destAnimation.lastPoint());
            qreal dx = line.dx();
            qreal dy = line.dy();
    
            qreal distance2 = dx * dx + dy * dy;
            qreal distance = sqrt(distance2);
            
            if (distance > 0) {
                qreal force = -(STIFFNESS * (m_restDistance - distance));
        
                sourceAnimation.force += QPointF(force * (dx / distance), force * ( dy / distance));
                destAnimation.force += QPointF(-force * (dx / distance), -force * (dy / distance));
            }
        }

        {
            // Calculate new position for each node.
            AnimationIterator end = m_animations.end();
            AnimationIterator iter ;
            for (iter = m_animations.begin(); iter != end; ++iter) {
                GraphicsNode *node = iter.key();
                if (!node->flags().testFlag(QGraphicsItem::ItemIsMovable)) {
                    m_animations[node].addPoint(node->pos());
                    continue;
                }
                
                NodeAnimation &animation = m_animations[node];
                QPointF resultForce = m_animations[node].force;
                qreal xvel = animation.force.x();
                qreal yvel = animation.force.y();
                qreal tolerance = 0.1;
                if (qAbs(xvel) < tolerance && qAbs(yvel) < tolerance)
                    xvel = yvel = 0;
    
                QPointF previous = animation.lastPoint();
                QPointF newPos = previous + QPointF(xvel, yvel);
                
                newPos.setX(qMin(qMax(newPos.x(), m_scene->sceneRect().left() + 10), m_scene->sceneRect().right() - 10));
                newPos.setY(qMin(qMax(newPos.y(), m_scene->sceneRect().top() + 10), m_scene->sceneRect().bottom() - 10));
                
                if (previous != newPos)
                    done = false;
                
                animation.addPoint(newPos);
                animation.force.setX(0.0);
                animation.force.setY(0.0);
            }
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
    m_state = FINISHED;
}

void ForceDirectedLayout::run()
{
    if (!m_scene->mouseGrabberItem())
        calculateAnimation();   
    else {
        m_state = CALC_FORCES;
        while (m_scene->mouseGrabberItem()) {
            calculateForces();
            int nodesCount = m_scene->graphNodes().size();
            QThread::msleep(qMax(10, 10 * (nodesCount / 20)));
            //qDebug() << qMax(10, 10 * (nodesCount / 20));
        }    
    }
    m_state = FINISHED;
    m_aborted.wakeAll();
}


void ForceDirectedLayout::calculateForces()
{
    QList<GraphicsNode*> nodes = m_scene->graphNodes();
    QList<GraphicsEdge*> edges = m_scene->graphEdges();
    QHash<GraphicsNode*, QPointF> resultForces;
    QList<GraphicsNode*>::const_iterator aIter;
    QList<GraphicsNode*>::const_iterator bIter;
    QList<GraphicsNode*>::const_iterator end = nodes.constEnd();
    for (aIter = nodes.constBegin(); aIter != end; aIter++) {
        GraphicsNode *aNode = *aIter;
        for (bIter = nodes.constBegin(); bIter != end; bIter++) {
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
        QLineF line(edge->source()->pos(), edge->dest()->pos());
        qreal dx = line.dx();
        qreal dy = line.dy();
    
        qreal distance2 = dx * dx + dy * dy;
        qreal distance = sqrt(distance2);
        if (distance > 0) {
            qreal force = -(STIFFNESS *  (m_restDistance - distance));
        
            resultForces[edge->source()] += QPointF(force * (dx / distance), force * ( dy / distance));
            resultForces[edge->dest()] += QPointF(-force * (dx / distance), -force * (dy / distance));
        }
    }
    
    QRectF sceneRect = m_scene->sceneRect();
    foreach (GraphicsNode *node, nodes) {
        if (!node->flags().testFlag(QGraphicsItem::ItemIsMovable)) {
            continue;
        }
        qreal xvel = resultForces[node].x();
        qreal yvel = resultForces[node].y();
        if (qAbs(xvel) < 0.1 && qAbs(yvel) < 0.1)
            xvel = yvel = 0;

        QPointF newPos = node->pos() + QPointF(xvel, yvel);
        newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
        newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
        node->setNewPos(newPos);
    }
}


bool ForceDirectedLayout::layoutSerial()
{
    QGraphicsItem *grabberNode = m_scene->mouseGrabberItem();
    QList<GraphicsNode*> nodes = m_scene->graphNodes();
    foreach (GraphicsNode *node, nodes) {
        if (grabberNode != node)
            node->advance(true);
    }
    return true;
}

void ForceDirectedLayout::startThread()
{
    if (!isRunning()) {
        start(QThread::NormalPriority);
        //  We want to get some points as soon as possible
        QThread::currentThread()->msleep(1);
    }
}













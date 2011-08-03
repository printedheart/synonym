/***************************************************************************
 *   Copyright (C) 2008 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
 *                                                                         *
 *   The following code is heavily based on TGLayout.java             *
 *   from TouchGraph project http://www.touchgraph.com.                    *
 *   The original java code can be downloaded from                         *
 *   http://sourceforge.net/project/showfiles.php?group_id=30469           *
 *                                                                         * 
 *                                                                         *
 *                                                                         *
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
 **************************************************************************/


#include "tglayout.h"

#include <algorithm>
#include <cmath>

static const double REPULSION = 10.0;


TGLayout::TGLayout(QObject *parent)
    :QThread(parent)
{
    m_maxMotion = 10.0;
    m_lastMaxMotion = 0.0;
    m_damper = 1.0;
    m_motionRatio = 0.0; 
    m_damping = true;
    m_restDistance = 100;
    stopped = false;
    connect (this, SIGNAL(updateNodes()), this, SLOT(advanceNodes()), Qt::BlockingQueuedConnection);
    m_doingLayout = false;
	m_terminateFlag = false;
}

TGLayout::~TGLayout()
{
	m_terminateFlag = true;
    if (isRunning()) {
        m_needsLayout.wakeAll();
    }
	
    while (isRunning())
        wait(0);
}


/**
 The original TGLayout always runs the algorithm
 in a separate thread. We do it only there are no any selected nodes.
 When a user moves nodes around we layout in the same thread, 
 and this appears to be faster.
*/


bool TGLayout::layout()
{
    if (m_scene->mouseGrabberItem()) {
        relax();
        return true;
    } else {
        if (!m_doingLayout) {
            m_needsLayout.wakeAll();
        }
        return false;
    }
}


void TGLayout::relax()
{
    for (int i = 0; i < 10; i++) { 
        relaxEdges();
        avoidLabels();
        moveNodes();
        QList<GraphicsEdge*> edges = m_scene->graphEdges();
    }
}


void TGLayout::run()
{
    forever {
        m_mutex.lock();
        m_needsLayout.wait(&m_mutex);
		if (m_terminateFlag) {
			return;
		}
        m_doingLayout = true;
        do {
             if (m_scene->mouseGrabberItem()) {
                 m_mutex.unlock();
                 break;
             }
            relax();
            emit updateNodes();
        } while (m_maxMotion > 0.001 && m_damper > 0.01 && !stopped);
        m_doingLayout = false;
        m_mutex.unlock();
    }
}


void TGLayout::advanceNodes()
{
    QList<GraphicsNode*> nodes = m_scene->graphNodes();
    QGraphicsItem *grabber = m_scene->mouseGrabberItem();
    foreach (GraphicsNode *node, nodes) {
        if (node != grabber)
            node->setPos(node->p);
    }

    foreach (GraphicsEdge *edge, m_scene->graphEdges()) {
        edge->adjust();
    }
}



void TGLayout::relaxEdge(GraphicsEdge *edge)
{
    QPointF sourcePos = edge->source()->p;
    QPointF destPos = edge->dest()->p;
    double vx = sourcePos.x() - destPos.x();
    double vy = sourcePos.y() - destPos.y();
    qreal distance2 = vx * vx + vy * vy;
    qreal distance = sqrt(distance2);
    if (distance == 0) return;

     // This is how original TGLayout calculates force 
    //between adjacent edges. 
//     double dx = vx;
//     double dy = vy;
//     dx /= (40 * 100);
//     dy /= (40 * 100);
//    QPointF move(- dx * distance, - dy * distance);
    
    
    //We do it the same way as in ForceDirectedLayout.    
    qreal force = 0.2 * (m_restDistance - distance);
    double dx = force * (vx / distance);
    double dy = force * (vy / distance);
    QPointF move(dx, dy); 
     
    edge->source()->d += move;
    edge->dest()->d -= move;
}


void TGLayout::relaxEdges()
{
    QList<GraphicsEdge*> edges = m_scene->graphEdges();
    foreach (GraphicsEdge *edge, edges)
        relaxEdge(edge);
}

void TGLayout::avoidLabels()
{
    QList<GraphicsNode*> nodes = m_scene->graphNodes();
    QList<GraphicsNode*>::const_iterator aIter;
    QList<GraphicsNode*>::const_iterator bIter;
    QList<GraphicsNode*>::const_iterator end = nodes.constEnd();
    for (aIter = nodes.constBegin(); aIter != end; aIter++) {
        GraphicsNode *aNode = *aIter;
        
        for (bIter = nodes.constBegin(); bIter != end; bIter++) {
            GraphicsNode *bNode = *bIter;
            if (aNode == bNode) continue;
                
            double vx = aNode->p.x() - bNode->p.x();
            double vy = aNode->p.y() - bNode->p.y();
            double dx = 0.0;
            double dy = 0.0;
            double distance2 = vx * vx + vy * vy;
            if (distance2 == 0.0) {
                dx = qrand() % 10 * (qrand() % 2 == 1 ? 1 : -1);
                dy = qrand() % 10 * (qrand() % 2 == 1 ? 1 : -1);
            } else {
                
                dx = vx /  (distance2);
                dy = vy / (distance2);
            }
            
            QPointF move(dx * aNode->mass() * bNode->mass() * REPULSION, 
                         dy * aNode->mass() * bNode->mass() * REPULSION);
            aNode->d += move;
            bNode->d -= move;
        }
    }
}


void TGLayout::moveNode(GraphicsNode *node) {
    double dx = node->d.x();
    double dy = node->d.y();
    dx *= m_damper;
    dy *= m_damper;
    
    node->d.setX(dx / 2);
    node->d.setY(dy / 2);
    
    double distMoved = sqrt(dx * dx + dy * dy);
    dx = qMax(-30.0, qMin(30.0, dx));
    dy = qMax(-30.0, qMin(30.0, dy));
    
//     QRectF sceneRect = m_scene->sceneRect();
//     dx = qMin(qMax(dx, sceneRect.left() + 10), sceneRect.right() - 10);
//     dy = qMin(qMax(dy, sceneRect.top() + 10), sceneRect.bottom() - 10);
    
    if (node->flags().testFlag(QGraphicsItem::ItemIsMovable) && node != m_scene->mouseGrabberItem()) {
        node->p += QPointF(dx, dy);
    }
    m_maxMotion = qMax(distMoved, m_maxMotion);
}

void TGLayout::moveNodes()
{
    m_lastMaxMotion = m_maxMotion;
    m_maxMotion = 0.0;
    QList<GraphicsNode*> nodes = m_scene->graphNodes();
    QGraphicsItem *grabberItem = m_scene->mouseGrabberItem();
    foreach (GraphicsNode *node, nodes) {
        moveNode(node);
        if (grabberItem && node != grabberItem) {
            node->setPos(node->p);
            foreach (GraphicsEdge *edge, node->edges()) {
                edge->adjust();
            }
        }
    }
   
    if (m_maxMotion > 0.0) 
        m_motionRatio = m_lastMaxMotion / m_maxMotion - 1;
    else
        m_motionRatio = 0.0;
    damp();
}

void TGLayout::damp()
{
    if (m_damping) {
        if (m_motionRatio <= 0.001) {
            if ((m_maxMotion<0.2 || (m_maxMotion>1 && m_damper<0.9)) && m_damper > 0.01) m_damper -= 0.01;
                //If we've slowed down significanly, damp more aggresively (then the line two below)
            else if (m_maxMotion<0.4 && m_damper > 0.003) m_damper -= 0.003;
                //If max motion is pretty high, and we just started damping, then only damp slightly
            else if(m_damper>0.0001) m_damper -=0.0001;
        }
    } 
    if (m_maxMotion < 0.001 && m_damping) {
        m_damper = 0.0;
    }
}

void TGLayout::startDamper()
{
    m_damping = true;
}
    
void TGLayout::stopDamper()
{
    m_damping = false;
    m_damper = 1.0;
}


void TGLayout::resetDamper()
{
    m_damping = true;
    m_damper = 1.0;
    stopped = false;
}
    
void TGLayout::stop()
{
    stopped = true;
}






    

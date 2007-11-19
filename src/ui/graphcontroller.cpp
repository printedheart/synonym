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
#include "graphcontroller.h"


#include "graphscene.h"
#include "worddataloader.h"
#include "worddatagraph.h"
#include "graphnode.h"
#include "graphedge.h"
#include "graphalgorithms.h"
#include "wordnetutil.h"                  
                  
#include <cstdlib>
#include <ctime>        
#include <QtCore>
#include <QtDebug>
#include <QtGui>
#include <cmath>        
                                       

static const int  HISTORY_SIZE = 10;

GraphController::GraphController(GraphScene *graphScene,
                                  WordDataLoader *loader, QObject *parent)
    : QObject(parent), m_scene(graphScene), m_loader(loader), m_lastGraph(0)
{
    srand((unsigned)time(0));

}


GraphController::~GraphController()
{
    
}


WordGraph*  GraphController::makeGraph(const QString &word)
{
    qDebug() << "makeGraph( " << word << " )";
    
    if (m_scene->centralNode() && m_scene->centralNode()->id() == word)
        return m_lastGraph;
    m_scene->setLayout(false);
    foreach (QGraphicsItem *item, m_scene->items()) {
        if (!item->parentItem())
            m_scene->removeItem(item);
    }
   
    
    GraphicsNode *centralNode;
    if (m_lastGraph && m_lastGraph->node(word) && IsMeaning()(m_lastGraph->node(word))) {
        centralNode = m_lastGraph->node(word);
        m_lastGraph->enableAll();     
        // Reset current central node
        GraphicsNode *currentCentralNode = m_scene->centralNode();
        currentCentralNode->setMass(1.0);
        currentCentralNode->setFlag(QGraphicsItem::ItemIsMovable);
    } else {
        if (m_lastGraph) {
            m_lastGraph->clearAll();
            delete m_lastGraph;
        }
        qDebug() << "load graph";
        QList<Relationship::Type> types;
        types.append(Relationship::Derivation); 
        m_lastGraph = m_loader->createWordGraph(word, Relationship::types());
        qDebug() << "loaded graph";            
        centralNode = m_lastGraph->node(word);
        if (!centralNode)
            return m_lastGraph;
    } 
    
    centralNode->setMass(10);
    
    // Scan the graph from the root and set the level for each node
    // Apply filter to avoid too many nodes appear on the graph.
    // Tha latter could make graph not connected, so we make it connected by 
    // hiding disconnected nodes.
    levelOrderScan(centralNode, SetLevel());
    QList<GraphicsNode*> nodes = m_lastGraph->nodes();
    QSet<GraphicsNode*> visualNodes;
    filter (nodes.constBegin(), nodes.constEnd(), visualNodes,  VisualFilter());
  
    foreach (GraphicsNode *node, nodes) {
        if (!visualNodes.contains(node)) {
            m_lastGraph->disableNode(node);
        } 
    }
    makeConnected(centralNode);
    
    
    
    QList<GraphicsEdge*> edges = m_lastGraph->edges();
    foreach (GraphicsEdge *edge, edges) {
        m_scene->addItem(edge);
    }
    
    QList<GraphicsNode*> finalListOfVisualNodes = m_lastGraph->nodes();
    foreach (GraphicsNode *node, finalListOfVisualNodes) {
        m_scene->addItem(node);
        node->setCursor(Qt::PointingHandCursor);
        if (node->id() == word) {
            m_scene->setCentralNode(node);
            node->setFlag(QGraphicsItem::ItemIsMovable, false);
        } else {
            node->setFlag(QGraphicsItem::ItemIsMovable);
        }
        if (IsMeaning()(node)) {
            node->setAcceptsHoverEvents(true);
        }
    }

    qDebug() << "madeGraph for word "  << word;
    m_scene->setLayout(true);
    m_scene->itemMoved();
    return m_lastGraph;
        
    
}


QList<QPair<WordGraph*, QList<QGraphicsItem*> > >::const_iterator
        GraphController::findInHistory(const QString &word)
{
    QList<QPair<WordGraph*, QList<QGraphicsItem*> > >::const_iterator iter;
    for (iter = m_graphHistory.constBegin(); iter != m_graphHistory.constEnd(); ++iter) {
        WordGraph *wordGraph = (*iter).first;
        if (wordGraph->centralNode()->id() == word)
            return iter;
    }
    return iter;
}

void GraphController::makeConnected(Node *goal)
{ 
    bool connected = false;
    while (!connected) {
        connected = true;
        QList<Node*> nodes = m_lastGraph->nodes();
        if (nodes.size() == 0)
            break;
        QList<Node*>::iterator iter = nodes.begin();
        for (; iter != nodes.end(); ++iter) {
            if (!isReachable(*iter, goal)) {
                m_lastGraph->disableNode(*iter);
                connected = false;    
            }
        }
    }
}

void GraphController::soundReady(const QString &word)
{
   WordGraph *wordGraph = m_graphHistory.back().first;
   Node *node = wordGraph->centralNode();
   if (node && node->data(WORD).toString() == word) {
        m_scene->displaySoundIcon();
   }
}

void GraphController::assertGraphConnectivityToNode(Node *goalNode)
{
    QList<Node*> nodes = m_lastGraph->nodes();
    foreach (Node *node, nodes) {
        QString displayString;
        if (IsWord() (node)) 
            displayString = node->data(WORD).toString();
        else
            displayString = node->data(MEANING).toString();
        Q_ASSERT_X(isReachable(node, goalNode), "assertGraphConnectivity", displayString.toLatin1().data());
    }
}
    

#include "moc_graphcontroller.cpp"

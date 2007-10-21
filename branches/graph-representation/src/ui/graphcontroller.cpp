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
#include "edge.h"
#include "node.h"
#include "graphalgorithms.h"
                  
#include <cstdlib>
#include <ctime>        
#include <QtCore>
#include <QtDebug>
#include <QtGui>
#include <cmath>        
                                       

static const int  HISTORY_SIZE = 10;

GraphController::GraphController(GraphScene *graphScene,
                                  WordDataLoader *loader, QObject *parent)
    : QObject(parent), m_scene(graphScene), m_loader(loader)
{
    srand((unsigned)time(0));

}


GraphController::~GraphController()
{
    
}



WordGraph*  GraphController::makeGraph(const QString &word)
{
    qDebug() << "makeGraph";
    m_scene->setLayout(false);
    
    QList<QGraphicsItem*> items = m_scene->items();
    foreach (QGraphicsItem *item, items) {
        if (!item->parentItem())
            m_scene->removeItem(item);
    }

    WordGraph *wordGraph;
    QList<QPair<WordGraph*, QList<QGraphicsItem*> > >::const_iterator iter =
            findInHistory(word);
    if (iter != m_graphHistory.constEnd()) {
        wordGraph = (*iter).first;
        QList<QGraphicsItem*> items = (*iter).second;
        foreach (QGraphicsItem *item, items)
            m_scene->addItem(item);
            
    } else {
        wordGraph = m_loader->createWordGraph(word);
        
        qDebug() << wordGraph->toString();
        
        Node *centralNode = wordGraph->centralNode();
        QList<Edge*> edges = centralNode->edges();
        
        WordNode *wordNode = qobject_cast<WordNode*>(centralNode);
        GraphicsNode *centralGraphicsNode = new WordGraphicsNode(m_scene);
        centralGraphicsNode->setNode(wordNode);
        centralGraphicsNode->setMass(10.0);
        m_scene->addItem(centralGraphicsNode);
        m_scene->setCentralNode(centralGraphicsNode);
        
        foreach (Edge *edge , edges) {
            addEdge(centralGraphicsNode, edge);
/*            GraphEdge *graphEdge = new GraphEdge();
            QString relationship = Relationship::toString(edge->relationship());
            if (!relationship.isEmpty()) {
                graphEdge->setToolTip(relationship);
                graphEdge->setAcceptsHoverEvents(true);
            }            
            m_scene->addItem(graphEdge);
  
  
            GraphicsNode *source = findGraphicsNode(edge->sourceNode());
            graphEdge->setSource(source);
            GraphicsNode *dest = findGraphicsNode(edge->destNode());
            graphEdge->setDest(dest);*/
        }
        
        QList<GraphicsNode*> graphNodes = m_scene->graphNodes();
        QPair<WordGraph*, QList<QGraphicsItem*> > pair;
        pair.first = wordGraph;
        pair.second = m_scene->items();
        m_graphHistory.append(pair);
    
        if (m_graphHistory.size() > HISTORY_SIZE) {
            QPair<WordGraph*, QList<QGraphicsItem*> > pair = m_graphHistory.takeFirst();
            delete pair.first;
            foreach (QGraphicsItem *item, pair.second)
                delete item;
        
            pair.second.clear();
        }
    }           
            
    m_scene->setLayout(true);
    m_scene->itemMoved();
    return wordGraph;    
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


void GraphController::addEdge(GraphicsNode *graphNode, Edge *edge)
{
    qDebug() << graphNode;
    Node *currentNode = graphNode->dataNode();
    Node *otherNode = edge->neighborTo(currentNode);
    
    if (qobject_cast<MeaningNode*>(currentNode) 
        && qobject_cast<MeaningNode*>(otherNode) 
        && currentNode->degree() > 2) {
        currentNode->graph()->removeNode(otherNode->id());
         return;
     }
    
    QList<GraphicsNode*> neighbors = findNeighbors(graphNode, graphNode->edges().constBegin(), graphNode->edges().constEnd());
    foreach (GraphicsNode *neighbor, neighbors) {
        if (neighbor->dataNode() == otherNode) {
            return;
        }
    }
    
    GraphEdge *graphEdge = new GraphEdge();
    QString relationship = Relationship::toString(edge->relationship());
    if (!relationship.isEmpty()) {
        graphEdge->setToolTip(relationship);
        graphEdge->setAcceptsHoverEvents(true);
    }            
    m_scene->addItem(graphEdge);
    graphEdge->setSource(graphNode);
  
    
    GraphicsNode *destGraphicsNode = findGraphicsNode(otherNode);
    graphEdge->setDest(destGraphicsNode);
    
    if (otherNode->edges().size() < 15) {
        foreach (Edge *ed, otherNode->edges()) {
            if (ed != edge) {
                addEdge(destGraphicsNode,  ed);
            }
        }
    } else {
        WordGraph *graph = currentNode->graph();
        foreach (Edge *ed, otherNode->edges()) {
            if (ed != edge) {
                graph->removeEdge(ed->id());
            }
        }
    }
                
}


GraphicsNode* GraphController::findGraphicsNode(Node *dataNode)
{
    Q_ASSERT(dataNode != 0);

    foreach (GraphicsNode *node, m_scene->graphNodes()) {
        if (node->dataNode() == dataNode) {
            return node;
        }
    }
    
    WordNode *wordNode = qobject_cast<WordNode*>(dataNode);
    GraphicsNode *graphNode;
    if (wordNode) {
        WordGraphicsNode *phraseGraphicsNode = new WordGraphicsNode(m_scene);
        phraseGraphicsNode->setNode(wordNode);
        graphNode = phraseGraphicsNode;
    } else {
        MeaningNode *meaningNode = qobject_cast<MeaningNode*>(dataNode);
        MeaningGraphicsNode *meaningGraphicsNode = new MeaningGraphicsNode(m_scene);
        meaningGraphicsNode->setNode(meaningNode);
        graphNode = meaningGraphicsNode;
        graphNode->setAcceptsHoverEvents(true);
    }

    graphNode->setCursor(Qt::PointingHandCursor);


    if (!dataNode->fixed())
        graphNode->setFlag(QGraphicsItem::ItemIsMovable);
    else {
        graphNode->setMass(4.0);
    }
    
    m_scene->addItem(graphNode);  
    return graphNode;
}


void GraphController::soundReady(const QString &word)
{
   WordGraph *wordGraph = m_graphHistory.back().first;
   WordNode *node = qobject_cast<WordNode*>(wordGraph->centralNode());
   if (node && node->word() == word) {
        m_scene->displaySoundIcon();
   }

}

#include "moc_graphcontroller.cpp"

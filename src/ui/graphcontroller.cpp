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
#include "wordgraph.h"
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

/**
    Method brakes the layout of the visual nodes 
    just a little bit.
*/
static void disturbLayout(WordGraph *graph)
{
    QList<GraphicsNode*> nodes = graph->nodes();
    foreach (GraphicsNode* node, nodes) {
        int sign = (qrand() % 2) ? 1 : -1;
        double x = node->pos().x() + (qrand() % 30) * sign;
        double y = node->pos().y() - (qrand() % 30) * sign;
        node->setPos(x, y); 
    }
}

static void setNewGraphToScene(GraphScene *scene, WordGraph *graph)
{
    scene->setLayout(false);
    QList<QGraphicsItem*> items = scene->items();
    foreach (QGraphicsItem *item, items) {
        if (!item->parentItem())
            scene->removeItem(item); 
    }
        
    QList<GraphicsEdge*> edges = graph->edges();
    foreach (GraphicsEdge *edge, edges) {
        scene->addItem(edge);
    }
    
    QList<GraphicsNode*> nodes = graph->nodes();
    foreach (GraphicsNode *node, nodes) {
        scene->addItem(node);
    } 

    scene->setCentralNode(graph->centralNode());
    scene->setLayout(true);
    scene->itemMoved();
}    


GraphController::GraphController(GraphScene *graphScene,
                                  WordDataLoader *loader, QObject *parent)
    : QObject(parent), m_scene(graphScene), m_loader(loader), m_graph(0)
{
    srand((unsigned)time(0));
    m_poses.append(Noun);
    m_poses.append(Verb);
    m_poses.append(Adjective);
    m_poses.append(Adverb);
    m_relTypes = Relationship::allTypes();
}


GraphController::~GraphController()
{
     
}



WordGraph*  GraphController::makeGraph(const QString &word)
{
    if (m_scene->centralNode() && m_scene->centralNode()->id() == word)
        return m_graph;
    
    GraphicsNode *centralNode;
    qDebug() << "makeGraph(" << word << ")";
    if (m_graph && m_graph->node(word) && IsMeaning()(m_graph->node(word))) {
        m_backHistory.append(m_graph->clone());
        m_graph->setCentralNode(word);
        centralNode = m_graph->node(word);
        m_graph->enableAll();     
        // Reset current central node
        GraphicsNode *currentCentralNode = m_scene->centralNode();
        currentCentralNode->setMass(1.0);
        currentCentralNode->setFlag(QGraphicsItem::ItemIsMovable); 
    } else {
        WordGraph *newGraph = m_loader->createWordGraph(word, m_relTypes);
        if (!newGraph)
            return m_graph;
        
        centralNode = newGraph->node(word);
        if (!centralNode) {
            delete newGraph;
            return m_graph; 
        }
        
        if (m_graph)
            m_backHistory.append(m_graph);
        
        m_graph = newGraph;
        m_graph->setCentralNode(word);
    } 
    
    if (m_backHistory.size() > HISTORY_SIZE) {
        WordGraph *graph = m_backHistory.takeFirst();
        delete graph;
    }
    foreach (WordGraph *g, m_forwardHistory) {
        g->deleteLater();
    }
    m_forwardHistory.clear();
    
    centralNode->setMass(10);
    centralNode->setFlag(QGraphicsItem::ItemIsMovable, false); 
    filterGraphNodes();
    setNewGraphToScene(m_scene, m_graph);
    m_graph->setEnabledSignals(true);
    return m_graph;
}

void GraphController::filterGraphNodes()
{
     // Scan the graph from the root and set the level for each node
    // Apply filter to avoid too many nodes appear on the graph.
    
    Node *centralNode = m_graph->centralNode();
    levelOrderScan(centralNode, SetLevel());
    QList<GraphicsNode*> nodes = m_graph->nodes();    
    IsPOS posPredicate(m_poses);
    
    // Use different strategies here. If the central node is word node
    // we use recursive algorithm. For meaning node we just scant a list of nodes.
    if (IsWord() (centralNode)) {
        VisualFilterForDFS filter;
        depthFirstSearch(centralNode, filter);
        QSet<GraphicsNode*> disabledNodes = filter.disabledNodes;
        qDebug() << "disabledNodes " << disabledNodes.size();
        foreach (GraphicsNode *node, nodes) {
            if (disabledNodes.contains(node) || !posPredicate(node)) {
                m_graph->disableNode(node);
            } 
        }
    } else {
        QSet<Node*> visualNodes;
        filter (nodes.constBegin(), nodes.constEnd(), visualNodes,
                FunctionCombiner<MeaningVisualFilter, IsPOS>(MeaningVisualFilter(), posPredicate));
        foreach (Node *node, nodes) {
            if (!visualNodes.contains(node)) {
                m_graph->disableNode(node);
            }
        }
    }
        
    // The filtering could make graph not connected, so we make it
    // connected by hiding disconnected nodes.
    makeConnected(centralNode);
}

WordGraph * GraphController::previousGraph()
{
    if (m_backHistory.size() == 0)
        return 0;
    
    WordGraph *lastGraph = m_backHistory.takeLast();
    m_forwardHistory.append(m_graph);
    m_graph = lastGraph;
    disturbLayout(m_graph);
    setNewGraphToScene(m_scene, m_graph);
    return m_graph;
}


WordGraph * GraphController::nextGraph()
{
    if (m_forwardHistory.size() == 0)
        return 0;    
    
    WordGraph *lastGraph = m_forwardHistory.takeLast();
    m_backHistory.append(m_graph);
    m_graph = lastGraph;
    disturbLayout(m_graph);
    setNewGraphToScene(m_scene, m_graph);
    return m_graph;
}

void GraphController::makeConnected(Node *goal)
{ 
    bool connected = false;
    while (!connected) {
        connected = true;
        QList<Node*> nodes = m_graph->nodes();
        if (nodes.size() == 0)
            break;
        
        QList<Node*>::iterator end = nodes.end();
        for (QList<Node*>::iterator iter = nodes.begin(); iter != end; ++iter) {
            if (!isReachable(*iter, goal)) {
                m_graph->disableNode(*iter);
                connected = false;    
            }
        }
    }
}

void GraphController::soundReady(const QString &word)
{
/*   WordGraph *wordGraph = m_graphHistory.back().first;
   Node *node = wordGraph->centralNode();
   if (node && node->data(WORD).toString() == word) {
        m_scene->displaySoundIcon();
   }*/
}

void GraphController::assertGraphConnectivityToNode(Node *goalNode)
{
    QList<Node*> nodes = m_graph->nodes();
    foreach (Node *node, nodes) {
        QString displayString;
        if (IsWord() (node)) 
            displayString = node->data(WORD).toString();
        else
            displayString = node->data(MEANING).toString();
        Q_ASSERT_X(isReachable(node, goalNode), "assertGraphConnectivity", displayString.toLatin1().data());
    }
}

void GraphController::setPoses(QList<PartOfSpeech> &poses)
{
    m_poses = poses;
    if (!m_graph)
        return;
    
    m_graph->setEnabledSignals(false);
    m_graph->enableAll();
    QList<GraphicsNode*> nodes = m_graph->nodes();
    QSet<GraphicsNode*> visualNodes;
    
    if (IsWord() (m_graph->centralNode())) {
        filter (nodes.constBegin(), nodes.constEnd(), visualNodes,
                FunctionCombiner<VisualFilter, IsPOS>(VisualFilter(), IsPOS(m_poses)));
    } else {
        filter (nodes.constBegin(), nodes.constEnd(), visualNodes,
                FunctionCombiner<MeaningVisualFilter, IsPOS>(MeaningVisualFilter(), IsPOS(m_poses)));
    }
  
    foreach (GraphicsNode *node, nodes) {
        if (!visualNodes.contains(node)) {
            m_graph->disableNode(node);
        } 
    }
    m_graph->setEnabledSignals(true);
    makeConnected(m_graph->centralNode());
    updateSceneNodes();
}

void GraphController::setRelationships(Relationship::Types relTypes)
{
    int mInt = m_relTypes;
    int rInt = relTypes;
    if (mInt == rInt)
        return;
    
    if (!m_graph) {
        m_relTypes = relTypes;
        return;
    }
    
    Relationship::Types newTypes = ((~m_relTypes) & relTypes) | (m_relTypes & relTypes);
    if (newTypes == 0) {
        return;
    }
    
    m_relTypes = relTypes;
    QList<Edge*> edges = m_graph->edges();
    QSet<Edge*> filteredEdges;
    IsInRelationships predicate(m_relTypes);
    filter(edges.constBegin(), edges.constEnd(), filteredEdges, predicate);
    foreach (Edge *edge, edges) {
        if (!filteredEdges.contains(edge)) {
            m_graph->disableEdge(edge);
        }
    }
    
    makeConnected(m_graph->centralNode());
    updateSceneNodes();
}
    
    
void GraphController::updateSceneNodes()
{
    QSet<QGraphicsItem*> graphItems;
    QList<GraphicsNode*> filteredNodes = m_graph->nodes();
    QList<GraphicsEdge*> filteredEdges = m_graph->edges();
    
    foreach (GraphicsNode *node, filteredNodes)
        graphItems.insert(node);
    foreach (GraphicsEdge *edge, filteredEdges)
        graphItems.insert(edge);
    
    QSet<QGraphicsItem*> sceneItems;
    
    QList<GraphicsNode*> sceneNodes = m_scene->graphNodes();
    QList<GraphicsEdge*> sceneEdges = m_scene->graphEdges();
    
    foreach (GraphicsNode *node, sceneNodes)
        sceneItems.insert(node);
    foreach (GraphicsEdge *edge, sceneEdges)
        sceneItems.insert(edge);
    
    QSet<QGraphicsItem*> sceneItemsCopy(sceneItems);  
    QSet<QGraphicsItem*> nodesToRemove = sceneItems.subtract(graphItems);
    QSet<QGraphicsItem*> nodesToAdd = graphItems.subtract(sceneItemsCopy);
    
    m_scene->setLayout(false);
    foreach (QGraphicsItem *oldItem, nodesToRemove)
        m_scene->removeItem(oldItem);   
    
    foreach (QGraphicsItem *newItem, nodesToAdd) {
        m_scene->addItem(newItem);
    }
    m_scene->setLayout(true);
    m_scene->itemMoved();
}
    

#include "moc_graphcontroller.cpp"
    

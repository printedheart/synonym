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
#include "iworddataloader.h"
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
        double x = node->pos().x() + (qrand() % 50) * sign;
        double y = node->pos().y() - (qrand() % 50) * sign;
        node->setPos(x, y); 
    }
}  

static void setNewGraphToScene(GraphScene *scene, WordGraph *graph, bool resetPos = false)
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
        if (resetPos) {
            node->setPos(QPointF());
        }
        scene->addItem(node);
    } 

    scene->setCentralNode(graph->centralNode());
    scene->setLayout(true);
    scene->itemMoved();
}   
 
static void adjustMass(WordGraph *graph)
{
    QList<GraphicsNode*> nodes = graph->nodes();
    foreach (GraphicsNode *node, nodes) {
        if (IsMeaning() (node)) {
            QSet<GraphicsNode*> neighbors = node->neighbors(); 
            if (neighbors.size() > 3) {
                foreach (GraphicsNode *neighbor, neighbors) {
                    if (neighbor != graph->centralNode()) {
                        neighbor->setMass(2);
                    }
                }
            }   
        } 
    }
} 


GraphController::GraphController(GraphScene *graphScene,
                                  IWordDataLoader *loader, QObject *parent)
    : QObject(parent), m_scene(graphScene), m_loader(loader), m_graph(0)
{
    srand((unsigned)time(0));
    m_poses.append(Noun);
    m_poses.append(Verb);
    m_poses.append(Adjective);
    m_poses.append(Adverb);
    m_relTypes = Relation::allTypes();
}


GraphController::~GraphController()
{
     
}

 

WordGraph*  GraphController::makeGraph(const QString &word)
{
    if (!m_scene->centralNode() || m_scene->centralNode()->id() != word) {
        GraphicsNode *centralNode;
        qDebug() << "makeGraph(" << word << ")";
        if (m_graph && m_graph->node(word) && IsMeaning()(m_graph->node(word))) {
            WordGraph *clone = m_graph->clone();
            m_backHistory.append(m_graph);
            m_graph = clone;
            GraphicsNode *prevCentralNode = m_graph->node(clone->centralNode()->id());
            prevCentralNode->setMass(1.0);
            prevCentralNode->setFlag(QGraphicsItem::ItemIsMovable);
            
            m_graph->setCentralNode(word);
            centralNode = m_graph->node(word);
            m_graph->enableAll();     
        } else {
            QString searchWord = word;
            WordGraph *newGraph = m_loader->createWordGraph(searchWord, m_relTypes);
            if (!newGraph)
                return m_graph;
              
            centralNode = newGraph->node(searchWord);    
            if (!centralNode) {
                delete newGraph;
                return m_graph; 
            }
            
            if (m_graph) 
                m_backHistory.append(m_graph);
            
            m_graph = newGraph;
            m_graph->setCentralNode(searchWord);
        }   
        
        if (m_backHistory.size() > HISTORY_SIZE) {
            WordGraph *graph = m_backHistory.takeFirst();
            delete graph;
        }
        foreach (WordGraph *g, m_forwardHistory) {
            g->deleteLater();
        }
        m_forwardHistory.clear();
        
     //   centralNode->setMass(10);
        centralNode->setFlag(QGraphicsItem::ItemIsMovable, false);     
        filterGraphNodes();
        adjustMass(m_graph);
    }
    
    applyUserSettings();
    setNewGraphToScene(m_scene, m_graph, m_scene->centralNode() != m_graph->centralNode());
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
    QSet<GraphicsNode*> disabledNodes;
    if (IsWord() (centralNode)) {
        VisualFilter filter; 
        depthFirstSearch(centralNode, filter);
        disabledNodes = filter.disabledNodes;
    } else {
        MeaningVusualFilter filter;
        depthFirstSearch(centralNode, filter);
        disabledNodes = filter.disabledNodes;
    }
    foreach (GraphicsNode *node, nodes) {
        if (disabledNodes.contains(node) || !posPredicate(node)) {
            m_graph->disableNode(node);
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
    applyUserSettings();
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
    applyUserSettings();
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
    filterGraphNodes();
    m_graph->setEnabledSignals(true);
    applyUserSettings();   
    updateSceneNodes();
}

void GraphController::setrelations(Relation::Types relTypes)
{
    int mInt = m_relTypes;
    int rInt = relTypes;
    if (mInt == rInt)
        return;
    
    if (!m_graph) {
        m_relTypes = relTypes;
        return;
    }
      
    Relation::Types newTypes = ((~m_relTypes) & relTypes) | (m_relTypes & relTypes);
    if (newTypes == 0) {
        return;
    }
    
    m_relTypes = relTypes;
    QList<Edge*> edges = m_graph->edges();
    QSet<Edge*> filteredEdges;
    filter(edges.constBegin(), edges.constEnd(), filteredEdges, WithRelations(m_relTypes));   
    foreach (Edge *edge, edges) {
        if (!filteredEdges.contains(edge)) {
            m_graph->disableEdge(edge);
        }
    }
    
    makeConnected(m_graph->centralNode());
    applyUserSettings();
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


void GraphController::applyUserSettings()
{
    QSettings settings("http://code.google.com/p/synonym/", "synonym");
    if (!settings.childGroups().contains("display"))
        return;
    
    settings.beginGroup("display");
    
    //word nodes
    QFont font;
    font.setFamily(settings.value("Font Family").toString());
    font.setPointSize(settings.value("Font Size").toInt());
    QList<Node*> nodes = m_graph->nodes();
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i] == m_graph->centralNode()) {
            QFont bigFont = font;
            bigFont.setPointSize(qRound(font.pointSize() * 1.5));
            nodes[i]->setFont(bigFont);
        } else 
            nodes[i]->setFont(font);
    }
    
    // meaning nodes
    const QColor posColors[4] = {
        QColor(settings.value("Noun Color").toString().trimmed()), 
        QColor(settings.value("Verb Color").toString().trimmed()),
        QColor(settings.value("Adjective Color").toString().trimmed()),
        QColor(settings.value("Adberb Color").toString().trimmed())
    };
    int circleRadius = settings.value("Circle Radius").toInt();                     
    QList<Node*> meaningNodes = m_graph->nodes(IsMeaning());
    for (int i = 0; i < meaningNodes.size(); i++) {
        MeaningGraphicsNode *meaningNode = static_cast<MeaningGraphicsNode*>(meaningNodes[i]);
        QColor c = posColors[meaningNode->data(POS).toInt() - 1];
        if (c.isValid())
            meaningNode->setCircleColor(c);
        // else invalid, node will use static default
        meaningNode->setCircleRadius(circleRadius);
    }
    
    // edges
    double edgeWidth = (double) settings.value("Edge Width").toInt() / 10;
    int edgeLineContrast = settings.value("Edge Contrast").toInt();
    QList<Edge*> edges = m_graph->edges();
    foreach (Edge *edge, edges) {
        edge->setLineWidth(edgeWidth);
        edge->setLineContrast(edgeLineContrast);
    }
}
    

#include "moc_graphcontroller.cpp"
    

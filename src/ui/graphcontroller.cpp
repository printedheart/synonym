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
#include "../model/worddataloader.h"
#include "../model/worddatagraph.h"
#include "graphnode.h"
#include "graphedge.h"
#include "../model/edge.h"
#include "../model/node.h"                
                  
#include <cstdlib>
#include <ctime>        
#include <QtCore>
#include <QtDebug>
#include <QtGui>
#include <cmath>        
                                       
        
static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

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



WordDataGraph*  GraphController::makeGraph(const QString &word)
{
    m_scene->setCalculate(false);
    
    QList<QGraphicsItem*> items = m_scene->items();
    foreach (QGraphicsItem *item, items)
        m_scene->removeItem(item);

    WordDataGraph *dataGraph;
    QList<QPair<WordDataGraph*, QList<QGraphicsItem*> > >::const_iterator iter =
            findInHistory(word);
    if (iter != m_graphHistory.constEnd()) {
        dataGraph = (*iter).first;
        QList<QGraphicsItem*> items = (*iter).second;
        foreach (QGraphicsItem *item, items)
            m_scene->addItem(item);
            
    } else {
        dataGraph = new WordDataGraph();
        m_loader->load(word, dataGraph);
        
    
    //TODO: What if dataGraph is empty.
        
        QList<Edge*> edges = dataGraph->edges();
        DataNode *centralNode = dataGraph->centralNode();
        

        foreach (Edge *edge, centralNode->edges()) {
            qDebug() << edge->id() << "  " << edge->sourceNode()->id() << "  " << edge->destNode()->id();
        }

        qDebug() << "\n\n";


        
        PhraseNode *phraseNode = qobject_cast<PhraseNode*>(centralNode);
        GraphNode *centralGraphNode = new PhraseGraphNode(m_scene);
        centralGraphNode->setDataNode(phraseNode);
        centralGraphNode->setMass(4.0);
        m_scene->addItem(centralGraphNode);
        
        double angleIncrement = TwoPi / centralNode->edges().size();
        double phi = 0.0;
        foreach (Edge *edge , edges) {
       // qDebug() << edge->id();
            GraphEdge *graphEdge = new GraphEdge();
            m_scene->addItem(graphEdge);
        //graphEdge->setPos((rand()%10)+1, (rand()%10)+1);
  
  
            GraphNode *source = findGraphNode(edge->sourceNode());
            graphEdge->setSource(source);
            GraphNode *dest = findGraphNode(edge->destNode());
            graphEdge->setDest(dest);
//             if (dest->dataNode() == centralNode) {
//                 source->setPos(75 * cos(phi), 75 * sin(phi));
//                 phi += angleIncrement;
//             } else {
//                 source->setPos(source->dataNode()->fixed()
//                     ? QPointF(0, 0)
//                     : QPointF(-15 + qrand() % 200, -15 + qrand() % 200));
//                     //: QPointF(75 * cos(phi) * (qrand() % 5), 75 * sin(phi) * (qrand() % 5)));
//             }
//             if (source->dataNode() == centralNode) {
//                 dest->setPos(75 * cos(phi), 75 * sin(phi));
//                 phi += angleIncrement;
//             } else {
//                 dest->setPos(dest->dataNode()->fixed()
//                     ? QPointF(0, 0)
//                     : QPointF(-15 + qrand() % 200, -15 + qrand() % 200));
//                     //: QPointF(75 * cos(phi) * (qrand() % 5) , 75 * sin(phi) * (qrand() % 5)));
//             }
//         }
        }

        QPair<WordDataGraph*, QList<QGraphicsItem*> > pair;
        pair.first = dataGraph;
        pair.second = m_scene->items();
        m_graphHistory.append(pair);
    
        if (m_graphHistory.size() > HISTORY_SIZE) {
            QPair<WordDataGraph*, QList<QGraphicsItem*> > pair = m_graphHistory.takeFirst();
            delete pair.first;
            foreach (QGraphicsItem *item, pair.second)
                delete item;
        
            pair.second.clear();
        }
        m_scene->doInitialLayout(centralGraphNode);

    }           
            
    m_scene->setCalculate(true);
    
    m_scene->itemMoved();
    
    return dataGraph;    
}


QList<QPair<WordDataGraph*, QList<QGraphicsItem*> > >::const_iterator
        GraphController::findInHistory(const QString &word)
{
    QList<QPair<WordDataGraph*, QList<QGraphicsItem*> > >::const_iterator iter;
    for (iter = m_graphHistory.constBegin(); iter != m_graphHistory.constEnd(); ++iter) {
        WordDataGraph *dataGraph = (*iter).first;
        if (dataGraph->centralNode()->id() == word)
            return iter;
    }
    return iter;
}


GraphNode* GraphController::findGraphNode(DataNode *dataNode)
{
    Q_ASSERT(dataNode != 0);

    foreach (GraphNode *node, m_scene->graphNodes()) {
        if (node->dataNode() == dataNode) {
            return node;
        }
    }
    
    PhraseNode *phraseNode = qobject_cast<PhraseNode*>(dataNode);
    GraphNode *graphNode;
    if (phraseNode) {
        PhraseGraphNode *phraseGraphNode = new PhraseGraphNode(m_scene);
        phraseGraphNode->setDataNode(phraseNode);
        graphNode = phraseGraphNode;
    } else {
        MeaningNode *meaningNode = qobject_cast<MeaningNode*>(dataNode);
        MeaningGraphNode *meaningGraphNode = new MeaningGraphNode(m_scene);
        meaningGraphNode->setDataNode(meaningNode);
        graphNode = meaningGraphNode;
        graphNode->setAcceptsHoverEvents(true);
    }

    graphNode->setCursor(Qt::PointingHandCursor);


    if (!dataNode->fixed())
        graphNode->setFlag(QGraphicsItem::ItemIsMovable);
    else
        graphNode->setMass(4.0);
    
    m_scene->addItem(graphNode);  
    return graphNode;
}

#include "moc_graphcontroller.cpp"

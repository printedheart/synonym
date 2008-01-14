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
#include "graphtest.h"

#include "wordgraph.h"
#include "graphnode.h"
#include "graphedge.h"
class TestGraph : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void testGraph();
        void testClone();
    
    private:    
        WordGraph *wordGraph;            
    
};




void TestGraph::initTestCase()
{
    TemplateNodeFactory<WordGraphicsNode> wordFactory;
    TemplateEdgeFactory<GraphicsEdge> edgeFactory;
    wordGraph = new WordGraph();
    wordGraph->addNode("word", wordFactory);
    wordGraph->addNode("order", wordFactory);
    wordGraph->addEdge("word", "order", edgeFactory);
    
    wordGraph->addNode("news", wordFactory);
    wordGraph->addEdge("word", "news", edgeFactory);

    wordGraph->addNode("unreachable", wordFactory);
    
}

void TestGraph::testClone()
{
    WordGraph *clone  = wordGraph->clone();
    QVERIFY(clone->nodes().size() == wordGraph->nodes().size());
    QVERIFY(clone->edges().size() == wordGraph->edges().size());
    
    Node *wordNode = clone->node("word");
    QVERIFY(wordNode->graph() == clone);
    QVERIFY(wordNode);
    QVERIFY(wordNode->edges().size() == 2);
    foreach (Edge *edge, wordNode->edges()) {
        QVERIFY(edge->graph() == clone);
    }
    
    Node *orderNode = clone->node("order");
    QVERIFY(orderNode->graph() == clone);
    QVERIFY(orderNode->edges().size() == 1);
    
    Edge *word_order = clone->edge("word-order");
    QVERIFY(word_order);
    QVERIFY(word_order->source() == wordNode);
    QVERIFY(word_order->dest() == orderNode);
    QVERIFY(*(orderNode->edges().begin()) == word_order);
    
    Node *newsNode = clone->node("news");
    QVERIFY(newsNode->graph() == clone);
    QVERIFY(newsNode->edges().size() == 1);
    
    Edge *word_news = clone->edge("word-news");
    QVERIFY(word_news);
    QVERIFY(word_news->source() == wordNode);
    QVERIFY(word_news->dest() == newsNode);
    QVERIFY(*(newsNode->edges().begin()) == word_news);
    
    foreach (Edge *edge, wordNode->edges()) {
        QVERIFY(edge == word_order || edge == word_news);
    }
    
}

void TestGraph::testGraph()
{
    Node *orderNode = wordGraph->node("order");
    Node *wordNode = wordGraph->node("word");
    bool reachable = isReachable(orderNode, wordNode); 
    QVERIFY(reachable);
    
    Node *unreachableNode = wordGraph->node("unreachable");
    bool unreachable = isReachable(unreachableNode, wordNode);
    QVERIFY(!unreachable);
}




QTEST_MAIN(TestGraph)
#include "graphtest.moc"
        
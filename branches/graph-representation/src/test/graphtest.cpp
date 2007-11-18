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

#include "worddatagraph.h"

class TestGraph : public QObject
{
    Q_OBJECT
    private slots:
        void initTestCase();
        void testGraph();
    
    private:    
        WordGraph wordGraph;            
    
};




void TestGraph::initTestCase()
{
    wordGraph.addNode("word", WordFactory());
    wordGraph.addNode("order", WordFactory());
    wordGraph.addEdge("word", "order", EdgeFactory());
    
    wordGraph.addNode("news", WordFactory());
    wordGraph.addEdge("word", "news", EdgeFactory());

    wordGraph.addNode("unreachable", WordFactory());
    
}

void TestGraph::testGraph()
{
    Node *orderNode = wordGraph.node("order");
    Node *wordNode = wordGraph.node("word");
    bool reachable = isReachable(orderNode, wordNode); 
    QVERIFY(reachable);
    
    Node *unreachableNode = wordGraph.node("unreachable");
    bool unreachable = isReachable(unreachableNode, wordNode);
    QVERIFY(!unreachable);
}


QTEST_MAIN(TestGraph)
#include "graphtest.moc"
        
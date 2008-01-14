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
#ifndef WORDGRAPHDECORATOR_H
#define WORDGRAPHDECORATOR_H


#include "wordgraph.h"
#include "relation.h"
#include <QObject>

class WordGraphDecorator : public QObject
{
Q_OBJECT    
    
public:        
    WordGraphDecorator();
    ~WordGraphDecorator();
    
    void setWordNodeFactory(NodeFactory *nodeFactory);
    void setMeaningNodeFactory(NodeFactory *nodeFactory);
    void setEdgeFactory(EdgeFactory *edgeFactory);
    
    void setrelationTypes(Relation::Types relationTypes);
    
    WordGraph *wordGraph() const;
    
public slots:
        
    WordGraph *new_WordGraph();
            
    void addWordNode(WordGraph *wordGraph, QString word);
    void setCentralNode(WordGraph *wordGraph, QString nodeId);
    
    void addMeaningNode(WordGraph *wordGraph, QString id, QString meaning, QString partOfSpeech);
     
    bool addEdge(WordGraph *wordGraph, QString id1, QString id2, QString pointerType = QString());
    
    
    int nodesCount() { return m_wordGraph->nodes().size(); }
    
    
private:    
    WordGraph *m_wordGraph;
    NodeFactory *m_wordNodeFactory;
    NodeFactory *m_meaningNodeFactory;
    EdgeFactory *m_edgeFactory;
    
    Relation::Types m_relationTypes;
};

#endif


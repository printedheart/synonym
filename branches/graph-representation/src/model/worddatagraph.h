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
#ifndef WORDDATAGRAPH_H
#define WORDDATAGRAPH_H

#include "graphnode.h"
#include "graphedge.h"
#include "graphalgorithms.h"
#include <QtCore>

/**
        @author Sergejs <sergey.melderis@gmail.com>
 */





typedef GraphicsNode Node;
typedef GraphicsEdge Edge;


class NodeFactory
{
public:    
    virtual ~NodeFactory() {}
    virtual Node *createNode(const QString &id, WordGraph *graph) = 0;
};

class EdgeFactory
{
    public:    
        virtual ~EdgeFactory() {}
        virtual Edge *createEdge(const QString &id, Node *source, Node *dest,  WordGraph *graph) = 0;
};


template <class T>
class TemplateNodeFactory : public NodeFactory
{
public:
    virtual Node *createNode(const QString &id, WordGraph *graph) { 
        return new T(id, graph);
    }
};

template <class T>
class TemplateEdgeFactory : public EdgeFactory
{
public:            
    virtual Edge *createEdge(const QString &id, Node *source, Node *dest,  WordGraph *graph) {
        return new T(id, source, dest, graph);
    }
};




            
    

class WordGraph : public QObject
{
Q_OBJECT
public:
    WordGraph(QObject *parent = 0);
    ~WordGraph();

    enum PartOfSpeech { Noun = 1, Verb = 2, Adjective = 3, Adverb = 4 };
    
   
    Node * addNode(const QString &nodeId, NodeFactory &factory);
    void removeNode(const QString &nodeId);
    void removeNode(Node *node);
    
    void disableNode(Node *node);
    void enableNode(Node *node);
    void enableAll();
    bool isEnabled(Node *node) const;
    
    Edge *addEdge(const QString &aNodeId, const QString &bNodeId, EdgeFactory &factory);
    
    void removeEdge(const QString &edgeId);
    void removeEdge(Edge *edge);

    void clearAll();

    Node* node(const QString &id) const;
    Edge *edge(const QString &id) const;

    QList<Edge*> edges() const;
    QList<Node*> nodes() const;
    
    template <typename Predicate>
    QList<Node*> nodes(Predicate interesting) const;      

    Node* centralNode() const;
    
    void setSelectedNode(Node *node);
    
signals:
    void nodeAdded(Node *node);
    void nodeRemoved(const QString &nodeId);
    void edgeAdded(Edge *edge);
    
private:
    QHash<QString, Node*> m_nodes;
    QHash<QString, Edge*> m_edges;
    typedef QHash<QString, Node*>::iterator NodeIterator;
    typedef QHash<QString, Node*>::const_iterator ConstNodeIterator;
    typedef QHash<QString, Edge*>::iterator EdgeIterator;
    typedef QHash<QString, Edge*>::const_iterator ConstEdgeIterator;
    QHash<QString, Node*> m_disabledNodes;
    QString m_centralNodeId;
    
   
};


template <typename Function>
class EnabledNodePredicateWrapper
{
public:            
    EnabledNodePredicateWrapper(const WordGraph *graph, Function &func) :
        m_graph(graph), m_function(func) {}
    
    EnabledNodePredicateWrapper<Function>(const EnabledNodePredicateWrapper<Function> &other) :
            m_graph(other.m_graph), m_function(other.m_function) {}
    
        
    bool operator() (Node *node) {
        if (!m_graph->isEnabled(node)) return false;
        return m_function.operator() (node);        
    }            
            
    const WordGraph *m_graph;        
    Function &m_function;
};        

template <typename Predicate>
QList<Node*> WordGraph::nodes(Predicate interesting) const
{
    QList<Node*> list;
    EnabledNodePredicateWrapper<Predicate> predicate(this, interesting);
    filter(m_nodes.constBegin(), m_nodes.constEnd(), list, predicate);
    return list;
}              
          
                    

#endif
        

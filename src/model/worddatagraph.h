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



const int POS            = 111;
const int MEANING        = 222;
const int RELATIONSHIP   = 223;
const int WORD           = 224;
const int LEVEL          = 225;

typedef GraphicsNode Node;
typedef GraphicsEdge Edge;

class WordGraph : public QObject
{
Q_OBJECT
public:
    WordGraph(QObject *parent = 0);
    ~WordGraph();

    enum PartOfSpeech { Noun = 1, Verb = 2, Adjective = 3, Adverb = 4 };
    
    template <typename Factory>
    Node * addNode(const QString &nodeId, Factory factory);
    
    void removeNode(const QString &nodeId);
    void removeNode(Node *node);
    
    template <typename Factory>
    Edge * addEdge(const QString &aNodeId, const QString &bNodeId, Factory factory);
    
    void removeEdge(const QString &edgeId);
    void removeEdge(Edge *edge);

    void clearAll();

    Node* node(const QString &id) const;
    Edge *edge(const QString &id) const;

    QList<Edge*> edges() const;
    QList<GraphicsNode*> nodes() const;
    
    template <typename Predicate>
    QList<GraphicsNode*> nodes(Predicate interesting) const
    {
        QList<GraphicsNode*> list;
        filter(m_nodes.constBegin(), m_nodes.constEnd(), list, interesting);
        return list;
    }                

    Node* centralNode() const;
    
    void setSelectedNode(Node *node);
    
signals:
    void nodeAdded(Node *node);
    void nodeRemoved(const QString &nodeId);
    void edgeAdded(Edge *edge);
    
private:
    QHash<QString, Node*> m_nodes;
    QHash<QString, Edge*> m_edges;
    QString m_centralNodeId;
};



template <typename Factory>
Node * WordGraph::addNode(const QString &nodeId, Factory factory)
{
    if (m_nodes.contains(nodeId))
        return m_nodes[nodeId];
    
    Node *node = factory.operator() (nodeId, this);
    m_nodes[nodeId] = node;
    emit nodeAdded(node);
    return node;
}

template <typename Factory>
Edge * WordGraph::addEdge(const QString &aNodeId, const QString &bNodeId, Factory factory)
{
    QString edgeId = aNodeId + bNodeId;
    QString edgeId2 = bNodeId + aNodeId;
    bool hasEdge = m_edges.contains(edgeId) || m_edges.contains(edgeId2);
    bool hasNodes = m_nodes.contains(aNodeId) && m_nodes.contains(bNodeId);
    Edge *edge = 0;
    if (!hasEdge && hasNodes) {
        edge = factory.operator() (edgeId, m_nodes[aNodeId], m_nodes[bNodeId], this);
        m_edges[edgeId] = edge;
        emit edgeAdded(edge);
    }
    return edge;
}



class IsMeaning
{
public:    
    bool operator() (Node *node) { return node->data(MEANING).isValid(); }
};

class IsWord
{
public:    
    bool operator() (Node *node) { return node->data(WORD).isValid(); }
};

class SetLevel
{
public:
    void operator() (Node *node, int level) { node->setData(LEVEL, level);}
};

class AlwaysTrue
{
public:    
    bool operator() (void *) { return true; }
};


class VisualFilter
{
public:    
    bool operator() (Node *node) {
        int level = node->data(LEVEL).toInt();
        if (level > 3) return false;
        if (level == 3 && IsMeaning() (node)) return false;
        if (level == 3 && IsWord() (node)) {
            foreach (Node *neighbor, node->neighbors()) {
                if (neighbor->edges().size() > 2 
                    && neighbor->data(LEVEL).toInt() < level 
                    && hasWordNeighborsWithGreaterLevel(neighbor)) {
                    return false;
                }
            }
        }    
            
        if (level == 2 && IsMeaning() (node)) {
            foreach (Node *neighbor, node->neighbors()) {
                if (neighbor->data(LEVEL).toInt() < level && hasWordNeighborsWithGreaterLevel(neighbor)) {
                    return false;
                }
            }
        }
        return true;
    }
    bool hasWordNeighborsWithGreaterLevel(Node *node) {
        IsWord isWord;
        int level = node->data(LEVEL).toInt();
        foreach (Node *neighbor, node->neighbors()) {
            if (neighbor->data(LEVEL).toInt() > level && isWord(neighbor)) return true;
        }
        return false;
    }
};


class WordFactory
{
public:    
    Node * operator() (const QString &id, WordGraph *graph) {
        return new WordGraphicsNode(id, graph);
    }
};
 
class MeaningFactory
{
public:     
    Node * operator() (const QString &id, WordGraph *graph) {
        return new MeaningGraphicsNode(id, graph);
    }
};

class EdgeFactory
{
public:    
    Edge * operator() (const QString &id, Node *source, Node *dest,  WordGraph *graph) {
        return new GraphicsEdge(id, source, dest, graph);
    }
};
        
                    

#endif
        

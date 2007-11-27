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
#include "worddatagraph.h"
#include "graphnode.h"
#include "graphedge.h"

      
WordGraph::WordGraph(QObject *parent)
    : QObject(parent)
{
}


WordGraph::~WordGraph()
{
    clearAll();
}


Node *WordGraph::addNode(const QString &nodeId, NodeFactory &factory)
{
    if (m_nodes.contains(nodeId))
        return m_nodes[nodeId];
    
    Node *node = factory.createNode(nodeId, this);
    m_nodes[nodeId] = node;
    emit nodeAdded(node);
    return node;
}

Edge *WordGraph::addEdge(const QString &aNodeId, const QString &bNodeId, EdgeFactory &factory)
{
    QString edgeId = aNodeId + "-" +  bNodeId;
    QString edgeId2 = bNodeId + "-" + aNodeId;
    bool hasEdge = m_edges.contains(edgeId) || m_edges.contains(edgeId2);
    bool hasNodes = m_nodes.contains(aNodeId) && m_nodes.contains(bNodeId);
    Edge *edge = 0;
    if (!hasEdge && hasNodes) {
        edge = factory.createEdge(edgeId, m_nodes[aNodeId], m_nodes[bNodeId], this);
        m_edges[edgeId] = edge;
        emit edgeAdded(edge);
    }
    return edge;
}



void WordGraph::disableNode(Node *node)
{
    if (!node || !m_nodes.contains(node->id()))
        return;
    
    m_nodes.remove(node->id());
    m_disabledNodes[node->id()] = node;
    
    QSet<Edge*>::iterator iter = node->m_edges.begin();
    for (; iter != node->m_edges.end(); ++iter) {
        Edge *edge = *iter;
        edge->adjacentNode(node)->m_edges.remove(edge);
    }
}

void WordGraph::enableNode(Node *node)
{
    if (!node || !m_disabledNodes.contains(node->id()))
        return;
    
    m_disabledNodes.remove(node->id());
    m_nodes[node->id()] = node;
    
    QSet<Edge*>::iterator iter = node->m_edges.begin();
    for (; iter != node->m_edges.end(); ++iter) {
        Edge *edge = *iter;
        edge->adjacentNode(node)->m_edges.insert(edge);
    }
}

void WordGraph::enableAll()
{
    NodeIterator iter = m_disabledNodes.begin();
    while (iter != m_disabledNodes.end()) {
        Node *node = *iter;
        iter = m_disabledNodes.erase(iter);
        m_nodes[node->id()] = node;
        
        QSet<Edge*>::iterator iter = node->m_edges.begin();
        for (; iter != node->m_edges.end(); ++iter) {
            Edge *edge = *iter;
            edge->adjacentNode(node)->m_edges.insert(edge);
        }   
    }
}

void WordGraph::removeNode(const QString &nodeId)
{
    if (m_nodes.contains(nodeId)) {
        Node *node = m_nodes[nodeId];
        foreach (Edge *edge, node->edges()) {                
            Node *neighbor = edge->adjacentNode(node);
            if (!neighbor) continue;
            
            node->m_edges.remove(edge);
            neighbor->m_edges.remove(edge);
            
            m_edges.remove(edge->id());
            delete edge;
            
        /*    if (!isReachable(neighbor, centralNode())) {
                removeNode(neighbor->id());
           } */
        }
        m_nodes.remove(nodeId);
        delete node;
        emit nodeRemoved(nodeId);
    }
}


void WordGraph::removeEdge(Edge *edge)
{
    Q_ASSERT(edge != 0);
    removeEdge(edge->id());
}

void WordGraph::removeEdge(const QString &id)
{
    if (!m_edges.contains(id))
        return;
    
    Edge *edge = m_edges[id];
    Node *source = edge->source();
    Node *dest = edge->dest(); 
    source->m_edges.remove(edge);
    dest->m_edges.remove(edge); 
    
    m_edges.remove(id);
    delete edge;
//     if (!isReachable(source, centralNode())) {
//         removeNode(source->id());
//     }
//     if (!isReachable(dest, centralNode())) {
//         removeNode(dest->id());
//     }
}

void WordGraph::clearAll()
{
    NodeIterator nodeIter = m_nodes.begin();
    for (; nodeIter != m_nodes.end(); ++nodeIter)
        delete (*nodeIter);

    EdgeIterator edgeIter = m_edges.begin();
    for (; edgeIter != m_edges.end(); ++edgeIter)
        delete (*edgeIter);

    nodeIter = m_disabledNodes.begin();
    for (; nodeIter != m_disabledNodes.end(); ++nodeIter)
        delete (*nodeIter);
    
    m_nodes.clear();
    m_edges.clear();
    m_disabledNodes.clear();
}



QList<Node*> WordGraph::nodes() const
{
    ConstNodeIterator iter = m_nodes.constBegin();
    QList<Node*> nodes;
    for (; iter != m_nodes.constEnd(); ++iter) {
        if (!m_disabledNodes.contains((*iter)->id())) {
            nodes.append(*iter);
        }
    }
    return nodes;
}
        


QList<Edge*> WordGraph::edges() const
{
    ConstEdgeIterator iter;
    ConstEdgeIterator end = m_edges.constEnd();
    QList<Edge*> edges;
    for (iter = m_edges.constBegin(); iter != end; ++iter) {
        Edge *edge = *iter;
        if (isEnabled(edge)) {
            edges.append(edge);
        }
    }
    return edges;
}

Node* WordGraph::node(const QString &id) const
{
    return m_nodes[id];
}

Edge* WordGraph::edge(const QString &id) const
{
    return m_edges[id];
}

Node* WordGraph::centralNode() const
{
    return node(m_centralNodeId);
}

void WordGraph::setCentralNode(const QString &nodeId)
{
    m_centralNodeId = nodeId;
}

bool WordGraph::isEnabled(Node *node) const
{
    return m_nodes.contains(node->id());
}

bool WordGraph::isEnabled(Edge *edge) const
{
    return isEnabled(edge->source()) && isEnabled(edge->dest());
}







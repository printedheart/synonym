/***************************************************************************
 *   Copyright (C) 2007 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
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
#include "wordgraph.h"
#include "graphnode.h"
#include "graphedge.h"

      
WordGraph::WordGraph(QObject *parent)
    : QObject(parent), m_signalsEnabled(false)
{
}

WordGraph::WordGraph(const WordGraph &o)
    : QObject(o.parent()), m_disabledNodes(o.m_disabledNodes), m_disabledEdges(o.m_disabledEdges),
     m_centralNodeId(o.m_centralNodeId), m_signalsEnabled(false)                
{
    ConstNodeIterator nodeIterEnd = o.m_nodes.constEnd();
    ConstNodeIterator nodeIter;
    for (nodeIter = o.m_nodes.constBegin(); nodeIter != nodeIterEnd; ++nodeIter) {
        m_nodes[nodeIter.key()] = nodeIter.value()->clone();
        m_nodes[nodeIter.key()]->m_graph = this;
    }
    
    ConstEdgeIterator edgeIterEnd = o.m_edges.constEnd();
    ConstEdgeIterator edgeIter;
    for (edgeIter = o.m_edges.constBegin(); edgeIter != edgeIterEnd; ++edgeIter) {
        Edge *cloneEdge = edgeIter.value()->clone();
        cloneEdge->m_graph = this;
        cloneEdge->m_source = m_nodes[cloneEdge->m_source->id()];
        cloneEdge->m_dest = m_nodes[cloneEdge->m_dest->id()];
        m_edges[edgeIter.key()] = cloneEdge;
    }
    
    // Replace each node edges with this graph edges.
    NodeIterator myNodeIterEnd = m_nodes.end();
    for (NodeIterator iter = m_nodes.begin(); iter != myNodeIterEnd; ++iter) {
        QSet<Edge*> newEdges;
        QSet<Edge*> curEdges = iter.value()->edges();
        foreach (Edge *edge, curEdges)
            newEdges << m_edges[edge->id()];
        
        iter.value()->m_edges = newEdges;
    }
}


WordGraph::~WordGraph()
{
    clearAll();
}

WordGraph * WordGraph::clone() const
{
    return new WordGraph(*this);
}


Node *WordGraph::addNode(const QString &nodeId, NodeFactory &factory)
{
    if (m_nodes.contains(nodeId))
        return m_nodes[nodeId];
    
    Node *node = factory.createNode(nodeId, this);
    m_nodes[nodeId] = node;
    signalChange();
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
        signalChange();
    }
    return edge;
}



void WordGraph::disableNode(Node *node)
{
    if (!node || !m_nodes.contains(node->id()))
        return;
    
    m_disabledNodes.insert(node->id());
    
    QSet<Edge*>::iterator iter = node->m_edges.begin();
    for (; iter != node->m_edges.end(); ++iter) {
        Edge *edge = *iter;
        edge->adjacentNode(node)->m_edges.remove(edge);
    }
    signalChange();
}

void WordGraph::enableNode(Node *node)
{
    if (!node || !m_disabledNodes.contains(node->id()))
        return;
    
    m_disabledNodes.remove(node->id());
    
    QSet<Edge*>::iterator iter = node->m_edges.begin();
    for (; iter != node->m_edges.end(); ++iter) {
        Edge *edge = *iter;
        edge->adjacentNode(node)->m_edges.insert(edge);
    }
    signalChange();
}

void WordGraph::disableEdge(Edge *edge)
{
    if (!edge || !m_edges.contains(edge->id()))
        return;
    
    // If the source node or destination node are disabled then
    // this edge is implicitely disabled.
    if (!isEnabled(edge->source()) || !isEnabled(edge->dest()))
         return;
    
    edge->source()->m_edges.remove(edge);
    edge->dest()->m_edges.remove(edge);
    m_disabledEdges.insert(edge->id());
    signalChange();
}

void WordGraph::enableEdge(Edge *edge)
{
    if (!edge || !m_disabledEdges.contains(edge->id())
              || !isEnabled(edge->source())
              || !isEnabled(edge->dest()))
        return;
    
    edge->source()->m_edges.insert(edge);
    edge->dest()->m_edges.insert(edge);
    m_disabledEdges.remove(edge->id());
    signalChange();
}

void WordGraph::enableAll()
{
    QSet<QString>::iterator iter = m_disabledNodes.begin();
    QSet<QString>::iterator disabledIterEnd = m_disabledNodes.end(); 
    while (iter != disabledIterEnd) {
        Node *node = m_nodes[*iter];
        iter = m_disabledNodes.erase(iter);
        
        QSet<Edge*>::iterator iter;
        QSet<Edge*>::iterator nodesIterEnd = node->m_edges.end();
        for (iter = node->m_edges.begin(); iter != nodesIterEnd; ++iter) {
            Edge *edge = *iter;
            edge->adjacentNode(node)->m_edges.insert(edge);
        }   
    }
    foreach (QString edgeId, m_disabledEdges)
        enableEdge(m_edges[edgeId]);
    
    m_disabledEdges.clear();
    signalChange();
}

// void WordGraph::removeNode(const QString &nodeId)
// {
//     if (m_nodes.contains(nodeId)) {
//         Node *node = m_nodes[nodeId];
//         foreach (Edge *edge, node->edges()) {                
//             Node *neighbor = edge->adjacentNode(node);
//             if (!neighbor) continue;
//             
//             node->m_edges.remove(edge);
//             neighbor->m_edges.remove(edge);
//             
//             m_edges.remove(edge->id());
//             delete edge;
//             
//         /*    if (!isReachable(neighbor, centralNode())) {
//                 removeNode(neighbor->id());
//            } */
//         }
//         m_nodes.remove(nodeId);
//         delete node;
//         signalChange();
//     }
// }
// 
// 
// void WordGraph::removeEdge(Edge *edge)
// {
//     Q_ASSERT(edge != 0);
//     removeEdge(edge->id());
//     signalChange();
// }
// 
// void WordGraph::removeEdge(const QString &id)
// {
//     if (!m_edges.contains(id))
//         return;
//     
//     Edge *edge = m_edges[id];
//     Node *source = edge->source();
//     Node *dest = edge->dest(); 
//     source->m_edges.remove(edge);
//     dest->m_edges.remove(edge); 
//     
//     m_edges.remove(id);
//     delete edge;
// //     if (!isReachable(source, centralNode())) {
// //         removeNode(source->id());
// //     }
// //     if (!isReachable(dest, centralNode())) {
// //         removeNode(dest->id());
// //     }
// }

void WordGraph::clearAll()
{
    NodeIterator nodeIterEnd = m_nodes.end();
    for (NodeIterator nodeIter = m_nodes.begin(); nodeIter != nodeIterEnd; ++nodeIter)
        delete (*nodeIter);

    EdgeIterator edgeIterEnd = m_edges.end(); 
    for (EdgeIterator edgeIter = m_edges.begin(); edgeIter != edgeIterEnd; ++edgeIter)
        delete (*edgeIter);

    
    m_nodes.clear();
    m_edges.clear();
    m_disabledNodes.clear();
    signalChange();
}



QList<Node*> WordGraph::nodes() const
{
    ConstNodeIterator iter;
    ConstNodeIterator iterEnd = m_nodes.constEnd();
    QList<Node*> nodes;
    for ( iter = m_nodes.constBegin(); iter != iterEnd; ++iter) {
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
    return !m_disabledNodes.contains(node->id());
}

bool WordGraph::isEnabled(Edge *edge) const
{
    return isEnabled(edge->source()) && isEnabled(edge->dest()) && !m_disabledEdges.contains(edge->id());
}


void WordGraph::signalChange()
{
    if (m_signalsEnabled)
        emit changed();
}

void WordGraph::setEnabledSignals(bool enabled)
{
    m_signalsEnabled = enabled;
}





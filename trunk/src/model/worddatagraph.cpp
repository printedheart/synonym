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

#include "graphalgorithms.h"  
      
WordGraph::WordGraph(QObject *parent)
    : QObject(parent)
{
}


WordGraph::~WordGraph()
{
}

void WordGraph::addNode(Node *node)
{
    qDebug() << "addNode";
    QString nodeId = node->id();
    if (!m_nodes.contains(nodeId)) {
        m_nodes[nodeId] = node;
        if (node->fixed())
            m_centralNodeId = node->id();
        emit nodeAdded(node);
    }
}

void WordGraph::removeNode(const QString &nodeId)
{
    qDebug() << "removeNode " << nodeId;
    if (m_nodes.contains(nodeId)) {
        Node *node = m_nodes[nodeId];
        qDebug() << node;
        foreach (Edge *edge, node->edges()) {   
             
            Node *neighbor = edge->neighborTo(node);
            qDebug() << neighbor;
            if (!neighbor) continue;
            
            for (int i = 0; i < node->m_edges.size(); i++) {
                if (node->m_edges[i] == edge) {
                    node->m_edges.removeAt(i);
                    break;
                }
            }
            for (int i = 0; i < neighbor->m_edges.size(); i++) {
                if (neighbor->m_edges[i] == edge) {
                    neighbor->m_edges.removeAt(i);
                    break;
                }
            }
            qDebug() << "removed edges in node";
            m_edges.remove(edge->id());
            delete edge;
            
            if (!isReachable(neighbor, centralNode())) {
                removeNode(neighbor->id());
            }
        }
        m_nodes.remove(nodeId);
        delete node;
        emit nodeRemoved(nodeId);
    }
}

Edge * WordGraph::addEdge(const QString &aNodeId, const QString &bNodeId)
{
    qDebug() << "addEdge";
    QString edgeId = aNodeId + bNodeId;
    QString edgeId2 = bNodeId + aNodeId;
    qDebug() << edgeId;
    bool hasEdge = m_edges.contains(edgeId) || m_edges.contains(edgeId2);
    bool hasNodes = m_nodes.contains(aNodeId) && m_nodes.contains(bNodeId);
    Edge *edge = 0;
    if (!hasEdge && hasNodes) {
        edge = new Edge(edgeId, m_nodes[aNodeId], m_nodes[bNodeId], this );
        m_edges[edgeId] = edge;
        emit edgeAdded(edge);
    }
    return edge;
}

void WordGraph::removeEdge(const QString &id)
{
    
    qDebug() << "removeEdge " << id;
    Node *source = m_edges[id]->source();
    Node *dest = m_edges[id]->dest();
    QList<Edge*> sourceNodeEdges = source->m_edges;
    QList<Edge*> destNodeEdges = dest->m_edges;
    Edge *edge = m_edges[id]; 
    qDebug() << edge;
            
    for (int i = 0; i < sourceNodeEdges.size(); i++) {
        if (sourceNodeEdges[i]->id() == id) {
            sourceNodeEdges.removeAt(i);
            break;
        }
    }
    
    for (int i = 0; i < destNodeEdges.size(); i++) {
        if (destNodeEdges[i]->id() == id) {
            destNodeEdges.removeAt(i);
            break;
        }
    }
    m_edges.remove(id);
    delete edge;
    if (!isReachable(source, centralNode())) {
        removeNode(source->id());
    }
    if (!isReachable(dest, centralNode())) {
        removeNode(dest->id());
    }
}

void WordGraph::clearAll()
{
    foreach (Node *node, m_nodes)
        delete node;

    foreach (Edge *edge, m_edges)
        delete edge;

    m_nodes.clear();
    m_edges.clear();
}


QList<WordNode*> WordGraph::phrases() const
{
    QList<WordNode*> phrases;
    foreach (Node *node, m_nodes) {
        WordNode *phrase = qobject_cast<WordNode*>(node);
        if (phrase)
            phrases.append(phrase);
    }
    return phrases;
}

QList<MeaningNode *> WordGraph::meanings() const
{
    QList<MeaningNode *> meanings;
    foreach (Node *node, m_nodes) {
        MeaningNode *meaning = qobject_cast<MeaningNode*>(node);
        if (meaning)
            meanings.append(meaning);
    }
    return meanings;
}

QList<Edge*> WordGraph::edges() const
{
    return m_edges.values();
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

QString WordGraph::toString() const
{
    return centralNode()->toString();
}







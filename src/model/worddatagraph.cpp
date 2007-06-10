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

        
WordDataGraph::WordDataGraph(QObject *parent)
    : QObject(parent)
{
}


WordDataGraph::~WordDataGraph()
{
}

void WordDataGraph::addNode(DataNode *node)
{
    QString nodeId = node->id();
    if (!m_nodes.contains(nodeId)) {
        m_nodes[nodeId] = node;
        if (node->fixed())
            m_centralNodeId = node->id();
        emit nodeAdded(node);
    }
}

void WordDataGraph::removeNode(const QString &nodeId)
{
    if (m_nodes.contains(nodeId)) {
        DataNode *node = m_nodes[nodeId];
        delete node;
        m_nodes.remove(nodeId);
        emit nodeRemoved(nodeId);
    }
}

Edge * WordDataGraph::addEdge(const QString &aNodeId, const QString &bNodeId)
{
    QString edgeId = aNodeId + bNodeId;
    QString edgeId2 = bNodeId + aNodeId;
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

void WordDataGraph::clearAll()
{
    foreach (DataNode *node, m_nodes)
        delete node;

    foreach (Edge *edge, m_edges)
        delete edge;

    m_nodes.clear();
    m_edges.clear();
}


QList<PhraseNode*> WordDataGraph::phrases() const
{
    QList<PhraseNode*> phrases;
    foreach (DataNode *node, m_nodes) {
        PhraseNode *phrase = qobject_cast<PhraseNode*>(node);
        if (phrase)
            phrases.append(phrase);
    }
    return phrases;
}

QList<MeaningNode *> WordDataGraph::meanings() const
{
    QList<MeaningNode *> meanings;
    foreach (DataNode *node, m_nodes) {
        MeaningNode *meaning = qobject_cast<MeaningNode*>(node);
        if (meaning)
            meanings.append(meaning);
    }
    return meanings;
}

QList<Edge*> WordDataGraph::edges() const
{
    return m_edges.values();
}

DataNode* WordDataGraph::node(const QString &id) const
{
    return m_nodes[id];
}

Edge* WordDataGraph::edge(const QString &id) const
{
    return m_edges[id];
}

DataNode* WordDataGraph::centralNode() const
{
    return node(m_centralNodeId);
}

QString WordDataGraph::toString() const
{
    centralNode()->toString();
}





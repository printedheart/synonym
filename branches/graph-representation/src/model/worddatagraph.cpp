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
            
            if (!isReachable(neighbor, centralNode())) {
                removeNode(neighbor->id());
            }
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



QList<GraphicsNode*> WordGraph::nodes() const
{
    return m_nodes.values();
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








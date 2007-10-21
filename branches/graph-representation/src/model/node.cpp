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
#include "node.h"
#include "edge.h"

#include "worddatagraph.h"
                            
Node::Node(const QString &id, WordGraph *parent)
    : QObject(parent), m_id(id), m_fixed(false)
{   
}


Node::~ Node()
{
}

void Node::addEdge(Edge *edge)
{
    m_edges << edge;
}

QList<Edge *> Node::edges() const
{
    return m_edges;
}

unsigned int Node::degree() const
{
    return edges().size();
}

QString Node::id() const
{
    return m_id;
}

void Node::setFixed(bool fixed)
{
    m_fixed = fixed;
}

bool Node::fixed() const
{
    return m_fixed;
}

QString Node::toString() const
{
    QString str;
    foreach (Edge *edge, m_edges) {
        str.append(edge->id() + "  " + edge->id() + "  ");
        str.append(edge->source()->id() + "  ");
        str.append(edge->dest()->id() + "\n");
    }
    return str;
}

WordGraph * Node::graph() const
{
    return static_cast<WordGraph*>(parent());
}



// WordNode

WordNode::WordNode(const QString &id, WordGraph *parent)
    :Node(id, parent)
{
}

WordNode::~WordNode()
{
}

QString WordNode::word() const
{
    QString idCopy(id());
    return idCopy.replace(QChar('_'), QChar(' '));
}


// MeaningNode
MeaningNode::MeaningNode(const QString &id, WordGraph *parent)
    :Node(id, parent)
{
}

MeaningNode::~MeaningNode()
{
}

void MeaningNode::setPartOfSpeech(int pos)
{
    m_pos = pos;
}


int MeaningNode::partOfSpeech() const
{
    return m_pos;
}

void MeaningNode::setMeaning(const QString &meaning)
{
    m_meaning = meaning;
}

QString MeaningNode::meaning() const
{
    return m_meaning;
}





    

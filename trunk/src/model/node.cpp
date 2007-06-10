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
                            
DataNode::DataNode(const QString &id, QObject *parent)
    : QObject(parent), m_id(id), m_fixed(false)
{   
}



DataNode::~ DataNode()
{
}

void DataNode::addEdge(Edge *edge)
{
    m_edges << edge;
}

QList<Edge *> DataNode::edges() const
{
    return m_edges;
}

QString DataNode::id() const
{
    return m_id;
}

void DataNode::setFixed(bool fixed)
{
    m_fixed = fixed;
}

bool DataNode::fixed() const
{
    return m_fixed;
}

QString DataNode::toString() const
{
    QString str;
    foreach (Edge *edge, m_edges) {
        str.append(edge->id() + "  " + edge->id() + "  ");
        str.append(edge->sourceNode()->id() + "  ");
        str.append(edge->destNode()->id() + "\n");
    }
    return str;
}



// PhraseNode


PhraseNode::PhraseNode(const QString &id, QObject *parent)
    :DataNode(id, parent)
{
}

PhraseNode::~PhraseNode()
{
}

QString PhraseNode::phrase() const
{
    return DataNode::id();
}


// MeaningNode
MeaningNode::MeaningNode(const QString &id, QObject *parent)
    :DataNode(id, parent)
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





    
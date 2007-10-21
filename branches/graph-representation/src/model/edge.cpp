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
#include "edge.h"
#include "node.h"
#include <QPointF>


const QString Relationship::descriptions[22] = 
{ "Antonym",   "Is a",          "Hyponym",
              "Entails",   "Is similar to", "Is member of",
              "Is made of", "Is part of",   "Member",
              "Has stuff",  "Has part",      "Meronym",
              "Holonym"  ,  "Cause",         "Participle",
              "Also see",  "Pertains to nound", "Attribute",
              "Verb group", "Derivation", "Domain",
              "Class"};

                    

Edge::Edge(const QString &id, Node *sourceNode, Node *destNode, QObject *parent)
    :QObject(parent), m_id(id), m_source(sourceNode), m_dest(destNode)
{
    m_source->addEdge(this);
    m_dest->addEdge(this);
}




Edge::~Edge()
{
}

Node *Edge::source() const
{
    return m_source;
}

void Edge::setSource(Node *node)
{
    m_source = node;
}

Node *Edge::dest() const
{
    return m_dest;
}

void Edge::setDest(Node *node)
{
    m_dest = node;
}

QString Edge::id() const
{
    return m_id;
}

int Edge::relationship() const
{
    return m_relationship;
}

void Edge::setRelationship(int relationship)
{
    m_relationship = relationship;
}

Node * Edge::neighborTo(Node *to) const
{
    if (to == source())
        return dest();
    if (to == dest())
        return source();
    return 0;
}




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

#include "wordgraphdecorator.h"
#include "wordnetutil.h"


WordGraphDecorator::WordGraphDecorator()
    :m_wordGraph(0)
{}

WordGraphDecorator::~WordGraphDecorator()
{}


void WordGraphDecorator::setWordNodeFactory(NodeFactory *nodeFactory)
{
    m_wordNodeFactory = nodeFactory;
}

void WordGraphDecorator::setMeaningNodeFactory(NodeFactory *nodeFactory)
{
    m_meaningNodeFactory = nodeFactory;
}
    
void WordGraphDecorator::setEdgeFactory(EdgeFactory *edgeFactory)
{
    m_edgeFactory = edgeFactory;
}
    
void WordGraphDecorator::setRelationshipTypes(Relationship::Types relationshipTypes)
{
    m_relationshipTypes = relationshipTypes;
}
 
WordGraph * WordGraphDecorator::wordGraph() const
{
    return m_wordGraph;
}

WordGraph * WordGraphDecorator::new_WordGraph()
{
    qDebug() << "new_WordGraph()";
    m_wordGraph = new WordGraph();
    return m_wordGraph;
}


void WordGraphDecorator::addWordNode(WordGraph *wordGraph, QString word)
{
//    qDebug() << "addWordNode(" << word << ")"; 
    Node *wordNode = wordGraph->addNode(word, *m_wordNodeFactory);
    wordNode->setData(WORD, QString(word).replace(QChar('_'), QChar(' ')));
}


static int getpos(QString &pos)
{
    if (pos == "noun")
        return Noun;
    else if (pos == "verb")
        return Verb;
    else if (pos == "adj")
        return Adjective;
    else if (pos == "adv")
        return Adverb;
    return 0;
}

static Relationship::Type toRelationshipType(QString &pointerType)
{
    QString t = pointerType;
    if (t == "antonym") return Relationship::Antonym;
    if (t == "hyponym") return Relationship::Hyponym;
    if (t == "hypernym") return Relationship::Hypernym;
    if (t == "entailment") return Relationship::Entailment;
    if (t == "similar") return Relationship::Similar;
    if (t == "member meronym") return Relationship::IsMember;
    if (t == "substance meronym") return Relationship::IsStuff;
    if (t == "part meronym") return Relationship::IsPart;
    if (t == "member holonym") return Relationship::HasMember;
    if (t == "substance holonym") return Relationship::HasStuff;
    if (t == "part holonym") return Relationship::HasPart;
    if (t == "cause") return Relationship::Cause;
    if (t == "participle of") return Relationship::Participle;
    if (t == "also see") return Relationship::SeeAlso;
    if (t == "pertainym") return Relationship::Pertains;
    if (t == "attribute") return Relationship::Attribute;
    if (t == "verb group") return Relationship::VerbGroup;
    return Relationship::Undefined;
}
    
void WordGraphDecorator::addMeaningNode(WordGraph *wordGraph, QString id, QString meaning, QString pos)
{
 //   qDebug() << "addMeaningNode(" << id << ", " << meaning << ")";
    Node *meaningNode = wordGraph->addNode(id, *m_meaningNodeFactory);
    meaningNode->setData(POS, getpos(pos));
    meaningNode->setData(MEANING, meaning);
}
     
bool WordGraphDecorator::addEdge(WordGraph *wordGraph, QString id1, QString id2, QString pointerType)
{
    //qDebug() << "addEdge(" << id1 << ",  " << id2 << ", " << pointerType << ")";
    bool success = false;
    if (pointerType != QString()) {
        Relationship::Type type = toRelationshipType(pointerType); 
        if ( type != Relationship::Undefined) {
           Edge *edge = wordGraph->addEdge(id1, id2, *m_edgeFactory);
           if (edge) {
              edge->setRelationship(type);
              success = true;
           }
        }
    } else {
        success =  wordGraph->addEdge(id1, id2, *m_edgeFactory);
    }
 //   qDebug() << success;
    return success;
}


void WordGraphDecorator::setCentralNode(WordGraph *wordGraph, QString nodeId)
{
    wordGraph->setCentralNode(nodeId);
}
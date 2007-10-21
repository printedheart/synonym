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

#include <QtCore>
#include "node.h"
#include "edge.h"
/**
        @author Sergejs <sergey.melderis@gmail.com>
 */







class WordGraph : public QObject
{
Q_OBJECT
public:
    WordGraph(QObject *parent = 0);
    ~WordGraph();

    enum PartOfSpeech { Noun = 1, Verb = 2, Adjective = 3, Adverb = 4 };
    
    void addNode(Node *node);
    void removeNode(const QString &nodeId);
    Edge * addEdge(const QString &aNodeId, const QString &bNodeId);
    void removeEdge(const QString &edgeId);

    void clearAll();


    QList<WordNode *> phrases() const;
    QList<MeaningNode *> meanings() const;

    Node* node(const QString &id) const;
    Edge *edge(const QString &id) const;

    QList<Edge*> edges() const;

    Node* centralNode() const;

    QString toString() const;
    
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

#endif
        

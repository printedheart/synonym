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


#include <QObject>
#include <QHash>
#include <QList>
#include "node.h"
#include "edge.h"
/**
        @author Sergejs <sergey.melderis@gmail.com>
 */
class WordDataGraph : public QObject
{
Q_OBJECT
public:
    WordDataGraph(QObject *parent = 0);
    ~WordDataGraph();

    void addNode(DataNode *node);
    void removeNode(const QString &nodeId);
    Edge * addEdge(const QString &aNodeId, const QString &bNodeId);
//void removeEdge(const QString &edgeId);

    void clearAll();


    QList<PhraseNode *> phrases() const;
    QList<MeaningNode *> meanings() const;

    DataNode* node(const QString &id) const;
    Edge *edge(const QString &id) const;

    QList<Edge*> edges() const;

    DataNode* centralNode() const;

    QString toString() const;
signals:
    void nodeAdded(DataNode *node);
    void nodeRemoved(const QString &nodeId);
    void edgeAdded(Edge *edge);
private:
    QHash<QString, DataNode*> m_nodes;
    QHash<QString, Edge*> m_edges;
    QString m_centralNodeId;
};

#endif
        

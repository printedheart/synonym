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
#ifndef NOUNLISTMODEL_H
#define NOUNLISTMODEL_H

#include "relationship.h"
#include "worddatagraph.h"

#include <QAbstractListModel>
#include <QStringList>


/**
	@author Sergejs <sergey.melderis@gmail.com>
*/
class PartOfSpeechListModel : public QAbstractListModel
{
Q_OBJECT
public:
    PartOfSpeechListModel(PartOfSpeech modelType, QObject *parent = 0);

    ~PartOfSpeechListModel();

   // QModelIndex index(int row, int row, const QModelIndex& parent) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    void setDataGraph(WordGraph *dataGraph);


    Node *nodeAt(const QModelIndex &index);
    QModelIndex indexForNode(Node *node);
    
    PartOfSpeech modelType() const;
private:
    WordGraph *m_dataGraph;

    QList<Node*> m_meanings;

    PartOfSpeech m_modelType;
    QStringList stringList;
private slots:
    void reload();
    
};

#endif

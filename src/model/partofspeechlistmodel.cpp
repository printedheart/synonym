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
#include "partofspeechlistmodel.h"
#include "worddatagraph.h"
#include "wordnetutil.h"
#include "graphalgorithms.h"

#include <QtCore>

PartOfSpeechListModel::PartOfSpeechListModel(PartOfSpeech modelType, QObject *parent) :
        QAbstractListModel(parent),m_dataGraph(0), m_modelType(modelType) 
{
}


PartOfSpeechListModel::~PartOfSpeechListModel()
{
}

void PartOfSpeechListModel::setDataGraph(WordGraph *dataGraph)
{
    if (m_dataGraph && m_dataGraph != dataGraph) {
        disconnect(m_dataGraph, SIGNAL(nodeAdded(Node *)),
                this, SLOT(reload()));
        disconnect(m_dataGraph, SIGNAL(nodeRemoved(const QString &)),
                this, SLOT(reload()));
    }

    if (m_dataGraph == 0 || m_dataGraph != dataGraph) {
        m_dataGraph = dataGraph;
    
        connect(m_dataGraph, SIGNAL(nodeAdded(Node *)),
                this, SLOT(reload()));
        connect(m_dataGraph, SIGNAL(nodeRemoved(const QString &)),
                this, SLOT(reload()));
    }
    reload();
    
}
int PartOfSpeechListModel::rowCount(const QModelIndex &parent) const
{
    return m_meanings.size();
}

QVariant PartOfSpeechListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole) {
        QString fullMeaning =  m_meanings.at(index.row())->data(MEANING).toString();
        QString definition = fullMeaning.section(';', 0, 0);
        definition.remove(0, 1);
        return definition;
    }
    else
        return QVariant();
}

void PartOfSpeechListModel::reload()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    m_meanings.clear();
    endRemoveRows();

    beginInsertRows(QModelIndex(), 0, 0);
    QList<Node *> meanings = m_dataGraph->nodes(IsMeaning());
    foreach (Node *node, meanings) {
        if (node->data(POS).toInt() == m_modelType)
            m_meanings.append(node);
    }
    endInsertRows();
}


 QVariant PartOfSpeechListModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

Node * PartOfSpeechListModel::nodeAt(const QModelIndex &index)
{
    if (!index.isValid())
        return 0;

    if (index.row() >= rowCount())
        return 0;
    
    return m_meanings.at(index.row());
}

QModelIndex PartOfSpeechListModel::indexForNode(Node *node)
{
    int index = m_meanings.indexOf(node);
    QModelIndex modelIndex;
    if (index != -1) {
        modelIndex = createIndex(index, 0);
    }
    return modelIndex;
}

        
PartOfSpeech PartOfSpeechListModel::modelType() const
{
    return m_modelType;
}

    


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
#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include "wordgraph.h"
#include "relationship.h"

#include <QObject>
#include <QPair>
#include <QList>

class GraphScene;
class WordDataLoader;
class MeaningGraphicsNode;
class WordGraphicsNode;
class GraphicsEdge;
class GraphicsNode;
class QGraphicsItem;                     
/**
    @author Sergejs <sergey.melderis@gmail.com>
*/

class GraphController : public QObject
{
Q_OBJECT
public:
    GraphController(GraphScene *graphScene, WordDataLoader *loader,
                    QObject *parent = 0);

    ~GraphController();


    WordGraph* makeGraph(const QString &word);
    
    WordGraph * previousGraph();
    WordGraph * nextGraph();
    
    void setPoses(QList<PartOfSpeech> &poses);
    
    void setRelationships(Relationship::Types relationships);
public slots:
    void soundReady(const QString &word);
    

    
private:
    GraphScene *m_scene;
    WordDataLoader *m_loader;
    WordGraph *m_graph;

    QList<PartOfSpeech> m_poses;
    Relationship::Types m_relTypes;

    void makeConnected(Node *goal);
    
    void updateSceneNodes();
    
    
    GraphicsNode * findGraphicsNode(Node *dataNode);
    
    void addEdge(GraphicsNode *graphNode, Edge *edge);
    
    QList<WordGraph*> m_backHistory;
    QList<WordGraph*> m_forwardHistory;
    
    void filterGraphNodes();
    
    void assertGraphConnectivityToNode(Node *node);
    
    
};

#endif

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
#ifndef GRAPHSCENE_H
#define GRAPHSCENE_H

#include <QGraphicsScene>
#include <QList>
        
class Layout;
class GraphNode;
class GraphEdge;
class PhraseGraphNode;
class DataNode;
class QSvgRenderer;
/**
	@author Sergejs <sergey.melderis@gmail.com>
*/

class GraphScene : public QGraphicsScene
{
Q_OBJECT
public:
    GraphScene(QObject *parent = 0);

    ~GraphScene();

    void itemMoved();

    QList<GraphNode *> graphNodes() const;

    void setCalculate(bool calculate);

    void propogateClickEvent(PhraseGraphNode *graphNode);

    void doInitialLayout(GraphNode *rootNode);

    void displaySoundIcon();

    void setCentralNode(GraphNode *node);
    GraphNode *centralNode() const;
    
signals:
    void nodeClicked(const QString &phrase);

    void soundButtonClicked(const QString &phrase);
protected:
    void timerEvent(QTimerEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void calculateForces();
    void doSpringAlgorithm(QList<GraphNode*> nodes, QList<GraphEdge*> edges);
       
private:
    int m_timerId;
    int m_timerInterval;

    bool m_calculate;

    GraphNode *m_centralNode;
    QSvgRenderer *m_soundIconRenderer;

    Layout *m_layout;
    void layoutNodes(GraphNode *node, GraphNode *parentNode);
    
    bool m_restartLayout;
};

#endif

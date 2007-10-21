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
#include <QSet>        
class Layout;
class GraphicsNode;
class GraphEdge;
class WordGraphicsNode;
class Node;
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

    QList<GraphicsNode *> graphNodes() const;

    void setLayout(bool enable);

    void propogateClickEvent(GraphicsNode *graphNode);
    
    void signalMouseHovered(GraphicsNode *graphNode);
    void signalMouseHoverLeaved(GraphicsNode *graphNode);

    void displaySoundIcon();

    void setCentralNode(GraphicsNode *node);
    GraphicsNode *centralNode() const;
public slots:    
    void setActivated(const QString &id);
    
signals:
    void nodeClicked(const QString &id);

    void nodeMouseHovered(const QString &id);
    void nodeMouseHoverLeaved(const QString &id);
    
    void soundButtonClicked(const QString &phrase);
protected:
    void timerEvent(QTimerEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void layout();
       
private:
    int m_timerId;
    int m_timerInterval;

    bool m_enableLayout;

    GraphicsNode *m_centralNode;
    QSvgRenderer *m_soundIconRenderer;

    Layout *m_layout;
    void layoutNodes(GraphicsNode *node, GraphicsNode *parentNode, QSet<GraphicsNode*> &visitSet);
    
    bool m_restartLayout;    
    GraphicsNode *m_activeNode;
};

#endif

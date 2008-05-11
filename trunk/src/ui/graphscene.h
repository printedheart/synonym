/***************************************************************************
 *   Copyright (C) 2007 by Sergejs Melderis                                *
 *   sergey.melderis@gmail.com                                             *
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
class GraphicsEdge;
class WordGraphicsNode;
class QSvgRenderer;
/**
    @author Sergejs <sergey.melderis@gmail.com>
*/

class GraphScene : public QGraphicsScene
{
Q_OBJECT
public:
    GraphScene(Layout *layout, QObject *parent = 0);

    ~GraphScene();

    

    QList<GraphicsNode*> graphNodes();
    QList<GraphicsEdge*> graphEdges();
    
    //  QGraphicsItem(s) are not QObject(s),and cannot emit signals.
    //  Using the following methods items can notify interested parties about events
    //  through signals and slots mechanism. We could derive GraphicsNode from QObject
    //  for that, but we would have to connect/disconnect nodes when we add and remove them.
    
    /**
     * Emits nodeClicked(const QString &id) signal.
     */
    void signalClickEvent(GraphicsNode *graphNode);
    
    /**
     * Emits nodeMouseHovered(const QString &id) signal.
     */
    void signalMouseHovered(GraphicsNode *graphNode);
    
    /**
     * Emits nodeMouseHoverLeaved(const QString &id) signal.
     */
    void signalMouseHoverLeaved(GraphicsNode *graphNode);

    void displaySoundIcon();
    
    void setCentralNode(GraphicsNode *node);
    GraphicsNode *centralNode() const;
    
    
     /* addItem() and removeItem() are not virtual in 
     *  QGraphicsScene. GraphScene must be used directly and not
     * through the QGraphicsScene. We "override" them to manage the cache
     * of nodes and edges. 
     */
    void addItem(QGraphicsItem *item);
    void removeItem(QGraphicsItem *item);
    
public slots:    
    /**
     * Mark the node with id as active node.
     */
    void setActivated(const QString &id);
    
    /**
     * If enable is true, the scene will schedule layouting of the nodes.
     */
    void setLayout(bool enable = true);
    
    void itemMoved();
signals:
    void nodeClicked(const QString &id);
    void nodeMouseHovered(const QString &id);
    void nodeMouseHoverLeaved(const QString &id);
    
    void soundButtonClicked();
protected:
    void timerEvent(QTimerEvent *event);
  //  void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    void layout();
       
    
    
private:
    Layout *m_layout;
    
    int m_timerId;
    int m_timerInterval;

    bool m_enableLayout;
    bool m_restartLayout;    
    
    GraphicsNode *m_centralNode;
    QSvgRenderer *m_soundIconRenderer;

    
    
    
    GraphicsNode *m_activeNode;
    
    // Nodes and edges we return in nodes() and edges()
    QList<GraphicsNode*> m_nodes;
    QList<GraphicsEdge*> m_edges;
    
    void adjustEdges();
    
    QPointF m_mousePos;
    QPointF m_mouseLayoutPos;
};

#endif


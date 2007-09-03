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
#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <QGraphicsItem>
#include <QList>
#include <QFont>
#include <QVector>
#include <QMutex>
     
#include <QtDebug>       
class GraphEdge;
class DataNode;
class MeaningNode;
class PhraseNode;
class GraphScene;
class QGraphicsSceneMouseEvent;
class QPointF;




class LayoutPath {
    public:
        void addPoint(QPointF p) {
            buffer[lastIndex++] = p;
        }
        QPointF  lastPoint()  {
            return buffer[(lastIndex - 1)]; }
        QPointF  takeFirstPoint() { 
            return  buffer[firstIndex++]; }
        void reset() { firstIndex = 0; lastIndex = 0;}
        int pointCount() const { return lastIndex - firstIndex; } 
    private:
        static const  uint BUFFER_SIZE = 8192;
        QPointF buffer[BUFFER_SIZE];
        volatile uint firstIndex;
        volatile uint lastIndex;
};    


/**
	@author Sergejs <sergey.melderis@gmail.com>
*/
class GraphNode : public QGraphicsItem
{
public:
    GraphNode(GraphScene * scene);
    virtual ~GraphNode();

    void addEdge(GraphEdge *edge) { m_edges.append(edge); }
    QList<GraphEdge*> edges() const { return m_edges; }

    QList<GraphNode*> neighbors() const;
    
    QString id() const;

    virtual void setDataNode(DataNode *dataNode) = 0;
    virtual DataNode *dataNode() const = 0;
    
    // Force  and position calculation
    inline QPointF force() const { return m_force; }
    inline void applyForce(qreal fx, qreal fy) { m_force += QPointF(fx, fy); }
    void discardForce();

    void setMass(double mass) { m_mass = mass; }
    inline double mass() const { return m_mass; }
    virtual bool advance();
    void setNewPos(QPointF newPos);
        
    
    
    // Types for QGraphicsScene::qgraphicsitem_cast()
    enum GraphTypes { GraphType = UserType + 90, PhraseType, MeaningType };
    enum { Type = UserType + 90 };
    int type() const { return Type; }

    
    
    void addAnimationPos(const QPointF &point);
    int animationPosCount();
    void resetAnimation();
    inline QPointF & lastAnimationPos() { return m_buffer[m_lastIndex - 1]; }
    QPointF   takeFirstAnimationPos();
    

    bool visited() const { return m_visited; }
    void visit() { m_visited = true; }
    void resetVisit() { m_visited = false; }

    // Painting
    QPainterPath shape() const = 0; 
    virtual QRectF boundingRect() const = 0;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) = 0; 
    
    virtual void setActivated(bool /* activated */ ) {} ;
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    
    QPointF m_newPos;
    QPointF m_intermedPos;
    GraphScene *m_scene;
private:
    double m_mass;
    QList<GraphEdge*> m_edges;

    QPointF m_force;

    bool m_visited;

    QList<QPointF> m_animationPoints;
    QMutex m_pointsMutex;
    
    QPointF m_buffer[10000]; 
    uint m_firstIndex;
    uint m_lastIndex;
    
    QPointF m_lastPoint;
    
};


class PhraseGraphNode : public GraphNode
{
public:
    PhraseGraphNode(GraphScene *scene);
    ~PhraseGraphNode();

    enum { Type = UserType + 91 };
    int type() const { return Type; }
    
    void setDataNode(DataNode *dataNode);
    DataNode *dataNode() const;
    
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    bool advance();

    void showSoundButton();
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
    PhraseNode *m_phraseNode;
    QFont m_font;
    QPointF m_mousePressPos;


    
};

class MeaningGraphNode : public GraphNode
{
public:
    MeaningGraphNode(GraphScene *scene);
    ~MeaningGraphNode();

    void setDataNode(DataNode *dataNode);
    DataNode *dataNode() const;
    
    enum { Type = UserType + 92 };
    int type() const { return Type; }
    QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    
    void setActivated(bool activated);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    MeaningNode *m_node;

    QGraphicsPathItem *m_toolTip;
    QGraphicsTextItem *m_defItem;

    void createToolTip();
    void showToolTip(const QPointF &pos);
    void hideToolTip();
};


#endif

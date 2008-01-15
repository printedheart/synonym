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
#include <QSet>
#include <QFont>
#include <QVector>
#include <QMutex>
     
#include <QtDebug>       
class GraphicsEdge;
class GraphScene;
class QGraphicsSceneMouseEvent;
class QPointF;
class WordGraph;



/**
    @author Sergejs <sergey.melderis@gmail.com>
*/
class GraphicsNode : public QGraphicsItem
{
public:
    GraphicsNode(const QString &id, WordGraph *graph);
    GraphicsNode(const GraphicsNode &o);
    virtual ~GraphicsNode();
    
    virtual GraphicsNode *clone() const = 0;
    
    friend class WordGraph;
    friend class GraphicsEdge;
    
    QString id() const;
    
    QSet<GraphicsEdge*> edges() const;

    QSet<GraphicsNode*> neighbors() const;
    
    QSet<GraphicsNode*> outNeighbors() const;
    
    unsigned int degree() const;
    
    WordGraph * graph() const;
    
    QString toString();
    
    
    void setMass(double mass) { m_mass = mass; }
    inline double mass() const { return m_mass; }
    
    virtual bool advance();
    void setNewPos(QPointF newPos);
    
    // Types for QGraphicsScene::qgraphicsitem_cast()
    enum GraphTypes { GraphType = UserType + 90, PhraseType, MeaningType };
    enum { Type = UserType + 90 };
    int type() const { return Type; }

    // Painting
    QPainterPath shape() const = 0; 
    virtual QRectF boundingRect() const = 0;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) = 0; 
    
    virtual void setActivated(bool /* activated */ ) {} ;
    
    GraphScene *graphScene() const;
    
protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    
    
private:    
    QPointF m_newPos;
    QPointF m_intermedPos;
    
    
    
    QString m_id;
    WordGraph *m_graph;
    
    double m_mass;
    
    QSet<GraphicsEdge*> m_edges;
};


class WordGraphicsNode : public GraphicsNode
{
public:
    WordGraphicsNode(const QString &id, WordGraph *graph);
    WordGraphicsNode(const WordGraphicsNode &o);
    virtual ~WordGraphicsNode();
    
    virtual WordGraphicsNode *clone() const;

    int type() const { return PhraseType; }
    
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

    bool advance();

    void showSoundButton();
    
    static void setFont(QFont font);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
private:
    static QFont s_font;
    QFont m_font;
    QPointF m_mousePressPos;
    
    void calculateBoundingRect();
    
    QRectF m_rectf;
    QPointF m_prevPos;
};

class MeaningGraphicsNode : public GraphicsNode
{
public:
    MeaningGraphicsNode(const QString &id, WordGraph *graph);
    MeaningGraphicsNode(const MeaningGraphicsNode &other);
    virtual ~MeaningGraphicsNode();
    
    virtual MeaningGraphicsNode *clone() const;
    
    static void setRadius(int radius) { s_radius = radius; }
    
    
    int type() const { return MeaningType; }
    QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    
    void setActivated(bool activated);
protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QGraphicsPathItem *m_toolTip;
    QGraphicsTextItem *m_defItem;
    QGraphicsLineItem *m_pointer;
    
    void createToolTip();
    void showToolTip(const QPointF &pos);
    void hideToolTip();
    
    static int s_radius;
    int m_radius;
    QRectF m_boundingRect;
    void calculateBoundingRect();
};


#endif

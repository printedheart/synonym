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
#ifndef GRAPHEDGE_H
#define GRAPHEDGE_H

#include "relation.h"
#include <QGraphicsItem>
#include <QPen>

class GraphicsNode;
class WordGraph;
/**
    @author Sergejs Melderis <sergey.melderis@gmail.com>
*/



class GraphicsEdge : public QGraphicsItem
{
public:
    GraphicsEdge(const QString &id, GraphicsNode *source, 
                 GraphicsNode *dest, WordGraph *graph);
    
    GraphicsEdge(const GraphicsEdge &o);
    
    /**
     * Creates a clone of the given edge.
     */ 
    GraphicsEdge * clone() const;
    
    virtual ~GraphicsEdge();
    
    friend class WordGraph;
    
    /* Edge */
    QString id() const;
    
    WordGraph *graph() const;
    
    GraphicsNode* source() const;
    GraphicsNode* dest() const;
    
    /**
     * Returns a node adjacent to the given node, 
     * or null if node is not either source od destination.
     */
    GraphicsNode* adjacentNode(GraphicsNode *node) const;
    
    //bool directed() const;
    
    void setrelation(Relation::Type type);
    Relation::Type relation() const;
    
    
    /* QGraphcisItem.  Painting. */
    enum { Type = UserType + 111 };
    int type() const { return Type; }
    
    /**
     * Adjusts edge position based on the position of source and destination nodes.
     */
    void adjust();
    
    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    
    void setToolTip(const QString &toolTip);
    void setPen(QPen pan);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
private:
    /* Graph */
    QString m_id;
    WordGraph *m_graph;
    GraphicsNode *m_source, *m_dest;
    bool m_directed;
    
    /* WordNet */
    Relation::Type m_type;
    
    /* Representation */
    QPointF m_sourcePoint, m_destPoint;
    QPen m_pen;
    
    /* Tooltips */
    QGraphicsPathItem *m_toolTip;
    QGraphicsLineItem *m_pointer;
    QGraphicsEllipseItem *m_connector;
    QString m_toolTipString;
    void createToolTip();
    void adjustToolTipPos();
};

#endif

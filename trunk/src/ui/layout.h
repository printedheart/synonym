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
#ifndef LAYOUT_H
#define LAYOUT_H

#include "graphnode.h"
#include "graphedge.h"
#include "graphscene.h"

#include <cmath>
#include <QList>
#include <QThread>



/**
    @author Sergey Melderis <sergey.melderis@gmail.com>
*/
class Layout
{
public:
    Layout() {};

    virtual ~Layout() {};

    /**
     * Layouts nodes and edges.
     * restart indicates that layout should be restarted.
     */
    virtual bool layout(QList<GraphicsNode*> &nodes, QList<GraphicsEdge*> &edges, bool restart) = 0;
    
    virtual void stop() {}
    
    virtual void setRestDistance(int distance) { m_restDistance = distance; }
    
protected:    
    int m_restDistance;
};



 

#include <QMutex>
#include <QWaitCondition>                 

 
class ForceDirectedLayout : public QThread, public Layout
{
Q_OBJECT
public:
    ForceDirectedLayout(QObject *parent = 0);
    ~ForceDirectedLayout();

    // TODO: make the following parameters configurable.
    static const qreal STIFFNESS = 0.2;
    static const qreal REPULSION = 3000;

    bool layout(QList<GraphicsNode*> &nodes, QList<GraphicsEdge*> &edges, bool restart = false);
    
    void run();
    void stop();    
private slots:
    
    void wakeUp();
private:
    void preLayout(GraphicsNode *rootNode);
    void layoutNodes(GraphicsNode *node, QSet<GraphicsNode*> &visitSet);
    
    bool layoutParallel(QList<GraphicsNode*> &nodes, QList<GraphicsEdge*> &edges);
    bool layoutSerial(QList<GraphicsNode*> &nodes, QList<GraphicsEdge*> &edges);
    
    void startThread();
    
    volatile bool finishedLayout;
    bool firstRemoved;
    
    volatile bool m_abort;
    QWaitCondition m_aborted;
    QMutex m_abortMutex;    
    
    QMutex m_mutex;
    
    QList<GraphicsEdge*> m_edges;
    GraphicsNode *centralNode;
    
    //Internal structure to hold node animation points.
    struct NodeAnimation {
        NodeAnimation() : firstIndex(0), lastIndex(0) {}
        static const  uint BUFFER_SIZE = 8192;
        QPointF buffer[BUFFER_SIZE];
        uint firstIndex;
        uint lastIndex;
        QPointF force;
        
        inline void addPoint(QPointF p) { buffer[(lastIndex++) % BUFFER_SIZE] = p; }
        inline QPointF &lastPoint()  { return buffer[(lastIndex - 1) % BUFFER_SIZE]; }
        inline QPointF  takeFirstPoint() { return  buffer[(firstIndex++) % BUFFER_SIZE]; }
        inline void reset() { firstIndex = 0; lastIndex = 0;}
        inline int pointCount() const { return lastIndex - firstIndex; } 
        
    };
        
    QHash<GraphicsNode*, NodeAnimation> m_animations; 
};

#endif


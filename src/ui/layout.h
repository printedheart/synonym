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
#ifndef LAYOUT_H
#define LAYOUT_H

#include "graphnode.h"
#include "graphedge.h"
#include "graphscene.h"

#include <cmath>
#include <QList>
#include <QLinkedList>
#include <QThread>

static const double PI = 3.14159265358979323846264338327950288419717;
static double TWO_PI = 2.0 * PI;


/**
    @author Sergey Melderis <sergey.melderis@gmail.com>
*/
class Layout
{
public:
    Layout() {};

    virtual ~Layout() {};

    virtual bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges) = 0;
    
    virtual bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart) = 0;
    
    virtual void stop() {}

};

    
/*                    

class ForceDirectedLayout : public Layout
{
public:
    ForceDirectedLayout();
    ~ForceDirectedLayout();

    bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges);

    bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart);
private:
    static const int REST_DISTANCE = 75;

    static const qreal STIFFNESS = 0.2;

    static const qreal REPULSION = 1000;
    
};

// class ForceDirectedLayout2 : public Layout
// {
// public:
//     ForceDirectedLayout2();
//     ~ForceDirectedLayout2();
// 
//     void layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges);
//     
// private:
//     static const int REST_DISTANCE = 75;
// 
//     static const qreal STIFFNESS = 30;
// 
//     static const int REPULSION = 1000;
// };
// 
class ForceDirectedLayout3 : public Layout
{
public:
    ForceDirectedLayout3();
    ~ForceDirectedLayout3();

    bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges);

    bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart);
private:
    static const int REST_DISTANCE = 75;

    static const qreal STIFFNESS = 0.2;

    static const qreal REPULSION = 5000;

};
*/

#include <QMutex>
#include <QWaitCondition>                 

                 
class ForceDirectedLayout4 : public QThread, public Layout
{
Q_OBJECT
public:
    ForceDirectedLayout4();
    ~ForceDirectedLayout4();

    bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges);
    
    bool layout(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges, bool restart);
    
    void run();
    
    void stop();
    
    volatile bool finishedLayout;
    
private slots:
    void wakeUp();
        

private:
    
    bool layoutSerial(QList<GraphicsNode*> nodes, QList<GraphicsEdge*> edges);
    
    void prepareLayout(GraphicsNode *rootNode);
    void layoutNodes(GraphicsNode *node, GraphicsNode *parentNode,
                        QSet<GraphicsNode*> &visitSet);
    
    static const int REST_DISTANCE = 100;

    static const qreal STIFFNESS = 0.2;

    static const qreal REPULSION = 3000;
    
    volatile bool firstRemoved;
    
    volatile bool m_abort;
    QWaitCondition m_aborted;
        
    QMutex m_mutex;
    
    QList<GraphicsNode*> m_nodes;
    QList<GraphicsEdge*> m_edges;
    
    
    volatile bool m_restart;
    
    
    struct NodeAnimation {
        NodeAnimation() : firstIndex(0), lastIndex(0) {}
        static const  uint BUFFER_SIZE = 8192;
        QPointF buffer[BUFFER_SIZE];
        volatile uint firstIndex;
        volatile uint lastIndex;

        inline void addPoint(QPointF p) { buffer[lastIndex++] = p; }
        inline QPointF &  lastPoint()  { return buffer[(lastIndex - 1)]; }
        QPointF  takeFirstPoint() { return  buffer[firstIndex++]; }
        inline void reset() { firstIndex = 0; lastIndex = 0;}
        inline int pointCount() const { return lastIndex - firstIndex; } 
    };
        
    QHash<GraphicsNode*, NodeAnimation> m_animations; 


};











#endif

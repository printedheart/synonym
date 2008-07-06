/***************************************************************************
 *   Copyright (C) 2008 by Sergejs Melderis                                *
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
 **************************************************************************/

#ifndef LAYOUT1_H
#define LAYOUT1_H

#include "layout.h"
#include <QtCore>

class ForceDirectedLayout : public QThread, public Layout
{
Q_OBJECT
public:
    ForceDirectedLayout(QObject *parent = 0);
    ~ForceDirectedLayout();

    // TODO: make the following parameters configurable.
    static const qreal STIFFNESS = 0.2;
    static const qreal REPULSION = 3000;
    
    static const int FINISHED = 1;
    static const int CALC_ANIMATION = 2;
    static const int CALC_FORCES = 3;
    static const int ABORT_LOOP = 5;
    

    bool layout();
    
    void stop();
    
    void run();
private slots:
    
    void wakeUp();
private:
    void preLayout(GraphicsNode *rootNode);
    void layoutNodes(GraphicsNode *node, QSet<GraphicsNode*> &visitSet);
    
    bool layoutParallel();
    bool layoutSerial();
    
    void calculateAnimation();
    
    void startThread();
    
    bool firstRemoved;
    
    volatile int m_state;
    
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
    void resetAnimations();
    
    void calculateForces();
};

#endif


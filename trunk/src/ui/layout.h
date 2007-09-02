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

#include <QList>
#include <QLinkedList>
#include <QThread>

/**
	@author Sergey Melderis <sergey.melderis@gmail.com>
*/
class Layout
{
public:
    Layout() {};

    virtual ~Layout() {};

    virtual bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges) = 0;
    
    virtual bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart) = 0;
    
    virtual void stop() {}

};

class ForceDirectedLayout : public Layout
{
public:
    ForceDirectedLayout();
    ~ForceDirectedLayout();

    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);

    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart);
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
//     void layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);
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

    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);

    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart);
private:
    static const int REST_DISTANCE = 75;

    static const qreal STIFFNESS = 0.2;

    static const qreal REPULSION = 5000;

};


#include <QMutex>
#include <QWaitCondition>                 

class ForceDirectedLayout4 : public QThread, public Layout
{
Q_OBJECT
public:
    ForceDirectedLayout4();
    ~ForceDirectedLayout4();

    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);
    
    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart);
    
    void run();
    
    void stop();
    
    volatile bool finishedLayout;
    
private slots:
    void wakeUp();
        

private:
    bool _layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);
    
    static const int REST_DISTANCE = 100;

    static const qreal STIFFNESS = 0.2;

    static const qreal REPULSION = 3000;
    
    volatile bool firstRemoved;
    
    volatile bool m_abort;
    QWaitCondition m_aborted;
        
    QMutex m_mutex;
    
    QList<GraphNode*> m_nodes;
    QList<GraphEdge*> m_edges;
    
    volatile bool m_restart;

};

class ForceDirectedLayout5 : QThread,  public Layout
{
Q_OBJECT    
public:
    ForceDirectedLayout5(QObject *parent = 0);
    ~ForceDirectedLayout5();

    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);
    
    bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart);
    
    void run();
    
    volatile bool finishedLayout;
    
    

private slots:
    void wakeUp();
private:
    
    static const int REST_DISTANCE = 75;

    static const qreal STIFFNESS = 0.15;

    static const qreal REPULSION = 5000;
    
    QWaitCondition m_finished;
    
    QWaitCondition m_enoughPoints;
    volatile bool firstRemoved;
    
    volatile bool m_abort;
    QWaitCondition m_aborted;
            
    volatile bool m_hasPoints;
    
    QMutex m_mutex;
    
    QMutex m_mutex2;
    QList<GraphNode*> m_nodes;
    QList<GraphEdge*> m_edges;
    QHash<GraphNode*, QLinkedList<QPointF> > m_points;

};


class ForceDirectedLayout6 : QThread,  public Layout
{
    Q_OBJECT    
    public:
        ForceDirectedLayout6(QObject *parent = 0);
        ~ForceDirectedLayout6();

        bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges);
    
        bool layout(QList<GraphNode*> nodes, QList<GraphEdge*> edges, bool restart);
    
        void run();
    
        volatile bool finishedLayout;
    
    

    private slots:
        void wakeUp();
    private:
        
        static const int DATA_KEY = 1;
    
        static const int REST_DISTANCE = 75;

        static const qreal STIFFNESS = 0.15;

        static const qreal REPULSION = 5000;
    
        QWaitCondition m_finished;
    
        QWaitCondition m_enoughPoints;
        volatile bool firstRemoved;
    
        volatile bool m_abort;
        QWaitCondition m_aborted;
            
        volatile bool m_hasPoints;
    
        QMutex m_mutex;
    
        QMutex m_mutex2;
        QList<GraphNode*> m_nodes;
        QList<GraphEdge*> m_edges;

};





    




#endif

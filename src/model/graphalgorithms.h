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
 
#ifndef GRAPHALGORITHMS__H
#define GRAPHALGORITHMS__H
 
 
#include <QSet>
#include <QList> 
#include <QQueue>
#include <QPair>
 
template <class V, class Iter, class Container>
void findNeighbors(V *vertex, Iter start, Iter end, Container &container)
{
    while (start != end) {
        if ((void*) (*start)->source() == vertex) {
            container << (*start)->dest();
        } else if ((void*) (*start)->dest() == vertex) {
            container << (*start)->source();
        }
        start++;
    }
}






template <class V>
bool isReachable(V *from, V *goal)
{
    QSet<V*> visitSet;
    return isReachableHelper(from, goal, visitSet);    
} 
       
template <class V>
bool isReachableHelper(V *from, V *goal, QSet<V*> &visitSet)
{
    if (from == goal)
        return true;
    
    visitSet.insert(from);
    QList<V*> neighbors;
    findNeighbors(from, from->edges().constBegin(), from->edges().constEnd(), neighbors);
    
    for (int i = 0; i < neighbors.size(); ++i) {
        V *v = neighbors.at(i);
        if (!visitSet.contains(v)) {
            bool reachable = isReachableHelper(v, goal, visitSet);
            if (reachable)
                return true;
        }
    }
    return false; 
}

template <class Container, class Iter, typename Predicate>
void filter(Iter start, Iter end, Container &container, Predicate predicate)
{ 
    while (start != end) {
        if (predicate.operator() ((*start))) {
            container << (*start);
        }
        ++start;
    }
}

template <class V, class Function>
void bfs(V *vertex, Function function)
{
    levelOrderScan(vertex, function);
}        

template <class V, class Function>
void levelOrderScan(V *vertex, Function function)
{
    QSet<V*> visitSet;
    QQueue<QPair<V*, int> > vQueue;
    vQueue.enqueue(qMakePair(vertex, 0));
    while (!vQueue.isEmpty()) {
        QPair<V*, int> p = vQueue.dequeue();
        V *v = p.first;
        int level = p.second;
        function(v, level);
        visitSet.insert(v);
        QList<V*> neighbors;
        findNeighbors(v, v->edges().constBegin(), v->edges().constEnd(), neighbors);
        for (int i = 0; i < neighbors.size(); ++i) {
            if (!visitSet.contains(neighbors[i])) {
                vQueue.enqueue(qMakePair(neighbors[i], level + 1));
            }
        }
    }
}


template <class V, class Function>
void depthFirstSearch(V *vertex, Function &function)        
{
    QSet<V*> visitSet;
    depthFirstSearchHelper(vertex, (V*) 0, visitSet, function);
}

template <class V, class Function>
void depthFirstSearchHelper(V *vertex, V *prev, QSet<V*> &visitSet, Function &function)
{
    function(vertex, prev);
    visitSet.insert(vertex);
    QSet<V*> outNeighbors = vertex->outNeighbors();
    foreach (V *n, outNeighbors) {
        if (!visitSet.contains(n)) {
            depthFirstSearchHelper(n, vertex, visitSet, function);
        }
    }
}      
                
    
#endif


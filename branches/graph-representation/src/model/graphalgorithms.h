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
 
template <class V, class Iter, class Container>
inline void findNeighbors(V *vertex, Iter start, Iter end, Container &container)
{
    while (start != end) {
        if ((*start)->source() == vertex) {
            container << (*start)->dest();
        } else if ((*start)->dest() == vertex) {
            container << (*start)->source();
        }
        start++;
    }
}


template <class V>
inline bool isReachable(V *from, V *goal)
{
    QSet<V*> visitSet;
    return isReachableHelper(from, goal, visitSet);    
} 
       
template <class V>
inline bool isReachableHelper(V *from, V *goal, QSet<V*> &visitSet)
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
inline void filter(Iter start, Iter end, Container &container, Predicate predicate)
{
    while (start != end) {
        if (predicate.operator() ((*start))) {
            container << (*start);
        }
        ++start;
    }
}

template <class V, class Function>
void levelOrderScan(V *vertex, Function function)
{
    QSet<V*> visitSet;
    levelVisit(vertex, -1, function,  visitSet);
}

        
template <class V, class Function>
void levelVisit(V *vertex, int level, Function function, QSet<V*> &visitSet)
{
    level++;
    function.operator() (vertex, level);
    visitSet.insert(vertex);
    QList<V*> neighbors;
    findNeighbors(vertex, vertex->edges().constBegin(), vertex->edges().constEnd(), neighbors);
    int size = neighbors.size();
    for (int i = 0; i < size; ++i) {   
        if (!visitSet.contains(neighbors[i])) 
            levelVisit(neighbors[i], level, function,  visitSet);
    }
}
                     
    
            
                
    
#endif
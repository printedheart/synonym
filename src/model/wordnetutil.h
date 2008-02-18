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

#ifndef _WORDNETUTIL_H_
#define _WORDNETUTIL_H_

#include "relation.h"
#include "wordgraph.h"

const int POS            = 111;
const int MEANING        = 222;
const int Relation       = 223;
const int WORD           = 224;
const int LEVEL          = 225;
const int SAMPLES        = 226;


class IsMeaning
{
public:
    bool operator() (Node *node) { return node->data (MEANING).isValid(); }
};

class IsWord
{
public:
    bool operator() (Node *node) { return node->data (WORD).isValid(); }
};

class SetLevel
{
public:
    void operator() (Node *node, int level) { node->setData (LEVEL, level); }
};

class AlwaysTrue
{
public:
    bool operator() (Node *node) {  qDebug() << node->id() << " " << node->data(LEVEL).toInt(); return true; }
};

class IsPOS
{
public:
    IsPOS(QList<PartOfSpeech> &poses):
        m_poses(poses) {}
    
    bool operator() (Node *node) {
        if (node->data(POS) == QVariant()) return true;
        int nodePos = node->data(POS).toInt();
        foreach (PartOfSpeech pos, m_poses) {
            if (pos == nodePos) 
                return true;
        }
        return false;
    }
    
private:        
    QList<PartOfSpeech> m_poses;
};

class WithRelations
{
public:    
    WithRelations(Relation::Types types):
        m_types(types) {}
    
    bool operator() (Edge *edge) {
        return edge->relation() == Relation::Undefined |  m_types.testFlag(edge->relation());
    }
private:    
    Relation::Types m_types;
};


    
class VisualFilter
{
public:    
    
    void operator() (Node *node, Node *prev) {
        int level = node->data(LEVEL).toInt();
        if (level == 0 || disabledNodes.contains(node)) 
            return;
        
        if (disabledNodes.contains(prev)) {
            disabledNodes.insert(node);
        } else {
            int level = node->data(LEVEL).toInt();
            QSet<GraphicsNode*> outNeighbors = node->outNeighbors();
            // If node outneighbors are both words and meanings we will
            // show only words.
            if (level == 1 || level == 2) {
                QSet<Node*> meanings;
                QSet<Node*> words;
                filter(outNeighbors.constBegin(), outNeighbors.constEnd(), meanings, IsMeaning());
                filter(outNeighbors.constBegin(), outNeighbors.constEnd(), words, IsWord());
                if (words.size() > 0) {
                    disabledNodes.unite(meanings);
                    while (words.size() > 13) {
                        Node *first = *words.begin();
                        disabledNodes.insert(first);
                        words.remove(first);
                    }
                } else {
                    WordGraph *graph = node->graph();
                    Node *lnn = 0;
                    foreach (Node *n, outNeighbors) {
                        Relation::Type nRelation = graph->edge(node, n)->relation();
                        if (nRelation == Relation::Hyponym) {
                            disabledNodes.insert(n);
                            continue;
                        } 
                         
                        if (lnn == 0)
                            lnn = n;
                        else {
                            Relation::Type lnnRelation = graph->edge(node, lnn)->relation();
                            if (lnnRelation == Relation::Hypernym && nRelation != Relation::Hypernym) {
                                continue;
                            }
                            if (lnnRelation != Relation::Hypernym && nRelation == Relation::Hypernym) {
                                lnn = n;
                                continue;
                            }
                            
                            if (n->outNeighbors().size() < lnn->outNeighbors().size() ) {
                                lnn = n;
                            }
                        }
                    }               
                    disabledNodes.unite(outNeighbors);
                    if (lnn != 0) 
                        disabledNodes.remove(lnn);
                    
                }
                
            } else { // Never show more then 3 levels.
                disabledNodes.unite(outNeighbors);
            }
        }
    }
    
    QSet<GraphicsNode*> disabledNodes;
};



class MeaningVusualFilter
{
public:
    void operator() (Node *node, Node *prev) {
        int level = node->data(LEVEL).toInt();
        if (disabledNodes.contains(node)) 
            return;
        
        if (disabledNodes.contains(prev)) {
            disabledNodes.insert(node);
            return;
        }
        WordGraph *graph = node->graph();
        if (level == 0) {
            QSet<Node*> neighbors = node->neighbors();
            if (neighbors.size() > 15) {
                foreach (Node *n, neighbors) {
                    if (IsMeaning() (node) && graph->edge(node, n)->relation() != Relation::Hypernym) {
                        disabledNodes.insert(n);
                    }
                }
            }
        } else if (level == 1 && IsWord() (node)) {
             foreach (Node *n, node->neighbors()) {
                 if (n->data(LEVEL).toInt() > 1)
                     disabledNodes.insert(n);
             }
        } else if (level == 2) {
            foreach (Node *n, node->neighbors()) {
                if (n->data(LEVEL).toInt() > 2)
                    disabledNodes.insert(n);
            }
        }
                    
    }
            
    QSet<GraphicsNode*> disabledNodes;            
};


#endif

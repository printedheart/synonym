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


const int POS            = 111;
const int MEANING        = 222;
const int RELATIONSHIP   = 223;
const int WORD           = 224;
const int LEVEL          = 225;


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


class VisualFilter
{
public:
    bool operator() (Node *node) {
        bool canShowNode = canShow(node);
        return canShowNode;
    }
        
    bool canShow(Node *node) {
        int level = node->data(LEVEL).toInt();
        if (level > 3) return false;
        if (level == 3 && IsMeaning() (node)) return false;
        
        QSet<Node*> neighbors = node->neighbors();
        if (level >= 1 && neighbors.size() > 20) return false;
     //   if (level > 2) return false;
        
        QSet<Node*>::const_iterator iter;
        QSet<Node*>::const_iterator end = neighbors.constEnd();
        if (level == 3 && IsWord() (node)) {
            for (iter = neighbors.constBegin(); iter != end; ++iter) {
                Node *neighbor = *iter;
                if (neighbor->data(LEVEL).toInt() < level
                        && (*this)(neighbor)) {
                    return true;
                }
                return false;
            }
        }

        if (level == 2 && IsMeaning() (node)) {
            
            for (iter = neighbors.constBegin(); iter != end; ++iter) {
                Node *neighbor = *iter;
                if (neighbor->data (LEVEL).toInt() < level && hasWordNeighborsWithGreaterLevel (neighbor)) {
                    return false;
                }
            }
        }
        return true;
    }
    bool hasWordNeighborsWithGreaterLevel (Node *node) {
        IsWord isWord;
        int level = node->data (LEVEL).toInt();
        QSet<Node*> neighbors = node->neighbors();
        QSet<Node*>::const_iterator iter; 
        QSet<Node*>::const_iterator end = neighbors.constEnd();
        for (iter = neighbors.constBegin(); iter != end; ++iter) {
            Node *neighbor = *iter;
            if (neighbor->data (LEVEL).toInt() > level && isWord (neighbor) ) return true;
        }
        return false;
    }
};

#endif
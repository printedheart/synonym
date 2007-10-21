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
#ifndef EDGE_H
#define EDGE_H

#include "node.h"
#include <QObject>
#include <QString>                
/**
    @author Sergejs <sergey.melderis@gmail.com>
*/


class Relationship
{
 public:
     enum { Antonym    = 1,    Hypernym = 2,  Hyponym    = 3, 
            Entailment = 4,    Similar  = 5,  IsMember   = 6, 
            IsStuff    = 7,    IsPart   = 8,  HasMember  = 9,
            HasStuff   = 10,   HasPart  = 11, Meronym    = 12,
            Holonym    = 13,   Cause    = 14, Participle = 15,
            };
     static const QString descriptions[22];
            
         
     static QString toString(int relationship) { 
         if (relationship > 0 && relationship <= 15)
            return descriptions[relationship - 1];
         return QString();
     } 
     
};



class Edge : public QObject {
Q_OBJECT
            
public:
    Edge(const QString &id, Node *sourceNode, Node *destNode, QObject *parent = 0);
    ~Edge();

    Node *source() const;
    void setSource(Node *node);

    Node *dest() const;
    void setDest(Node *node);
    
    Node *neighborTo(Node *to) const;

    QString id() const;
    
    int relationship() const;
    void setRelationship(int relationship);
    
private:
    Node *m_source;
    Node *m_dest;
    QString m_id;
    int m_relationship;
};

#endif

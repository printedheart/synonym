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
#ifndef RELATIONSHIP_H
#define RELATIONSHIP_H

#include <QFlags>
#include <QString>
#include <QList>

class Relationship
{
public:
    enum Type { 
        Antonym          = 0x1, 
        Hypernym         = 0x2, 
        Hyponym          = 0x4,  
        Entailment       = 0x10, 
        Similar          = 0x20,
        IsMember         = 0x40,
        IsStuff          = 0x100,
        IsPart           = 0x200,
        HasMember        = 0x400,
        HasStuff         = 0x1000,
        HasPart          = 0x2000,
        Meronym          = 0x4000,
        Holonym          = 0x10000,
        Cause            = 0x20000,
        Participle       = 0x40000,
        SeeAlso          = 0x100000,
        Pertains         = 0x200000,
        Attribute        = 0x400000,
        VerbGroup        = 0x1000000,
        Derivation       = 0x2000000,
        Classification   = 0x4000000
    };
    
    Q_DECLARE_FLAGS(Types, Type)
            
            
    // The types for different parts of speech        
    static const Types NounTypes;
    static const Types VerbTypes;
    static const Types AdjectiveTypes;
    static const Types AdverbTypes;
    
    /**
     * Returns Types for part of speech
     */
    static Types typesForPos(int pos);
    
    
    /**
     * Returns an integer to use for wordnet C api for Type.
     */
    static int toSearchType(Type type);
    
    /**
     * Returns string representation of the relationship.
     */
    static QString toString(int relationship);
    
    /**
     * Returns a list of all possible relationships.
     */
    static QList<Type> types();
    
private:    
    static const int SIZE = 21;    
    static const QString descriptions[SIZE];
    static const Type typesArray[SIZE];
    
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Relationship::Types)




#endif

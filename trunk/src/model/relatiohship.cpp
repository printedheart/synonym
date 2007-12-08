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

#include "relationship.h"



QString Relationship::toString(Type type, int pos)
{
    static QString descriptions[SIZE] = 
    { 
        "undefined",
        "antonym",
        "is a",
        "hyponym",
        "entails", 
        "is similar to",
        "is member of",
        "is made of",
        "is part of",
        "member",
        "has stuff",
        "has part",
        "meronym",
        "holonym",
        "cause",
        "participle",
        "also see",
        "pertains to",
        "attribute",
        "verb group", 
        "derivation", 
        "domain",
        "domain"
    };
    
    if (pos != 0) {
        if (!typesForPos(pos).testFlag(type)) {
            return "Undefined";
        }
        
        if (type == Pertains && pos == Adverb) {
                return "Derived from";
        }    
    }
    
    for (int i = 0; i < SIZE; i++) {
        if (typesArray[i] == type) {
            return descriptions[i];
        }
    }
   
    return QString();
}

const Relationship::Type Relationship::typesArray[SIZE] = 
{
    Undefined,
    Antonym,
    Hypernym,
    Hyponym, 
    Entailment,
    Similar,
    IsMember, 
    IsStuff,
    IsPart,
    HasMember,
    HasStuff,
    HasPart,
    Meronym,
    Holonym,
    Cause,
    Participle,
    SeeAlso,
    Pertains,
    Attribute,
    VerbGroup,
    Derivation,
    Classification,
    Class
            
            
};

QList<Relationship::Type> Relationship::types()
{
    QList<Type> typesList;
    for (int i = 1; i < SIZE; ++i) {
        typesList.append(typesArray[i]);
    }
    return typesList;
} 

int Relationship::toSearchType(Type type) {
    if (type == Undefined) {
        return -1;
    }
    for (int i = 1; i < SIZE; ++i) {
        if (typesArray[i] & type) {
            int searchType = i;
            if (type == Hypernym) 
                searchType = -searchType;
            return searchType;
        }
    }
    return -1;
}

const Relationship::Types Relationship::NounTypes = Types(Hypernym | Antonym | Hyponym | Holonym | IsPart | IsMember | IsStuff | Meronym | HasStuff | HasMember | HasPart | Classification | Attribute | Class); 
const Relationship::Types Relationship::VerbTypes = Types(Hypernym | Antonym | Entailment | Cause | Classification | VerbGroup | Hyponym);
const Relationship::Types Relationship::AdjectiveTypes = Types(Similar | Antonym | Pertains | Attribute | Classification);
const Relationship::Types Relationship::AdverbTypes = Types(Antonym | Pertains | Classification | Derivation);       

Relationship::Types Relationship::typesForPos(int pos)
{
    switch (pos) {
        case 1: return NounTypes;
        case 2: return VerbTypes;
        case 3: return AdjectiveTypes;
        case 4: return AdverbTypes;
        default: return Relationship::Types(0);
    }
}

Relationship::Types Relationship::allTypes()
{
    return ~Types(0);
}

Relationship::Type Relationship::toType(int intType)
{
    if (intType < 0 || intType >= SIZE) {
        return Undefined;
    }
    return typesArray[intType];
}

bool Relationship::applies(Type type, int forPos)
{
    return typesForPos(forPos).testFlag(type);
}

Relationship::Type Relationship::symmetricTo(Type type)
{
    switch(type) {
        case Hypernym: return Hyponym;
        case Hyponym: return Hypernym;
        case Meronym: return Holonym;
        case Holonym: return Meronym;
        case Antonym: return Antonym;
        case Similar: return Similar;
        default: return Undefined;
    }
}
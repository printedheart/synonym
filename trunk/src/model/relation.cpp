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

#include "relation.h"


QString Relation::toString(Type type, int pos)
{
    static QString labels[SIZE] = 
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
        "domain ",
        "domain",
        "synonym"
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
            return labels[i];
        }
    }
   
    return QString();
}

const Relation::Type Relation::typesArray[SIZE] = 
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
    Class,
    Syns        
            
};

QList<Relation::Type> Relation::types()
{
    QList<Type> typesList;
    for (int i = 1; i < SIZE; ++i) {
        typesList.append(typesArray[i]);
    }
    return typesList;
} 

int Relation::toSearchType(Type type) {
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

const Relation::Types Relation::NounTypes = Types(Hypernym | Antonym | Hyponym | Holonym | IsPart | IsMember | IsStuff | Meronym | HasStuff | HasMember | HasPart | Classification | Attribute | Class); 
const Relation::Types Relation::VerbTypes = Types(Hypernym | Antonym | Entailment | Cause | Classification | VerbGroup | Hyponym);
const Relation::Types Relation::AdjectiveTypes = Types(Similar | Antonym | Pertains | Attribute | Classification);
const Relation::Types Relation::AdverbTypes = Types(Syns | Antonym | Pertains | Classification | Derivation);       

Relation::Types Relation::typesForPos(int pos)
{
    switch (pos) {
        case 1: return NounTypes;
        case 2: return VerbTypes;
        case 3: return AdjectiveTypes;
        case 4: return AdverbTypes;
        default: return Relation::Types(0);
    }
}

Relation::Types Relation::allTypes()
{
    return ~Types(0);
}

Relation::Type Relation::toType(int intType)
{
    if (intType < 0 || intType >= SIZE) {
        return Undefined;
    }
    return typesArray[intType];
}

bool Relation::applies(Type type, int forPos)
{
    return typesForPos(forPos).testFlag(type);
}

Relation::Type Relation::symmetricTo(Type type)
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



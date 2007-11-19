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


//const int Relationship::SIZE = 21;

const QString Relationship::descriptions[SIZE] = 
{ 
    "Antonym",
    "Is a",
    "Hyponym",
    "Entails", 
    "Is similar to",
    "Is member of",
    "Is made of",
    "Is part of",
    "Member",
    "Has stuff",
    "Has part",
    "Meronym",
    "Holonym",
    "Cause",
    "Participle",
    "Also see",
    "Pertains to nound",
    "Attribute",
    "Verb group", 
    "Derivation", 
    "Domain"
};

QString Relationship::toString(int relationship)
{
    if (relationship > 0 && relationship <= SIZE)
            return descriptions[relationship - 1];
    
    return QString();
}

const Relationship::Type Relationship::typesArray[SIZE] = 
{
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
    Classification
};

QList<Relationship::Type> Relationship::types()
{
    QList<Type> typesList;
    for (int i = 0; i < SIZE; ++i) {
        typesList.append(typesArray[i]);
    }
    return typesList;
} 

int Relationship::toSearchType(Type type) {
    for (int i = 0; i < SIZE; ++i) {
        if (typesArray[i] == type) {
            return i + 1;
        }
    }
}

const Relationship::Types Relationship::NounTypes = Types(Hypernym | Antonym | Hyponym | Holonym | IsPart | IsMember | IsStuff | Meronym | HasStuff | HasMember | HasPart | Classification | Attribute); 
const Relationship::Types Relationship::VerbTypes = Types(Hypernym | Antonym | Entailment | Cause | Derivation | Classification);
const Relationship::Types Relationship::AdjectiveTypes = Types(Similar | Antonym | Pertains | Attribute | Classification);
const Relationship::Types Relationship::AdverbTypes = Types(Antonym | Pertains | Classification);       

Relationship::Types Relationship::typesForPos(int pos)
{
    switch (pos) {
        case 1: return NounTypes;
        case 2: return VerbTypes;
        case 3: return AdjectiveTypes;
        case 4: return AdverbTypes;
    }
}
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
#include "dbdataloader.h"
#include "wordgraph.h"
#include "wordnetutil.h"
#include <QtSql>
#include <QDir>
#include <QMessageBox>

DbDataLoader::DbDataLoader(QObject *parent)
 : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbFile = QDir::currentPath() + "/wordnet30";
    db.setDatabaseName(dbFile);
    
    bool ok = db.open();
    if (ok) {
        QSqlQuery query(db);
        ok = query.exec("SELECT word.wordid from word where lemma = 'word'");
    }
    if (!ok) {
        QMessageBox::critical(0,
                              "Synonym",
                              "Cannot open database file '" + dbFile + "'.\n" + 
                              "Please place wordnet30 file together with executable.");
    }
                                          
    
    relTypeLinkId[Relation::Antonym] = 30;
    relTypeLinkId[Relation::Hypernym] = 1;
    relTypeLinkId[Relation::Hyponym] = 2;
    relTypeLinkId[Relation::Entailment] = 21;
    relTypeLinkId[Relation::Similar] = 40;
    relTypeLinkId[Relation::IsMember] = 14;
    relTypeLinkId[Relation::IsStuff] = 16;
    relTypeLinkId[Relation::IsPart] = 12;
    relTypeLinkId[Relation::HasMember] = 13;
    relTypeLinkId[Relation::HasStuff] = 15;
    relTypeLinkId[Relation::HasPart] = 11;
    relTypeLinkId[Relation::Meronym] = 0;
    relTypeLinkId[Relation::Holonym] = 0;
    relTypeLinkId[Relation::Cause] = 23;
    relTypeLinkId[Relation::Participle] = 71;
    relTypeLinkId[Relation::SeeAlso] = 50;
    relTypeLinkId[Relation::Pertains] = 80;
    relTypeLinkId[Relation::Attribute] = 60;
    relTypeLinkId[Relation::VerbGroup] = 70;
    relTypeLinkId[Relation::Derivation] = 81;
    relTypeLinkId[Relation::Classification] = 0;
    relTypeLinkId[Relation::Class] = 0;
    relTypeLinkId[Relation::Syns] = 0;
    
    QHashIterator<Relation::Type, int> i(relTypeLinkId);
    while (i.hasNext()) {
        i.next();
        linkIdRelType[i.value()] = i.key();
    }
}


DbDataLoader::~DbDataLoader()
{
}



static int getpos(QString pos)
{
    if (pos == "n")
        return Noun;
    else if (pos == "v")
        return Verb;
    else if (pos == "a" || pos == "s")
        return Adjective;
    else if (pos == "r")
        return Adverb;
    return 0;
}


WordGraph * DbDataLoader::createWordGraph(const QString &searchWord, Relation::Types searchTypes)
{
    TemplateNodeFactory<WordGraphicsNode> wordFactory;
    TemplateNodeFactory<MeaningGraphicsNode> meaningFactory;
    TemplateEdgeFactory<GraphicsEdge> edgeFactory;
    WordGraph *graph = new WordGraph();
    
    QString linkIds;
    foreach (Relation::Type type, Relation::types()) {
        if (type & searchTypes) {
            linkIds += QString::number(relTypeLinkId[type]);
            linkIds += ",";
        }
    }
    linkIds.remove(linkIds.size() - 1, 1);
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    
    int wordId;
    // Get synsets
    query.prepare("SELECT word.wordid, synset.synsetid, pos, definition " 
                  "FROM word, sense, synset " 
                  "WHERE word.wordid = sense.wordid "  
                  "AND sense.synsetid = synset.synsetid "  
                  "AND lemma = ?");
    query.addBindValue(searchWord);
    query.exec();
    Node *searchNode = graph->addNode(searchWord, wordFactory);
    searchNode->setData(WORD, searchWord);
    QString synsets;
    int count = 0;
    while (query.next()) {
        count++;
        wordId = query.value(0).toInt();
        QString synsetId = query.value(1).toString();
        Node *meaning = graph->addNode(synsetId, meaningFactory);
        meaning->setData(POS, getpos(query.value(2).toString()));
        meaning->setData(MEANING, query.value(3).toString());
        graph->addEdge(searchNode->id(), synsetId, edgeFactory);
        synsets += synsetId + ",";
    }

    synsets.remove(synsets.size() - 1, 1);
    query.exec("SELECT linkdef.linkid, synset1id,  synset2id, pos, definition "
            "FROM semlinkref, linkdef, synset "
            "WHERE semlinkref.linkid = linkdef.linkid "
            "AND synset.synsetid = semlinkref.synset2id "
            "AND semlinkref.synset1id IN(" + synsets + ")"
            "AND linkdef.linkid IN(" + linkIds + ")");
    while (query.next()) {
        Node *meaning = graph->addNode(query.value(2).toString(), meaningFactory);
        meaning->setData(POS, getpos(query.value(3).toString()));
        meaning->setData(MEANING, query.value(4).toString());
        
        Edge *edge = graph->addEdge(query.value(1).toString(), meaning->id(), edgeFactory);
        if (edge) {
            edge->setRelation(linkIdRelType[query.value(0).toInt()]);
        }
    }

    
    synsets = "";
    QList<Node*> meanings = graph->nodes(IsMeaning());
    foreach (Node *node, meanings)
        synsets += (node->id() + ",");
     
    synsets.remove(synsets.size() - 1, 1);    
    
    // Get all words for synsets
    query.exec("SELECT word.wordid, lemma, synset.synsetid FROM word, sense, synset "  
                  "WHERE word.wordid = sense.wordid " 
                  "AND sense.synsetid = synset.synsetid "  
                  "AND synset.synsetid IN(" + synsets + ")");
    while (query.next()) {
        Node *word = graph->addNode(query.value(1).toString(), wordFactory);        
        word->setData(WORD, query.value(1));
        graph->addEdge(query.value(2).toString(), word->id(), edgeFactory);
    }
    
    // Get all samples
    query.exec("SELECT synsetid, sample from sample WHERE synsetid IN(" + synsets + ")");
    while (query.next()) {
        Node *node = graph->node(query.value(0).toString());
        QList<QVariant> samples = node->data(SAMPLES).toList();
        samples.append(query.value(1).toString());
        node->setData(SAMPLES, samples);            
    }
    
    
    // Get lexical links
    query.exec("select se2.rank, w2.lemma, ldef.linkid, ldef.name, sy1.pos, sy2.pos "
               "from word w1 "
               "left join sense se1 on w1.wordid = se1.wordid "
               "join synset sy1 on se1.synsetid = sy1.synsetid "
               "join lexlinkref lref on sy1.synsetid = lref.synset1id and w1.wordid = lref.word1id "
               "join synset sy2 on lref.synset2id = sy2.synsetid "
               "join sense se2 on sy2.synsetid = se2.synsetid "
               "join word w2 on se2.wordid = w2.wordid "
               "join linkdef ldef on lref.linkid = ldef.linkid "
               "where w1.lemma = '" + searchWord +"' and w2.lemma != '" + searchWord +"' group by ldef.linkid order by se1.rank asc");
    while (query.next()) {
        int linkId = query.value(2).toInt();
        if (!linkIdRelType.contains(linkId))
            continue;
        Node *word = graph->addNode(query.value(1).toString(), wordFactory);
        word->setData(WORD, query.value(1).toString());
        word->setData(POS, getpos(query.value(5).toString()));
        searchNode->setData(POS, getpos(query.value(4).toString()));
        Edge *edge = graph->addEdge(searchNode->id(), word->id(), edgeFactory);
        if (edge) {
            edge->setRelation(linkIdRelType[linkId]);
        }
    }
    
   
    return graph;
    
}
     
QStringList DbDataLoader::words() const
{
    QStringList words;
    
    QSqlQuery query(QSqlDatabase::database());
    query.exec("SELECT lemma from word");
    while (query.next())
        words << query.value(0).toString();
    
    return words;
}




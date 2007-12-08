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
#include "worddataloader.h"
#include "worddatagraph.h"
#include "graphnode.h"
#include "graphedge.h"
#include <QtCore>                  
#include "wn.h"
#include "wordnetutil.h"        
#include <iostream>
WordDataLoader::WordDataLoader(QObject *parent)
 : QObject(parent)
{
    wninit();
}


WordDataLoader::~WordDataLoader()
{
}

WordGraph * WordDataLoader::createWordGraph(const QString &searchWord, Relationship::Types searchTypes) 
{
    static const Relationship::Types useThis(
            Relationship::IsPart | 
            Relationship::IsStuff |
            Relationship::HasStuff | 
            Relationship::HasPart |
            Relationship::IsMember |
            Relationship::HasMember);
    
    TemplateNodeFactory<WordGraphicsNode> wordFactory;
    TemplateNodeFactory<MeaningGraphicsNode> meaningFactory;
    TemplateEdgeFactory<GraphicsEdge> edgeFactory;
 
    WordGraph *wordGraph = new WordGraph();
    Node *wordNode = wordGraph->addNode(searchWord, wordFactory);
    wordNode->setData(WORD, searchWord);
    SynsetPtr synsetToFree;
    
    QList<Relationship::Type> allTypesList = Relationship::types();
    for (int pos = 1; pos <= NUMPARTS; pos++) {
        
        foreach (Relationship::Type searchType, allTypesList) {  
            if (!Relationship::typesForPos(pos).testFlag(searchType) || !searchTypes.testFlag(searchType))
                continue;
                
            int wnSearchType = Relationship::toSearchType(searchType);
            SynsetPtr synset = findtheinfo_ds(searchWord.toLatin1().data(),
                                            pos, wnSearchType, ALLSENSES);
            synsetToFree = synset;
            
            qDebug() << Relationship::toString(searchType, pos);
            while (synset) {
                SynsetPtr nextSynset = synset->nextss;
                
                Node *meaning = wordGraph->addNode(QString::number(synset->hereiam), meaningFactory);
                meaning->setData(POS, getpos(synset->pos));
                meaning->setData(MEANING, synset->defn);
                Edge *edge = wordGraph->addEdge(wordNode->id(), meaning->id(), edgeFactory);
    
                for (int i = 0; i < synset->wcount; i++) {
                    if (searchWord != synset->words[i]) {
                        Node *word = wordGraph->addNode(synset->words[i], wordFactory);
                        word->setData(WORD, QString(synset->words[i]).replace(QChar('_'), QChar(' ')));
                        Edge *edge = wordGraph->addEdge(meaning->id(), word->id(), edgeFactory);
                            
                    }
                }
                Relationship::Type parentSynsetType = Relationship::toType(*synset->ptrtyp);
                synset = synset->ptrlist;
                while (synset) {
                    Relationship::Type synsetType = Relationship::toType(*synset->ptrtyp);
                    Node *meaning2 = wordGraph->addNode(QString::number(synset->hereiam), meaningFactory);
                    
                    meaning2->setData(POS, getpos(synset->pos));
                    meaning2->setData(MEANING, synset->defn);
                    Edge *edge = wordGraph->addEdge(meaning->id(), meaning2->id(), edgeFactory);
                    qDebug() << meaning->id() << "   " << meaning2->id() 
                             << "  parent : " << Relationship::toString(parentSynsetType, meaning->data(POS).toInt())
                             << "  synset : " << Relationship::toString(synsetType, meaning->data(POS).toInt()) 
                             << "  search : " << Relationship::toString(searchType, meaning->data(POS).toInt());
                    if (edge) {
                        if (useThis & searchType) {
                            if (searchType & Relationship::HasPart)
                                edge->setRelationship(Relationship::IsPart);
                            else if (searchType & Relationship::HasStuff)
                                edge->setRelationship(Relationship::IsStuff);
                            else if (searchType & Relationship::HasMember)
                                edge->setRelationship(Relationship::IsMember);
                            else
                                edge->setRelationship(searchType);
                            
                        } else {
                            if (searchType & Relationship::Antonym) 
                                edge->setRelationship(searchType);
                            else if (searchType & parentSynsetType & synsetType)
                                edge->setRelationship(searchType);
                            else if  (searchType & parentSynsetType)
                                edge->setRelationship(searchType);
                            else if (searchType & synsetType)
                                edge->setRelationship(searchType);
                            else if (synsetType & parentSynsetType)
                                edge->setRelationship(synsetType);
                            else if (Relationship::symmetricTo(searchType) & parentSynsetType)
                                edge->setRelationship(parentSynsetType); 
                            
                            
                            else if (Relationship::applies(synsetType, meaning->data(POS).toInt()))
                                edge->setRelationship(synsetType);
                            else if (Relationship::applies(parentSynsetType, meaning->data(POS).toInt()))
                                edge->setRelationship(parentSynsetType);
                            else 
                                edge->setRelationship(searchType);
                        }
                    }
                    for (int i = 0; i < synset->wcount; i++) {
                        qDebug() << Relationship::toString(searchType, pos) << "  " << synset->words[i];
                        if (searchWord != synset->words[i]) {
                            Node *word = wordGraph->addNode(synset->words[i], wordFactory);
                            word->setData(WORD, QString(synset->words[i]).replace(QChar('_'), QChar(' ')));
                            Edge *edge = wordGraph->addEdge(meaning2->id(), word->id(), edgeFactory);
                        }
                    }
                    
                    synset = synset->nextss;
                }
                synset = nextSynset;
            }
        }
    }
//    free_syns(synsetToFree);
    
    return wordGraph;
}

QStringList WordDataLoader::words() const
{
    QStringList allWords;
    if (indexfps) {
        QFile file;
        for (int i = 1; i < NUMPARTS + 1; i++) {
            QFile file;
            if (!file.open(indexfps[i], QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "Cannot open file" << file.fileName();
                continue;
            }
                
            while (!file.atEnd()) {
                QByteArray line = file.readLine();
                if (line.startsWith(' '))
                    continue;
                
                QByteArray phraseWithUnderscores =  line.split(' ')[0];
                QList<QByteArray> words = phraseWithUnderscores.split('_');
                QString phrase;
                foreach (QByteArray word, words) {
                    phrase.append(word.data());
                    phrase.append(' ');
                }
                allWords << phrase.trimmed();
            }
        }
        
        allWords.sort();   
    }
    return allWords;
}


            

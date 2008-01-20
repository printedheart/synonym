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
#include "wordgraph.h"
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

// Whatever comes out from Wordnet C API does not always makes sense to me.
// Python API is much more simple and easier to use.
WordGraph * WordDataLoader::createWordGraph(const QString &searchWord, Relation::Types searchTypes) 
{
    static const Relation::Types watchForTypes(
            Relation::IsPart | 
            Relation::IsStuff |
            Relation::HasStuff | 
            Relation::HasPart |
            Relation::IsMember |
            Relation::HasMember);
    
    TemplateNodeFactory<WordGraphicsNode> wordFactory;
    TemplateNodeFactory<MeaningGraphicsNode> meaningFactory;
    TemplateEdgeFactory<GraphicsEdge> edgeFactory;
 
    WordGraph *wordGraph = new WordGraph();
    Node *wordNode = wordGraph->addNode(searchWord, wordFactory);
    wordNode->setData(WORD, QString(searchWord).replace('_', ' '));
    
    QList<SynsetPtr> createdSynsets;
    
    QList<Relation::Type> allTypesList = Relation::types();
    for (int pos = 1; pos <= NUMPARTS; pos++) {
        
        foreach (Relation::Type searchType, allTypesList) {  
            if (!Relation::typesForPos(pos).testFlag(searchType) || !searchTypes.testFlag(searchType))
                continue;
                
            
            unsigned int defined = is_defined(searchWord.toLatin1().data(), pos);
            int wnSearchType = Relation::toSearchType(searchType);
            
            if (!(defined & bit(wnSearchType))) 
                continue;
            
            SynsetPtr synset = findtheinfo_ds(searchWord.toLatin1().data(),
                                            pos, wnSearchType, ALLSENSES);
            createdSynsets.append(synset);
            
        //    qDebug() << Relation::toString(searchType, pos) << " " << pos;
            while (synset) {
                SynsetPtr nextSynset = synset->nextss;
                
                Node *meaning = wordGraph->addNode(QString::number(synset->hereiam), meaningFactory);
                meaning->setData(POS, getpos(synset->pos));
                meaning->setData(MEANING, synset->defn);
                wordGraph->addEdge(wordNode->id(), meaning->id(), edgeFactory);
    
                for (int i = 0; i < synset->wcount; i++) {
                    if (searchWord != synset->words[i]) {
                        Node *word = wordGraph->addNode(synset->words[i], wordFactory);
                        word->setData(WORD, QString(synset->words[i]).replace(QChar('_'), QChar(' ')));
                        wordGraph->addEdge(meaning->id(), word->id(), edgeFactory);
                            
                    }
                }
                if (!synset->ptrtyp)
                    break;
                
                Relation::Type parentSynsetType = Relation::toType(*synset->ptrtyp);
                synset = synset->ptrlist;
                while (synset) {
                    Relation::Type synsetType = Relation::toType(*synset->ptrtyp);
                    Node *meaning2 = wordGraph->addNode(QString::number(synset->hereiam), meaningFactory);
                    
                    meaning2->setData(POS, getpos(synset->pos));
                    meaning2->setData(MEANING, synset->defn);
                    Edge *edge = wordGraph->addEdge(meaning->id(), meaning2->id(), edgeFactory);
//                     qDebug() << meaning->id() << "   " << meaning2->id() 
//                              << "  parent : " << Relation::toString(parentSynsetType, meaning->data(POS).toInt())
//                              << "  synset : " << Relation::toString(synsetType, meaning->data(POS).toInt()) 
//                              << "  search : " << Relation::toString(searchType, meaning->data(POS).toInt());
                    if (edge) {
                        if (watchForTypes & searchType) {
                            if (searchType & Relation::HasPart)
                                edge->setrelation(Relation::IsPart);
                            else if (searchType & Relation::HasStuff)
                                edge->setrelation(Relation::IsStuff);
                            else if (searchType & Relation::HasMember)
                                edge->setrelation(Relation::IsMember);
                            else
                                edge->setrelation(searchType);
                            
                        } else {
                            if (searchType & Relation::Antonym) 
                                edge->setrelation(searchType);
                            else if (searchType & parentSynsetType & synsetType)
                                edge->setrelation(searchType);
                            else if  (searchType & parentSynsetType)
                                edge->setrelation(searchType);
                            else if (searchType & synsetType)
                                edge->setrelation(searchType);
                            else if (synsetType & parentSynsetType)
                                edge->setrelation(synsetType);
                            else if (Relation::symmetricTo(searchType) & parentSynsetType)
                                edge->setrelation(parentSynsetType); 
                            
                            
                            else if (Relation::applies(synsetType, meaning2->data(POS).toInt()))
                                edge->setrelation(synsetType);
                            else if (Relation::applies(parentSynsetType, meaning2->data(POS).toInt()))
                                edge->setrelation(parentSynsetType);
                            else 
                                edge->setrelation(searchType);
                        }
                    }
                    for (int i = 0; i < synset->wcount; i++) {
                    //    qDebug() << Relation::toString(searchType, pos) << "  " << synset->words[i];
                        if (searchWord != synset->words[i]) {
                            Node *word = wordGraph->addNode(synset->words[i], wordFactory);
                            word->setData(WORD, QString(synset->words[i]).replace(QChar('_'), QChar(' ')));
                            wordGraph->addEdge(meaning2->id(), word->id(), edgeFactory);
                        }
                    }
                    
                    synset = synset->nextss;
                }
                synset = nextSynset;
            }
        }
        
    }
    
     foreach (SynsetPtr synset, createdSynsets) 
         free_syns(synset);
     
    
    
    //If there is only one node, meaning that is the central node
    //we put before doing any search, then we did not find anything.
    if (wordGraph->nodes().size() == 1) {
        delete wordGraph;
        wordGraph = 0;
    }
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


            



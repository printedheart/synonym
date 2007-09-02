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
#include "node.h"
#include "edge.h"
#include <QDebug>            
#include <QBuffer>                
#include <QCoreApplication>
                  
#include "wn.h"        
WordDataLoader::WordDataLoader(QObject *parent)
 : QObject(parent)
{
    wninit();
}


WordDataLoader::~WordDataLoader()
{
}

void WordDataLoader::load(const QString &phrase, WordDataGraph *dataGraph)
{
    PhraseNode *phraseNode = new PhraseNode(phrase, dataGraph);
    phraseNode->setFixed(true);
    dataGraph->addNode(phraseNode);
    
    for (int pos = 1; pos <= NUMPARTS; pos++) {
        SynsetPtr synset = findtheinfo_ds(phrase.toLatin1().data(),
                                          pos, - HYPERPTR  /*ANTPTR */, ALLSENSES);

        while (synset) {
            SynsetPtr nextSynset = synset->nextss;
            MeaningNode *meaning = new MeaningNode(QString::number(synset->hereiam));
            meaning->setPartOfSpeech(pos);
            meaning->setMeaning(synset->defn);
            dataGraph->addNode(meaning);
            dataGraph->addEdge(phraseNode->id(), meaning->id());

            if (synset->wcount == 1) {
                synset = synset->ptrlist;
                if (synset) {
                    MeaningNode *meaning2 = new MeaningNode(QString::number(synset->hereiam));
                    meaning2->setPartOfSpeech(pos);
                    meaning2->setMeaning(synset->defn);
                    dataGraph->addNode(meaning2);
                    dataGraph->addEdge(meaning->id(), meaning2->id());

                    for (int i = 0; i < synset->wcount; i++) {
                        if (m_curPhrase != synset->words[i]) {
                            PhraseNode *word =  new PhraseNode(synset->words[i], dataGraph);
                            dataGraph->addNode(word);
                            dataGraph->addEdge(meaning2->id(), word->id());
                        }
                    }
                }
            } else {
                for (int i = 0; i < synset->wcount; i++) {
                    if (m_curPhrase != synset->words[i]) {
                        PhraseNode *word =  new PhraseNode(synset->words[i], dataGraph);
                        dataGraph->addNode(word);
                        dataGraph->addEdge(meaning->id(), word->id());
                    }
                }
            }
            

            synset = nextSynset;
        }
    }
}


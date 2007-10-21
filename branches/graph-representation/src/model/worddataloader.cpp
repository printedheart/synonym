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
#include <QtCore>                  
#include "wn.h"        
#include <iostream>
WordDataLoader::WordDataLoader(QObject *parent)
 : QObject(parent)
{
    wninit();
}


WordDataLoader::~WordDataLoader()
{
}

WordGraph * WordDataLoader::createWordGraph(const QString &searchWord) 
{
    WordGraph *wordGraph = new WordGraph();
    WordNode *wordNode = new WordNode(searchWord, wordGraph);
    wordNode->setFixed(true);
    wordGraph->addNode(wordNode);
    
    int searchTypes[8] = { -HYPERPTR ,  -HYPOPTR, /* -SIMPTR , */ -ENTAILPTR, -VERBGROUP, -CLASSIFICATION, -CLASS, -PERTPTR, - ANTPTR };
    for (int pos = 1; pos <= NUMPARTS; pos++) {
        for (int type = 0; type < 9; type++) {
        
            qDebug() << "before findtheinfo_ds" << pos << " " <<  type;
            SynsetPtr synset = findtheinfo_ds(searchWord.toLatin1().data(),
                                            pos, searchTypes[type], ALLSENSES);
            qDebug() << "after findtheinfo_ds";
            
            int relationshipType = searchTypes[type] > 0 ? searchTypes[type] : - searchTypes[type];
            
            while (synset) {
                SynsetPtr nextSynset = synset->nextss;
                
                MeaningNode *meaning = new MeaningNode(QString::number(pos) + QString::number(synset->hereiam), wordGraph);
                meaning->setPartOfSpeech(pos);
                meaning->setMeaning(synset->defn);
                wordGraph->addNode(meaning);
                Edge *edge = wordGraph->addEdge(wordNode->id(), meaning->id());
    
                for (int i = 0; i < synset->wcount; i++) {
                    if (searchWord != synset->words[i]) {
                        WordNode *word =  new WordNode(synset->words[i], wordGraph);
                        wordGraph->addNode(word);
                        Edge *edge = wordGraph->addEdge(meaning->id(), word->id());
                            
                    }
                }
                
                    synset = synset->ptrlist;
                    if (synset) {//while (synset) {
                        MeaningNode *meaning2 = new MeaningNode(QString::number(pos) + QString::number(synset->hereiam), wordGraph);
                        meaning2->setPartOfSpeech(pos);
                        meaning2->setMeaning(synset->defn);
                        wordGraph->addNode(meaning2);
                        Edge *edge = wordGraph->addEdge(meaning->id(), meaning2->id());
                        
                        if (edge) {
                            edge->setRelationship(relationshipType);
                        }
    
                        for (int i = 0; i < synset->wcount; i++) {
                            if (searchWord != synset->words[i]) {
                                WordNode *word =  new WordNode(synset->words[i], wordGraph);
                                wordGraph->addNode(word);
                                Edge *edge = wordGraph->addEdge(meaning2->id(), word->id());
                            }
                        }
                        synset = synset->nextss;
                        //synset = synset->ptrlist;
                    }
                synset = nextSynset;
            }
        }
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
            if (!file.open(indexfps[1], QIODevice::ReadOnly | QIODevice::Text)) {
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



    

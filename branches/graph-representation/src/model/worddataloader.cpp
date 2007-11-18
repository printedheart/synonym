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

WordGraph * WordDataLoader::createWordGraph(const QString &searchWord) 
{
    TemplateNodeFactory<WordGraphicsNode> wordFactory;
    TemplateNodeFactory<MeaningGraphicsNode> meaningFactory;
    TemplateEdgeFactory<GraphicsEdge> edgeFactory;
 
    WordGraph *wordGraph = new WordGraph();
    Node *wordNode = wordGraph->addNode(searchWord, wordFactory);
    wordNode->setData(WORD, searchWord);
    SynsetPtr synsetToFree;
    
    int searchTypes[8] = { -HYPERPTR ,  -HYPOPTR, /* -SIMPTR ,*/  -ENTAILPTR, -VERBGROUP, -CLASSIFICATION, -CLASS, -PERTPTR, - ANTPTR };
    for (int pos = 1; pos <= NUMPARTS; pos++) {
        for (int type = 0; type < 9; type++) {
        
            SynsetPtr synset = findtheinfo_ds(searchWord.toLatin1().data(),
                                            pos, searchTypes[type], ALLSENSES);
            synsetToFree = synset;
            int relationshipType = searchTypes[type] > 0 ? searchTypes[type] : - searchTypes[type];
            
            while (synset) {
                SynsetPtr nextSynset = synset->nextss;
                
                Node *meaning = wordGraph->addNode(
                        QString::number(pos) + QString::number(synset->hereiam), meaningFactory);
                meaning->setData(POS, QVariant(pos));
                meaning->setData(MEANING, synset->defn);
                Edge *edge = wordGraph->addEdge(wordNode->id(), meaning->id(), edgeFactory);
    
                for (int i = 0; i < synset->wcount; i++) {
                    if (searchWord != synset->words[i]) {
                        Node *word = wordGraph->addNode(synset->words[i], wordFactory);
                        word->setData(WORD, QString(synset->words[i]).replace(QChar('_'), QChar(' ')));
                        Edge *edge = wordGraph->addEdge(meaning->id(), word->id(), edgeFactory);
                        if (edge) edge->setData(RELATIONSHIP, relationshipType);
                            
                    }
                }
                
                synset = synset->ptrlist;
                while (synset) {
                    Node *meaning2 = wordGraph->addNode(
                            QString::number(pos) + QString::number(synset->hereiam), meaningFactory);
                    
                    meaning2->setData(POS, pos);
                    meaning2->setData(MEANING, synset->defn);
                    Edge *edge = wordGraph->addEdge(meaning->id(), meaning2->id(), edgeFactory);
                    
                    if (edge) {
                        edge->setData(RELATIONSHIP, relationshipType);
                    }

                    for (int i = 0; i < synset->wcount; i++) {
                        if (searchWord != synset->words[i]) {
                            Node *word = wordGraph->addNode(synset->words[i], wordFactory);
                            word->setData(WORD, QString(synset->words[i]).replace(QChar('_'), QChar(' ')));
                            Edge *edge = wordGraph->addEdge(meaning2->id(), word->id(), edgeFactory);
                        }
                    }
                    
                    synset = synset->nextss;
                    //synset = synset->ptrlist;
                }
                synset = nextSynset;
            }
        }
    }
    free_syns(synsetToFree);
    
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



    

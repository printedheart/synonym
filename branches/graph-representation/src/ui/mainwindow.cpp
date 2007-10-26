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
#include "mainwindow.h"
#include "graphwidget.h"
#include "graphscene.h"
#include "graphcontroller.h"
#include "partofspeechitemdelegate.h"
#include "worddatagraph.h"
#include "worddataloader.h"
#include "partofspeechlistmodel.h"
#include "pronunciationsoundholder.h"
#include "player.h"
#include <QtGui>
#include <QtCore>
#include <stdlib.h>
#include <wn.h>

        
MainWindow::MainWindow()
 : QMainWindow()
{
    GraphScene *scene = new GraphScene(this);
    m_scene = scene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-500, -600, 1000, 1200);
    m_graphView = new QGraphicsView(scene, this);
    setCentralWidget(m_graphView);

    m_loader = new WordDataLoader(this);
    m_graphController = new GraphController(scene, m_loader);

    m_graphView->setCacheMode(QGraphicsView::CacheBackground);
    m_graphView->setRenderHint(QPainter::Antialiasing);
    m_graphView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    m_graphView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_graphView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_graphView->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    QToolBar *toolBar = addToolBar("synonym");
    m_wordLine = new QLineEdit(toolBar);
    toolBar->addWidget(m_wordLine);

    connect (m_wordLine, SIGNAL(returnPressed()),
            this, SLOT(callLoadWord()));
    connect (scene, SIGNAL(nodeClicked(const QString &)),
            this, SLOT(lookUpWordNet(const QString&)));
    connect (scene, SIGNAL(nodeMouseHovered(const QString&)),
             this, SLOT(nodeActivated(const QString&)));



    // Setup item views.
    QString partsOfSpeechHeaders[4] = { "Nouns", "Verbs", "Adjectives", "Adverbs"};
    WordGraph::PartOfSpeech partsOfSpeech[4] = { WordGraph::Noun, WordGraph::Verb,
        WordGraph::Adjective, WordGraph::Adverb};
                                     
    for (int i = 0; i < 4; i++) {
        QDockWidget *dockWidget = new QDockWidget(partsOfSpeechHeaders[i], this);

        PartOfSpeechItemView *view = new PartOfSpeechItemView(this);
        m_posViews[i] = view;
        
        PartOfSpeechListModel *model =
                new PartOfSpeechListModel(partsOfSpeech[i], this);
        m_posModels[i] = model;
        
        view->setModel(model);
        view->setItemDelegate(new PartOfSpeechItemDelegate(view));
        view->setResizeMode(QListView::Adjust);
        view->setWordWrap(true);
        QFont font = view->font();
        font.setPointSize(8);
        view->setFont(font);
        view->setMouseTracking(true);
        
        dockWidget->setWidget(view);
        addDockWidget(Qt::RightDockWidgetArea, dockWidget);        
        connect (view, SIGNAL(entered(const QModelIndex&)),
                 this, SLOT(nodeActivated(const QModelIndex&)));
        connect (scene, SIGNAL(nodeMouseHoverLeaved(const QString&)),
                 view, SLOT(clearHighlighting()));
    }



    //Sound setup
    m_soundHolder = new PronunciationSoundHolder(this);
    connect (m_soundHolder, SIGNAL(soundReady(const QString&)),
             m_graphController, SLOT(soundReady(const QString&)));

    connect(m_scene, SIGNAL(soundButtonClicked(const QString&)),
            this, SLOT(playSound(const QString&)));
    
    
   initCompleter();  
}


MainWindow::~MainWindow()
{
    
}

void MainWindow::callLoadWord()
{
    QString word = m_wordLine->text();
    if (word.length() != 0) {
        lookUpWordNet(word);
    }       
}

void MainWindow::lookUpWordNet(const QString &word)
{
    WordGraph *dataGraph = m_graphController->makeGraph(word);
    m_currentGraph = dataGraph;
    for (int i = 0; i < 4; i++)
        m_posModels[i]->setDataGraph(dataGraph);

    m_soundHolder->findPronunciation(word);
        
}

void MainWindow::playSound(const QString &word)
{
    Player player;
    QIODevice *sound = m_soundHolder->pronunciationSound(word);
    if (sound) {
        player.play(sound);
    }
}

void MainWindow::nodeActivated(const QModelIndex &index)
{
    PartOfSpeechListModel *indexModel = qobject_cast<PartOfSpeechListModel*>(
            const_cast<QAbstractItemModel*>(index.model()));
    Node *node = 0;
    node = indexModel->nodeAt(index);
    if (!node) {
        m_scene->setActivated(QString());
        return;
    }
    m_scene->setActivated(node->id());
}

void MainWindow::nodeActivated(const QString &id)
{
    Node *node = m_currentGraph->node(id);
    if (node) {
        QVariant meaning = node->data(MEANING);
        if (meaning.isValid()) {
            int partOfSpeech = node->data(POS).toInt();
            QModelIndex nodeIndex = m_posModels[partOfSpeech - 1]->indexForNode(node);
            m_posViews[partOfSpeech - 1]->highlightItem(nodeIndex);
        }
    }
}

void MainWindow::initCompleter()
{

    QStringList words = m_loader->words();   
    QCompleter *completer = new QCompleter(words, this);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_wordLine->setCompleter(completer);
}
    




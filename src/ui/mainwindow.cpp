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
#include "../model/worddatagraph.h"
#include  "../model/worddataloader.h"
#include "../model/node.h"
#include "../model/partofspeechlistmodel.h"
#include <QtGui>
#include <QtCore>
                        

        
MainWindow::MainWindow()
 : QMainWindow()
{
    GraphScene *scene = new GraphScene(this);
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



    // Setup item views.
    QString partsOfSpeechHeaders[4] = { "Nouns", "Adjectives", "Verbs", "Adverbs"};
    MeaningNode::PartOfSpeech partsOfSpeech[4] = { MeaningNode::Noun, MeaningNode::Adjective,
        MeaningNode::Verb, MeaningNode::Adverb};
                                     
    for (int i = 0; i < 4; i++) {
        QDockWidget *dockWidget = new QDockWidget(partsOfSpeechHeaders[i], this);

        QListView *view = new PartOfSpeechItemView(this);
        m_posViews[i] = view;
        
        PartOfSpeechListModel *model =
                new PartOfSpeechListModel(partsOfSpeech[i], this);
        m_posModels[i] = model;
        
        view->setModel(model);
        view->setItemDelegate(new PartOfSpeechItemDelegate(view));
        view->setResizeMode(QListView::Adjust);
        view->setWordWrap(true);
        QFont font = view->font();
        font.setPointSize(10);
        view->setFont(font);
        
        dockWidget->setWidget(view);
        addDockWidget(Qt::RightDockWidgetArea, dockWidget);        
    } 

    
     
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
    WordDataGraph *dataGraph = m_graphController->makeGraph(word);
    for (int i = 0; i < 4; i++)
        m_posModels[i]->setDataGraph(dataGraph);    
}



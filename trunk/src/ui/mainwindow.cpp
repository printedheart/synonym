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
 **************************************************************************/
#include "mainwindow.h"
#include "graphwidget.h"
#include "graphscene.h"
#include "graphcontroller.h"
#include "partofspeechitemdelegate.h"
#include "wordgraph.h"
#include "partofspeechlistmodel.h"
#include "wordnetutil.h"
#include "relation.h"
#include "configdialog.h"
#include "tglayout.h"
#include "dbdataloader.h"
#include "audiopronunciationloaderfactory.h"
#include "audiopronunciationloader.h"

#include <QtGui>
#include <QtCore>

#include <phonon/audiooutput.h>



        
MainWindow::MainWindow()
 : QMainWindow(), m_currentGraph(0), soundLoader(0)
{
    //m_layout = new ForceDirectedLayout();
    m_layout = new TGLayout();
    GraphScene *scene = new GraphScene(m_layout, this);
    m_scene = scene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-800, -800, 1600, 1600);
    
    m_graphView = new QGraphicsView(scene, this);
    setCentralWidget(m_graphView);

    m_loader = new DbDataLoader(this);
    m_graphController = new GraphController(scene, m_loader);

    m_graphView->setCacheMode(QGraphicsView::CacheBackground);
    m_graphView->setRenderHint(QPainter::Antialiasing);
    m_graphView->setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    m_graphView->setOptimizationFlag(QGraphicsView::DontClipPainter);
    
    m_graphView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    m_graphView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    m_graphView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    

    QToolBar *toolBar = addToolBar("synonym");
    
    QPushButton *backButton = new QPushButton(toolBar);
    backButton->setIcon(style()->standardIcon(QStyle::QStyle::SP_ArrowLeft));
    QPushButton *forwardButton = new QPushButton("Forward", toolBar);
    toolBar->addWidget(backButton);
    toolBar->addWidget(forwardButton);
    toolBar->addSeparator();
    connect (backButton, SIGNAL(clicked()), this, SLOT(slotBack()));
    connect (forwardButton, SIGNAL(clicked()), this, SLOT(slotForward()));
    
    QLabel *label = new QLabel(" Enter a Word: ", toolBar);
    toolBar->addWidget(label);
    m_wordLine = new QLineEdit(toolBar);
    m_wordLine->setObjectName("inputline");
    m_wordLine->setFixedWidth(300);
    toolBar->addWidget(m_wordLine);
    QAction *searchAct = toolBar->addAction(QIcon(":resources/synonymicon3.png"), "Search");
    addToolBar(" ");
        
    connect (searchAct, SIGNAL(triggered()), this, SLOT(callLoadWord()));
    connect (m_wordLine, SIGNAL(returnPressed()), this, SLOT(callLoadWord()));
    connect (scene, SIGNAL(nodeClicked(const QString &)),this, SLOT(lookUpWordNet(const QString&)));
    connect (scene, SIGNAL(nodeMouseHovered(const QString&)),this, SLOT(nodeActivated(const QString&)));



    // Setup item views.
    QString partsOfSpeechHeaders[4] = { "Nouns", "Verbs", "Adjectives", "Adverbs"};
    PartOfSpeech partsOfSpeech[4] = { Noun, Verb, Adjective, Adverb};
                                     
    for (int i = 0; i < 4; i++) {
        QDockWidget *dockWidget = new QDockWidget(partsOfSpeechHeaders[i], this);
        

        PartOfSpeechItemView *view = new PartOfSpeechItemView(dockWidget);
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
        connect (dockWidget, SIGNAL(visibilityChanged(bool)), 
                 this, SLOT(dockWidgetVisibilityChanged()));
        connect (view, SIGNAL(clicked(const QModelIndex&)),
                 this, SLOT(nodeClicked(const QModelIndex&)));
        
    }

    
   createActions();
   createMenus();
  
   //Zack's Rusin advice. http://developer.kde.org/documentation/other/mistakes.html
   QTimer::singleShot(10, this, SLOT(initServices()));
   m_layout->start();
}


MainWindow::~MainWindow()
{
}


void MainWindow::initServices()
{
    configure();
    initSound();
    initCompleter();
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
    if (dataGraph) 
        setNewGraph(dataGraph);
    
}

void MainWindow::nodeActivated(const QModelIndex &index)
{
    PartOfSpeechListModel *indexModel = 
            qobject_cast<PartOfSpeechListModel*>(const_cast<QAbstractItemModel*>(index.model()));
    Node *node = 0;
    node = indexModel->nodeAt(index);
    if (!node) {
        m_scene->setActivated(QString());
        return;
    }
    m_scene->setActivated(node->id());
}

void MainWindow::nodeClicked(const QModelIndex &index)
{
    PartOfSpeechListModel *indexModel = 
            qobject_cast<PartOfSpeechListModel*>(const_cast<QAbstractItemModel*>(index.model()));
    
    Node *node = 0;
    node = indexModel->nodeAt(index);
    if (node) 
        lookUpWordNet(node->id());
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

class CompleterLoader : public QThread
{
Q_OBJECT
public: 
    CompleterLoader(QObject *parent, IWordDataLoader *loader):
        QThread(parent), m_loader(loader) {}
    void run() {
        words = m_loader->words();
    }
               
    QStringList words;
    IWordDataLoader *m_loader;
};

void MainWindow::initCompleter()
{
    CompleterLoader *l = new CompleterLoader(this, m_loader);
    l->setObjectName("completerloader");
    connect (l, SIGNAL(finished()), this, SLOT(completerLoaderFinished()));
    l->start();
}
    
void MainWindow::completerLoaderFinished()
{
    CompleterLoader *l = findChild<CompleterLoader*>("completerloader");
    QStringList words = l->words;
    delete l;   
    
    QCompleter *completer = new QCompleter(words, this);
    completer->setModelSorting(QCompleter::CaseSensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_wordLine->setCompleter(completer);    
}    


void MainWindow::dockWidgetVisibilityChanged()
{
    QList<PartOfSpeech> poses;
    for (int i = 0; i < 4; ++i) {
        QDockWidget *dock = qobject_cast<QDockWidget*>(m_posViews[i]->parent());
        if (dock->isVisible()) {
            poses.append(m_posModels[i]->modelType());
        }
    }
    m_graphController->setPoses(poses);
}


void MainWindow::configure()
{    
    QSettings settings("http://code.google.com/p/synonym/", "synonym");
    if (settings.childGroups().contains("relations")) {
        Relation::Types userTypes;
        settings.beginGroup("relations");
        QStringList keys = settings.childKeys();
        
        foreach (Relation::Type type, Relation::types()) {
            QString typeString = Relation::toString(type);
            if (keys.contains(typeString) && settings.value(typeString).toBool()) {
                userTypes |= type;
            }
        }
        settings.endGroup();
        m_graphController->setrelations(userTypes);
    } else {
        m_graphController->setrelations(Relation::allTypes());
    }
    if (settings.childGroups().contains("display")) {
        settings.beginGroup("display");
        int edgeRestDistance = settings.value("Edge Length").toInt();
        m_layout->setRestDistance(edgeRestDistance);
        
        const QColor posColors[4] = {
            QColor(settings.value("Noun Color").toString().trimmed()), 
            QColor(settings.value("Verb Color").toString().trimmed()),
            QColor(settings.value("Adjective Color").toString().trimmed()),
            QColor(settings.value("Adverb Color").toString().trimmed())
        };
        for (int i = 0; i < 4; i++) {
            QString style = "QDockWidget::title {text-align: center; background: " + posColors[i].name() + ";}";
            qobject_cast<QWidget*>(m_posViews[i]->parent())->setStyleSheet(style);
        }

    }
}

void MainWindow::slotBack()
{
    WordGraph * graph = m_graphController->previousGraph();
    if (graph)
        setNewGraph(graph);
}


void MainWindow::slotForward()
{
    WordGraph * graph = m_graphController->nextGraph();
    if (graph)
        setNewGraph(graph);
}

void MainWindow::setNewGraph(WordGraph *graph)
{
    m_currentGraph = graph;
    for (int i = 0; i < 4; i++)
        m_posModels[i]->setDataGraph(graph);
    loadSound();
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(quitAct);
    
    m_settingsMenu = menuBar()->addMenu(tr("&Settings"));
    m_settingsMenu->addAction(m_settingsAct);
}

void MainWindow::createActions()
{
    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setStatusTip(tr("Quit the application"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));
    
    m_settingsAct = new QAction(tr("&Settings"), this);
    m_settingsAct->setStatusTip(tr("Settings"));
    connect(m_settingsAct, SIGNAL(triggered()), this, SLOT(showConfigDialog()));
}

void MainWindow::showConfigDialog()
{
    ConfigDialog dialog(m_loader);
    dialog.exec();
    if (dialog.settingsChanged()) {
        configure();
        AudioPronunciationLoaderFactory::instance()->reconfigure();
    }
    if (m_currentGraph) {
        m_graphController->makeGraph(m_currentGraph->centralNode()->id());
    }
        
}

static QObject *__soundLoaderThread__;
static void cleanupRoutine()
{
    delete __soundLoaderThread__;
}

class SoundLoaderThread : public QThread
{
Q_OBJECT    
public:    
    SoundLoaderThread(QObject *parent, AudioPronunciationLoader *loader) 
        : QThread(parent) {
        m_loader = loader;
        connect (loader, SIGNAL(soundLoaded(const Phonon::MediaSource&)),
                 this, SLOT(slotSoundLoaded(const Phonon::MediaSource&)));
        connect (loader, SIGNAL(soundLoaded(const QString&)),
                 this, SLOT(slotSoundLoaded(const QString&)));
    }
    
    ~SoundLoaderThread() {
        exit(0);
        while (isRunning()) 
            wait(1);
        delete m_loader;
    }
    
    void run() {
        exec();
    }
    
signals:    
    void soundLoaded(const Phonon::MediaSource&);
    void soundLoaded(const QString&);
    
public slots:    
    void loadSound(const QString &word) {
        m_loader->loadAudio(word);
    }
    
    void slotSoundLoaded(const Phonon::MediaSource& source) {
        emit soundLoaded(source);
    }
    
    void slotSoundLoaded(const QString &filename) {
        emit soundLoaded(filename);
    }
    
private:    
    AudioPronunciationLoader *m_loader;    
};


Q_DECLARE_METATYPE(Phonon::MediaSource)

void MainWindow::initSound()
{
    mediaObject = new Phonon::MediaObject(this);
    
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(mediaObject, audioOutput);
    soundLoader = AudioPronunciationLoaderFactory::instance()->createAudioLoader();
    if (soundLoader) {        
        connect (m_scene, SIGNAL(soundButtonClicked()), this, SLOT(play()));
        qRegisterMetaType<Phonon::MediaSource>();
        SoundLoaderThread *loaderThread = new SoundLoaderThread(0, soundLoader);
        loaderThread->start(QThread::LowestPriority);
        loaderThread->moveToThread(loaderThread);
        soundLoader->moveToThread(loaderThread);
        
        connect (this, SIGNAL(audioRequested(const QString&)), 
                 loaderThread, SLOT(loadSound(const QString&)), Qt::QueuedConnection);
        
        connect (loaderThread, SIGNAL(soundLoaded(const Phonon::MediaSource&)),
                 this, SLOT(soundLoaded(const Phonon::MediaSource&)), Qt::QueuedConnection);
        connect (loaderThread, SIGNAL(soundLoaded(const QString&)),
                 this, SLOT(soundLoaded(const QString&)),  Qt::QueuedConnection);
        __soundLoaderThread__ = loaderThread;
        qAddPostRoutine(cleanupRoutine);
    }
    
}

void MainWindow::soundLoaded(const QString &fileName)
{
    Phonon::MediaSource source(fileName);
    source.setAutoDelete(true);
    mediaObject->setCurrentSource(source);
    m_scene->displaySoundIcon();
}

void MainWindow::soundLoaded(const Phonon::MediaSource &sound)
{
    bool playAfterSet = mediaObject->state() == Phonon::ErrorState;
    mediaObject->clearQueue();
    mediaObject->setCurrentSource(sound);
    m_scene->displaySoundIcon();
    if (playAfterSet)
        play();
}


void MainWindow::play()
{
    if (mediaObject->state() == Phonon::ErrorState) {
        qDebug() << mediaObject->errorString();
        emit audioRequested(m_currentGraph->centralNode()->id());
    }
    else if (mediaObject->state() == Phonon::PlayingState) {
        mediaObject->stop();
        mediaObject->seek(0);
        mediaObject->play();
    } else {
        if (mediaObject->currentTime() == mediaObject->totalTime()) 
            mediaObject->seek(0);
        mediaObject->play();    
    }
}

bool MainWindow::soundAvailable() 
{
    return soundLoader != 0;
}

void MainWindow::loadSound()
{
    if (soundAvailable()) {
        GraphicsNode *centralNode = m_currentGraph->centralNode();
        if (dynamic_cast<WordGraphicsNode*>(centralNode)) { 
            mediaObject->stop();
            mediaObject->clearQueue();
            emit audioRequested(centralNode->id());
        }
    }    
}


#include "mainwindow.moc"
                 
                 
                 

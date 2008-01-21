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


#include "pythondataloader.h"
#include "wordgraph.h"
#include "wordnetutil.h"
#include "wordgraphdecorator.h"

#include "PythonQt.h"


#include <QApplication>
#include <QTextBrowser>
#include <QLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QObject>


PythonDataLoader::PythonDataLoader(QObject *parent)
    : QObject(parent), IWordDataLoader()
{
    QFile("/home/sergey/devel/graphs/trunk/synonym/src/scripts/loadgraph.pyc").remove();    
    m_graphDecorator = new WordGraphDecorator();
    m_graphDecorator->setMeaningNodeFactory(new TemplateNodeFactory<MeaningGraphicsNode>());
    m_graphDecorator->setWordNodeFactory(new TemplateNodeFactory<WordGraphicsNode>());
    m_graphDecorator->setEdgeFactory(new TemplateEdgeFactory<GraphicsEdge>());
    
    
    PythonQt::init();//PythonQt::IgnoreSiteModule | PythonQt::RedirectStdOut);
    
    m_mainContext = PythonQt::self()->getMainModule();
    
    connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), this, SLOT(stdOut(const QString&)));
    connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), this, SLOT(stdOut(const QString&)));
    
    PythonQt::self()->addDecorators(m_graphDecorator);
    PythonQt::self()->registerClass(&WordGraph::staticMetaObject);
    m_mainContext.evalFile("/home/sergey/devel/graphs/trunk/synonym/src/scripts/loadgraph.py");    
    
}
    
PythonDataLoader::~PythonDataLoader()
{
    QFile("/home/sergey/devel/graphs/trunk/synonym/src/scripts/loadgraph.pyc").remove();    
}
    
WordGraph * PythonDataLoader::createWordGraph(const QString &searchWord, Relation::Types searchTypes)
{
    QFile("/home/sergey/devel/graphs/trunk/synonym/src/scripts/loadgraph.pyc").remove(); 
    m_mainContext.evalFile("/home/sergey/devel/graphs/trunk/synonym/src/scripts/loadgraph.py");
    m_graphDecorator->setRelationTypes(searchTypes);
    
    m_mainContext.call("createGraph", QVariantList() << searchWord);
    
    WordGraph *wordGraph = m_graphDecorator->wordGraph();
    WordGraph *clone = wordGraph->clone();
    delete wordGraph;
    return clone;
}



     
QStringList PythonDataLoader::words() const
{
    return QStringList();
}

void PythonDataLoader::stdOut(const QString &str)
{
    qDebug() << str;
}

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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

        
#include <QMainWindow>
class QGraphicsView;
class QLineEdit;
class WordGraph;
class WordDataLoader;
class GraphController;
class QListView;
class PartOfSpeechListModel;
class PartOfSpeechItemView;
class PronunciationSoundHolder;
class GraphScene;
class QModelIndex;

/**
    @author Sergejs <sergey.melderis@gmail.com>
*/
class MainWindow : public QMainWindow
{
Q_OBJECT    
public:
    MainWindow();

    ~MainWindow();
    
public slots:    
    void nodeActivated(const QModelIndex &index);
    void nodeActivated(const QString &id);
private:
    QGraphicsView *m_graphView;
    GraphScene *m_scene;
    WordDataLoader *m_loader;
    GraphController *m_graphController;
       
    QLineEdit *m_wordLine;

    PartOfSpeechItemView *m_posViews[4];
    PartOfSpeechListModel *m_posModels[4];
    
    
    PronunciationSoundHolder *m_soundHolder;
private slots:
    void callLoadWord();

    void lookUpWordNet(const QString &word);

    void playSound(const QString &word);

private:
    WordGraph *m_currentGraph;    
    
    
    void initCompleter();
   

};

#endif

/****************************************************************************
**
** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
** 
** Trolltech ASA (c) 2007
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "wordnetutil.h"

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QSettings;
class QPushButton;



class SettingsPage : public QWidget
{
Q_OBJECT    
public:    
    SettingsPage(QSettings *settings, QWidget *parent = 0);
    virtual ~SettingsPage();
    
    virtual void writeSettings() = 0;

signals:    
    void settingsChanged(SettingsPage *page);
protected:    
    QSettings* settings() { return m_settings; }
private:
    QSettings *m_settings;
};


class IWordDataLoader;

class ConfigDialog : public QDialog
{
Q_OBJECT

public:
    ConfigDialog(IWordDataLoader *loader);

    bool settingsChanged();
public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);
    void settingsChanged(SettingsPage *page);

   
private slots:
    void applySettings();    
private:
    void createIcons();

    QListWidget *contentsWidget;
    QStackedWidget *pagesWidget;
    
    QPushButton *m_applyButton;
    QList<SettingsPage*> m_changedPages;
    
    QSettings *m_settings;
    bool m_settingsChanged;
};


class QTableWidget;

class RelationPage : public SettingsPage
{
Q_OBJECT    
public:    
    RelationPage(QSettings *settings, QWidget *parent = 0);
    ~RelationPage();
    
    void writeSettings();
    
private slots:    
    void slotTableChanged();
private:    
    
    QTableWidget *relationTable;
};

class GraphScene;
class TGLayout;
class GraphController;

#include "ui_displayconfig.h"

class DisplayPage : public SettingsPage
{
Q_OBJECT
public:            
    DisplayPage(QSettings *settings, IWordDataLoader *loader,  QWidget *parent = 0);
    ~DisplayPage();
    
    void writeSettings();
        
private slots:    
    void fontChanged();
    void circleSizeChanged(int value);
    
    void chooseNounColor();
    void chooseVerbColor();
    void chooseAdjColor();
    void chooseAdvColor();
    
    void edgeLenghChanged(int value);
    void edgeWidthChanged(int value);
    void edgeContrastChanged(int value);
    
    void loadWord();
    
private:    
    GraphScene *m_graphScene;    
    TGLayout *m_layout;
    GraphController *controller;
    Ui::DisplayPageUi ui;
    
    void chooseAndSetColor(QPushButton *button, PartOfSpeech pos);
    void setColorToMeaningNodes(const QList<GraphicsNode*> &nodes, PartOfSpeech pos, const QColor &color);
    void updatePreview();
    void initializeFromSettings();
};
#endif

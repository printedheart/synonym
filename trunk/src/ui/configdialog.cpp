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

#include <QtGui>
#include <QtCore>
#include "configdialog.h"
#include "relation.h"
#include "graphscene.h"
#include "tglayout.h"
#include "graphcontroller.h"
#include "audioscriptpage.h"



SettingsPage::SettingsPage(QSettings *settings,    QWidget *parent)
    : QWidget(parent), m_settings(settings)
{}

SettingsPage::~SettingsPage()
{}


static void addrelationItem(QTableWidget *tableWidget, int row,  QString &item, bool selected)
{
    QTableWidgetItem *keyItem = new QTableWidgetItem(item);
    QTableWidgetItem *valueItem = new QTableWidgetItem();
    valueItem->setCheckState(selected ? Qt::Checked : Qt::Unchecked);
    valueItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    tableWidget->setItem(row, 0, keyItem);
    tableWidget->setItem(row, 1, valueItem);
}


RelationPage::RelationPage(QSettings *settings, QWidget *parent)
    : SettingsPage(settings, parent)
{
    relationTable = new QTableWidget(Relation::types().size(), 2, this);
    relationTable->setSortingEnabled(false);
    QList<Relation::Type> types = Relation::types();
    QStringList typeLabels;
    foreach (Relation::Type type, types) 
        typeLabels << Relation::toString(type);
    
    int insertRow = 0;
    if (settings->childGroups().contains("relations")) {
        settings->beginGroup("relations");
        QStringList keys = settings->childKeys();
        foreach (QString rel, keys) {
            if (typeLabels.contains(rel)) {
                bool selected = settings->value(rel, true).toBool();
                addrelationItem(relationTable, insertRow++,  rel, selected);
            }
        }
        settings->endGroup();
    }
    
    // Verify that no relation type is missing in the settings.
    // If a type is missed add it to the table.
    foreach (QString rel, typeLabels) {
        if (!settings->childKeys().contains(rel)) {
            addrelationItem(relationTable, insertRow++, rel, true);
        }
    }
    
    
    connect(relationTable, SIGNAL(cellChanged(int,int)),
            this, SLOT(slotTableChanged()));
    
    // Hack to get rid of outline around text label of the checkbox on focus, 
    // because we don't have any labels.
    class CheckBoxDelegate : public QItemDelegate {
        public:
            CheckBoxDelegate(QObject *parent) : QItemDelegate(parent) {}

            void drawFocus(QPainter * /*p*/, const QStyleOptionViewItem & /*o*/, const QRect &/*r*/) const {}
    };
    relationTable->setItemDelegateForColumn(1, new CheckBoxDelegate(relationTable));
    relationTable->setColumnWidth(1, 30);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(relationTable);
    setLayout(mainLayout);
    
    
}

RelationPage::~RelationPage()
{}

void RelationPage::slotTableChanged()
{
    emit settingsChanged(this);
}

void RelationPage::writeSettings()
{
    settings()->beginGroup("relations");
    for (int row = 0; row < relationTable->rowCount(); row++) {
        QString key = relationTable->item(row, 0)->data(Qt::DisplayRole).toString();
        bool selected = relationTable->item(row, 1)->checkState() == Qt::Checked;
        settings()->setValue(key, selected);
    }
    settings()->endGroup();    
}


DisplayPage::DisplayPage(QSettings *settings, IWordDataLoader *loader,  QWidget *parent)
    : SettingsPage(settings, parent)
{
    ui.setupUi(this);
    m_layout = new TGLayout(); //new ForceDirectedLayout(this->parent());
    m_layout->start();
    m_graphScene = new GraphScene(m_layout);
    m_graphScene->setObjectName("config");
    m_graphScene->setItemIndexMethod(QGraphicsScene::NoIndex);
    m_graphScene->setSceneRect(-500, -500, 1000, 1000);
    ui.graphicsView->setScene(m_graphScene);
    ui.graphicsView->setInteractive(false);
    ui.graphicsView->setRenderHint(QPainter::Antialiasing);
    ui.graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    
    connect (ui.fontComboBox, SIGNAL(currentFontChanged(const QFont&)), this, SLOT(fontChanged()));
    connect (ui.fontSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(fontChanged()));
    connect (ui.circleSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(circleSizeChanged(int)));
    
    connect (ui.nounColorButton, SIGNAL(clicked()), this, SLOT(chooseNounColor()));
    connect (ui.verbColorButton, SIGNAL(clicked()), this, SLOT(chooseVerbColor()));
    connect (ui.adjColorButton, SIGNAL(clicked()), this, SLOT(chooseAdjColor()));
    connect (ui.advColorButton, SIGNAL(clicked()), this, SLOT(chooseAdvColor()));
    
    connect (ui.edgeWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(edgeWidthChanged(int)));
    connect (ui.edgeLenghSlider, SIGNAL(valueChanged(int)), this, SLOT(edgeLenghChanged(int)));
    connect (ui.lineContrastSlider, SIGNAL(valueChanged(int)), this, SLOT(edgeContrastChanged(int)));
    
    if (settings->childGroups().contains("display"))
        initializeFromSettings();
    
    controller = new GraphController(m_graphScene, loader,  this);
    
    QTimer::singleShot(100, this, SLOT(loadWord()));
}

DisplayPage::~DisplayPage()
{
    m_layout->stop();   
    m_graphScene->deleteLater();
}
static void setBackgroundColorToButton(const QColor &color, QPushButton *button)
{
    if (color.isValid())
        button->setStyleSheet("background-color: " + color.name());
}

void DisplayPage::initializeFromSettings()
{
    settings()->beginGroup("display");
    QFont font;
    font.setFamily(settings()->value("Font Family").toString());
    ui.fontComboBox->setCurrentFont(font);
    ui.fontSizeSpinBox->setValue(settings()->value("Font Size").toInt());
    ui.circleSizeSlider->setValue(settings()->value("Circle Radius").toInt());
    
    setBackgroundColorToButton(QColor(settings()->value("Noun Color").toString().trimmed()), ui.nounColorButton);
    setBackgroundColorToButton(QColor(settings()->value("Verb Color").toString().trimmed()), ui.verbColorButton);
    setBackgroundColorToButton(QColor(settings()->value("Adjective Color").toString().trimmed()), ui.adjColorButton);
    setBackgroundColorToButton(QColor(settings()->value("Adverb Color").toString().trimmed()), ui.advColorButton);
    
    ui.edgeLenghSlider->setValue(settings()->value("Edge Length").toInt());
    ui.edgeWidthSlider->setValue(settings()->value("Edge Width").toInt());
    ui.lineContrastSlider->setValue(settings()->value("Edge Contrast").toInt());
    settings()->endGroup();    
}

void DisplayPage::loadWord()
{
    controller->makeGraph("down");
}

void DisplayPage::updatePreview()
{
    m_graphScene->update(m_graphScene->sceneRect());
}

void DisplayPage::fontChanged()
{
    QFont font = ui.fontComboBox->currentFont();
    font.setPointSize(ui.fontSizeSpinBox->value());
    QList<GraphicsNode*> nodes = m_graphScene->graphNodes();
    foreach (GraphicsNode *node, nodes) {
        node->setFont(font);
         
    }
    updatePreview();
    emit settingsChanged(this);
}


void DisplayPage::setColorToMeaningNodes(const QList<GraphicsNode*> &nodes, PartOfSpeech pos, const QColor &color)
{
    foreach (GraphicsNode *node, nodes) {
        if (node->data(POS) == pos) {
            static_cast<MeaningGraphicsNode*>(node)->setCircleColor(color);
        }
    }
    updatePreview();
    emit settingsChanged(this);
}

void DisplayPage::chooseAndSetColor(QPushButton *button, PartOfSpeech pos)
{
    QColor color = QColorDialog::getColor(QColor(button->styleSheet().section(':', 1, -1).trimmed()));
    if (!color.isValid())
        return;
    setBackgroundColorToButton(color, button);
    setColorToMeaningNodes(m_graphScene->graphNodes(), pos, color);
}

void DisplayPage::chooseNounColor()
{
    chooseAndSetColor(ui.nounColorButton, Noun);
}
    
void DisplayPage::chooseVerbColor()
{
    chooseAndSetColor(ui.verbColorButton, Verb);
}
    

void DisplayPage::chooseAdjColor()
{
    chooseAndSetColor(ui.adjColorButton, Adjective);
}
    
void DisplayPage::chooseAdvColor()
{
    chooseAndSetColor(ui.advColorButton, Adverb);
}


void DisplayPage::circleSizeChanged(int value)
{
    QList<GraphicsNode*> nodes = m_graphScene->graphNodes();
    foreach (GraphicsNode *node, nodes) {
        if (MeaningGraphicsNode *meaningNode = dynamic_cast<MeaningGraphicsNode*>(node)) {
            meaningNode->setCircleRadius(value);
        }
    } 
    updatePreview();
    emit settingsChanged(this);
}


void DisplayPage::edgeLenghChanged(int value)
{
    m_layout->setRestDistance(value);
    m_layout->resetDamper();
    m_graphScene->itemMoved();
    emit settingsChanged(this);
}


void DisplayPage::edgeWidthChanged(int value)
{
    QList<GraphicsEdge*> edges = m_graphScene->graphEdges();
    foreach (GraphicsEdge *edge, edges)
        edge->setLineWidth((double) value / 10);
    updatePreview();
    emit settingsChanged(this);
}


void DisplayPage::edgeContrastChanged(int value)
{
    QList<GraphicsEdge*> edges = m_graphScene->graphEdges();
    foreach (GraphicsEdge *edge, edges)
        edge->setLineContrast(value);
    updatePreview();
    emit settingsChanged(this);
}

void DisplayPage::writeSettings()
{
    settings()->beginGroup("display");
    settings()->setValue("Font Family",  ui.fontComboBox->currentFont().family());
    settings()->setValue("Font Size", ui.fontSizeSpinBox->value());
    settings()->setValue("Circle Radius", ui.circleSizeSlider->value());
    settings()->setValue("Noun Color", QVariant(ui.nounColorButton->styleSheet().section(':', 1, -1)));
    settings()->setValue("Verb Color", ui.verbColorButton->styleSheet().section(':', 1, -1));
    settings()->setValue("Adjective Color", ui.adjColorButton->styleSheet().section(':', 1, -1));
    settings()->setValue("Adverb Color", ui.advColorButton->styleSheet().section(':', 1, -1));
    settings()->setValue("Edge Length", ui.edgeLenghSlider->value());
    settings()->setValue("Edge Width", ui.edgeWidthSlider->value());
    settings()->setValue("Edge Contrast", ui.lineContrastSlider->value());
    settings()->endGroup();
}


ConfigDialog::ConfigDialog(IWordDataLoader *loader)
    : m_settingsChanged(false)
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::ListMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setMinimumWidth(128);
    contentsWidget->setSpacing(12);

    m_settings =  new QSettings("http://code.google.com/p/synonym/", "synonym");
    
    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(new RelationPage(m_settings));
    pagesWidget->addWidget(new DisplayPage(m_settings, loader));
    pagesWidget->addWidget(new AudioScriptPage(m_settings));

    m_applyButton = new QPushButton(tr("Apply"));
    m_applyButton->setEnabled(false);
    QPushButton *closeButton = new QPushButton(tr("Close"));
 
    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(m_applyButton);
    buttonsLayout->addWidget(closeButton);
    
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(contentsWidget, 0, 0, 2, 1);
    mainLayout->addWidget(pagesWidget, 0, 1);
    mainLayout->addLayout(buttonsLayout, 1, 1);
    setLayout(mainLayout);

    setWindowTitle(tr("Config Dialog"));
    
    for (int i = 0; i < pagesWidget->count(); i++) {
        SettingsPage *page = qobject_cast<SettingsPage*>(pagesWidget->widget(i));
        connect (page, SIGNAL(settingsChanged(SettingsPage*)), 
                 this, SLOT(settingsChanged(SettingsPage*)));
    }
    qDebug() << pagesWidget->width();
    qDebug() << pagesWidget->minimumWidth();
}



void ConfigDialog::createIcons()
{
    QListWidgetItem *relationButton = new QListWidgetItem(contentsWidget);
    relationButton->setText(tr("Relations"));
    relationButton->setTextAlignment(Qt::AlignHCenter);
    relationButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


    QListWidgetItem *displayButton = new QListWidgetItem(contentsWidget);
    displayButton->setText(tr("Display"));
    displayButton->setTextAlignment(Qt::AlignHCenter);
    displayButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *audioScriptButton = new QListWidgetItem(contentsWidget);
    audioScriptButton->setText(tr("Audio"));
    audioScriptButton->setTextAlignment(Qt::AlignHCenter);
    audioScriptButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    
    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
                   this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

void ConfigDialog::settingsChanged(SettingsPage *page)
{
    m_changedPages.append(page);
    m_applyButton->setEnabled(true);
}

void ConfigDialog::applySettings()
{
    foreach (SettingsPage *page, m_changedPages) 
        page->writeSettings();
    
    m_settings->sync();
    m_changedPages.clear();
    m_settingsChanged = true;
    m_applyButton->setEnabled(false);
}


bool ConfigDialog::settingsChanged()
{
    return m_settingsChanged;
}


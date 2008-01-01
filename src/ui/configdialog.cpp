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
#include "relationship.h"

SettingsPage::SettingsPage(QSettings *settings,    QWidget *parent)
    : QWidget(parent), m_settings(settings)
{}

SettingsPage::~SettingsPage()
{}


static void addRelationshipItem(QTableWidget *tableWidget, int row,  QString &item, bool selected)
{
    QTableWidgetItem *keyItem = new QTableWidgetItem(item);
    QTableWidgetItem *valueItem = new QTableWidgetItem();
    valueItem->setCheckState(selected ? Qt::Checked : Qt::Unchecked);
    valueItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    tableWidget->setItem(row, 0, keyItem);
    tableWidget->setItem(row, 1, valueItem);

}


RelationshipPage::RelationshipPage(QSettings *settings, QWidget *parent)
    : SettingsPage(settings, parent)
{
    relationshipTable = new QTableWidget(Relationship::types().size(), 2, this);
    relationshipTable->setSortingEnabled(false);
    QList<Relationship::Type> types = Relationship::types();
    QStringList typeLabels;
    foreach (Relationship::Type type, types) 
        typeLabels << Relationship::toString(type);
    
    int insertRow = 0;
    if (settings->childGroups().contains("relationships")) {
        settings->beginGroup("relationships");
        QStringList keys = settings->childKeys();
        foreach (QString rel, keys) {
            if (typeLabels.contains(rel)) {
                bool selected = settings->value(rel, true).toBool();
                addRelationshipItem(relationshipTable, insertRow++,  rel, selected);
            }
        }
    }
    
    // Verify that no relationship type is missing in the settings.
    // If a type is missed add it to the table.
    foreach (QString rel, typeLabels) {
        if (!settings->childKeys().contains(rel)) {
            addRelationshipItem(relationshipTable, insertRow++, rel, true);
        }
    }
    m_settings->endGroup();
    
    connect(relationshipTable, SIGNAL(cellChanged(int,int)),
            this, SLOT(slotTableChanged()));
    
    // Hack to get rid of outline around text label of the checkbox on focus, 
    // because we don't have any labels.
    class CheckBoxDelegate : public QItemDelegate {
        public:
            CheckBoxDelegate(QObject *parent) : QItemDelegate(parent) {}

            void drawFocus(QPainter *p, const QStyleOptionViewItem &o, const QRect &r) const {}
    };
    relationshipTable->setItemDelegateForColumn(1, new CheckBoxDelegate(relationshipTable));
    relationshipTable->setColumnWidth(1, 30);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(relationshipTable);
    setLayout(mainLayout);
    
    
}

RelationshipPage::~RelationshipPage()
{}

void RelationshipPage::slotTableChanged()
{
    emit settingsChanged(this);
}

void RelationshipPage::writeSettings()
{
    m_settings->beginGroup("relationships");
    for (int row = 0; row < relationshipTable->rowCount(); row++) {
        QString key = relationshipTable->item(row, 0)->data(Qt::DisplayRole).toString();
        bool selected = relationshipTable->item(row, 1)->checkState() == Qt::Checked;
        m_settings->setValue(key, selected);
    }
    qDebug() << "endgroup";
    m_settings->endGroup();    
}


ConfigDialog::ConfigDialog()
    : m_settingsChanged(false)
{
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::ListMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    m_settings =  new QSettings("http://code.google.com/p/synonym/", "synonym");
    
    pagesWidget = new QStackedWidget;
    pagesWidget->addWidget(new RelationshipPage(m_settings));

    m_applyButton = new QPushButton(tr("Apply"));
    m_applyButton->setEnabled(false);
    QPushButton *closeButton = new QPushButton(tr("Close"));
 
    createIcons();
    contentsWidget->setCurrentRow(0);

    connect(m_applyButton, SIGNAL(clicked()), this, SLOT(applySettings()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

//     QHBoxLayout *horizontalLayout = new QHBoxLayout;
//     horizontalLayout->addWidget(contentsWidget);
//     horizontalLayout->addWidget(pagesWidget, 1);
// 
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(m_applyButton);
    buttonsLayout->addWidget(closeButton);
// 
//     QVBoxLayout *mainLayout = new QVBoxLayout;
//     mainLayout->addLayout(horizontalLayout);
//     mainLayout->addStretch(1);
//     mainLayout->addSpacing(12);
//     mainLayout->addLayout(buttonsLayout);
//     setLayout(mainLayout);
    
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
}

void ConfigDialog::createIcons()
{
    QListWidgetItem *relationshipButton = new QListWidgetItem(contentsWidget);
    relationshipButton->setText(tr("Relationships"));
    relationshipButton->setTextAlignment(Qt::AlignHCenter);
    relationshipButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


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
}


bool ConfigDialog::settingsChanged()
{
    return m_settingsChanged;
}


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


relationPage::relationPage(QSettings *settings, QWidget *parent)
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
    }
    
    // Verify that no relation type is missing in the settings.
    // If a type is missed add it to the table.
    foreach (QString rel, typeLabels) {
        if (!settings->childKeys().contains(rel)) {
            addrelationItem(relationTable, insertRow++, rel, true);
        }
    }
    m_settings->endGroup();
    
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

relationPage::~relationPage()
{}

void relationPage::slotTableChanged()
{
    emit settingsChanged(this);
}

void relationPage::writeSettings()
{
    m_settings->beginGroup("relations");
    for (int row = 0; row < relationTable->rowCount(); row++) {
        QString key = relationTable->item(row, 0)->data(Qt::DisplayRole).toString();
        bool selected = relationTable->item(row, 1)->checkState() == Qt::Checked;
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
    pagesWidget->addWidget(new relationPage(m_settings));

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
}

void ConfigDialog::createIcons()
{
    QListWidgetItem *relationButton = new QListWidgetItem(contentsWidget);
    relationButton->setText(tr("relations"));
    relationButton->setTextAlignment(Qt::AlignHCenter);
    relationButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);


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


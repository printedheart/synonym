/***************************************************************************
 *   Copyright (C) 2008 by Sergejs Melderis                                *
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


#include "audioscriptpage.h"

#include <QDir>
#include <QFile>

AudioWebPage::AudioWebPage(QObject *parent)
    :QWebPage(parent)
{
    setForwardUnsupportedContent(true);
    connect (this, SIGNAL(unsupportedContent(QNetworkReply *)), 
             this, SLOT(handleUnsupportedContent(QNetworkReply*)));
    
}
    
AudioWebPage::~AudioWebPage()
{}



bool AudioWebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type)
{
    bool accept = true;
    QString url = request.url().toString();
    qDebug() << "acceptNavigationRequest() " << url;
    if (url.endsWith(".js")) {
        emit downloadRequested(request);
        accept = false;
    } else if (url.endsWith(".delete")) {
        int pos = url.indexOf(".delete");
        QString scriptPath = url.remove(pos, QString(".delete").length());
        QString script = scriptPath.split("/").last();
        emit deleteRequested(script);
        accept = false;
    } else if (url.endsWith(".setcurrent")) {
        int pos = url.indexOf(".setcurrent");
        QString scriptPath = url.remove(pos, QString(".setcurrent").length());
        QString script = scriptPath.split("/").last();
        emit setCurrentRequested(script);
        accept = false;
    }
    return accept;
}


void AudioWebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    qDebug() << reply->url();
}



static QString createJavaScriptArray(const QStringList &list)
{
    QStringList scriptStr;
    scriptStr << "var arr = new Array();";
    foreach (QString item, list) {
        if (!item.isEmpty())
            scriptStr << "arr.push('" + item + "');";
    }
    return scriptStr.join("\n");
}




AudioScriptPage::AudioScriptPage(QSettings *settings, QWidget *parent)
    :SettingsPage(settings, parent), 
     m_webView(new QWebView(this))
{
    AudioWebPage *page = new AudioWebPage(m_webView);
    m_webView->setPage(page);   
    connect (page, SIGNAL(downloadRequested (const QNetworkRequest &)),
             this, SLOT(downloadScript(const QNetworkRequest &)));
    connect (page, SIGNAL(deleteRequested(const QString &)), this, SLOT(deleteScript(const QString &)));
    connect (page, SIGNAL(setCurrentRequested(const QString&)), this, SLOT(setCurrent(const QString &)));
        
    connect (m_webView, SIGNAL(loadFinished(bool)), this, SLOT(pageLoaded(bool)));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_webView);
    setLayout(mainLayout);
   // m_pageReply = m_accessManager.get(QNetworkRequest(QUrl("http://synonym.googlecode.com/svn/trunk/src/scripts/scripts.html")));
    m_pageReply = m_accessManager.get(QNetworkRequest(QUrl("file:///home/sergey/devel/graphs/trunk/synonym/src/scripts/scripts.html")));
    connect (m_pageReply, SIGNAL(finished()), this, SLOT(pageDownloaded()));
}
    
AudioScriptPage::~AudioScriptPage()
{
    
}

void AudioScriptPage::pageLoaded(bool ok)
{
    qDebug() << "pageLoaded";
    if (ok) {
        if (settings()->childGroups().contains("audio")) {
            settings()->beginGroup("audio");
            QVariant scripts = settings()->value("Scripts");
            qDebug() << "pageLoaded " << scripts;
            QStringList scriptList = scripts.toStringList();
            QString js = createJavaScriptArray(scriptList) + "\nsetInstalled(arr);";
            m_webView->page()->mainFrame()->evaluateJavaScript(js);
           
            QVariant currentScript = settings()->value("CurrentScript");
            QString js2;
            if (!currentScript.isNull()) {
                js2 = "setCurrent('" + currentScript.toString() + "');";
            } else {
                js2 = "setCurrent(null)";
            }
            m_webView->page()->mainFrame()->evaluateJavaScript(js2);
            settings()->endGroup();
        } else {
            m_webView->page()->mainFrame()->evaluateJavaScript("setInstalled(null);");
        }
            
    }    
}


void AudioScriptPage::downloadScript(const QNetworkRequest &request)
{
    m_scriptReply = m_accessManager.get(request);    
    connect (m_scriptReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void AudioScriptPage::pageDownloaded()
{
    if (m_pageReply->error() != QNetworkReply::NoError) {
        m_webView->page()->mainFrame()->setHtml(
                        "<html><head></head><body>"
                        "<h3>It seems that you are not connected to the Internet.<br/>"
                        "Audio pronunciation is available only if you are connected to Internet.</h3></body></html>");
    } else {
        m_webView->page()->mainFrame()->setContent(m_pageReply->readAll());
        pageLoaded(true);
    }
    m_pageReply->deleteLater();
}


void AudioScriptPage::downloadFinished()
{
    m_scriptReply->deleteLater();
    if (m_scriptReply->error() == QNetworkReply::NoError) {
        QString scriptUrl = m_scriptReply->url().toString();
        QString scriptName = scriptUrl.split("/").last();
        QDir home = QDir::home();
        if (!home.exists(".synonym")) {
            home.mkdir(".synonym");
            if (!home.exists(".synonym")) {
                qDebug() << "Error creating synonym directory";
                return;
            }
        }
        
        QFile file(home.canonicalPath() + "/.synonym/" + scriptName);
        file.open(QIODevice::WriteOnly);
        file.write(m_scriptReply->readAll());
        file.close();
        
        settings()->beginGroup("audio");
        QStringList scripts = settings()->value("Scripts").toStringList();
        scripts << scriptName;
        settings()->setValue("Scripts", scripts);
        settings()->endGroup();
        pageLoaded(true);
        emit settingsChanged(this);
    }
}


void AudioScriptPage::deleteScript(const QString &script)
{
    qDebug() << "deleteScript " << script;
    QFile file(QDir::homePath() + "/.synonym/" + script);
    file.remove();
    settings()->beginGroup("audio");
    QStringList scripts = settings()->value("Scripts").toStringList();
    scripts.removeOne(script);
    settings()->setValue("Scripts", scripts);
    QString currentScript = settings()->value("CurrentScript").toString();
    if (currentScript == script)
        settings()->remove(currentScript);
    settings()->endGroup();
        
    pageLoaded(true);
    emit settingsChanged(this);
}

void AudioScriptPage::setCurrent(const QString &script)
{
    qDebug() << "set current " << script.split("/").last();
    settings()->beginGroup("audio");
    settings()->setValue("CurrentScript", script.split("/").last());
    settings()->endGroup();         
    pageLoaded(true);   
    emit settingsChanged(this);
}













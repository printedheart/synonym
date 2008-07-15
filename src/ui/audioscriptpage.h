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
 
#include "configdialog.h" 
#include <QtWebKit>



class AudioWebPage : public QWebPage
{
Q_OBJECT
public:            
    AudioWebPage(QObject *parent);
    ~AudioWebPage();
    
    
signals:    
    void deleteRequested(const QString &script);
    void setCurrentRequested(const QString &script);
protected:    
    virtual bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type);
private slots:
    void handleUnsupportedContent(QNetworkReply *reply);
};


class AudioScriptPage : public SettingsPage
{
Q_OBJECT
            
public:        
    AudioScriptPage(QSettings *settings, QWidget *parent = 0);
    ~AudioScriptPage();
    
    virtual void writeSettings() {};
    
private slots:    
    void downloadScript(const QNetworkRequest &request);
    void downloadFinished();  
    void pageLoaded(bool ok);
    void pageDownloaded();
    void deleteScript(const QString &script);
    void setCurrent(const QString &script);
private:    
    QWebView *m_webView;
    QNetworkReply *m_scriptReply;
    QNetworkReply *m_pageReply;
    QNetworkAccessManager m_accessManager;
};

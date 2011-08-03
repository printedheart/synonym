#include "wordnikloader.h"
#include <QUrl>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>
#include <QtXml>



WordnikLoader::WordnikLoader(QObject *parent):
    AudioPronunciationLoader(parent), m_networkManager(0)
{
    apiKey = "0bd99de49a43197f5900c035ac706ff982aa91f71864af267";
    m_networkManager = new QNetworkAccessManager();
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));
}

WordnikLoader::~WordnikLoader()
{
    delete m_networkManager;

}

QNetworkAccessManager* WordnikLoader::networkManager()
{
    return m_networkManager;
}


void WordnikLoader::doGetAudio(const QString &word)
{
    QString urlString = "http://api.wordnik.com/v4/word.xml/" + word + "/audio";
    QUrl url(urlString);
    url.addQueryItem("limit", "1");
    url.addQueryItem("useCanonical", "true");
    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("api_key"), apiKey.toAscii());
    QNetworkReply *reply = networkManager()->get(request);
    reply->setObjectName(word);
}

bool WordnikLoader::isAvailable() const
{
    return true;
}

void WordnikLoader::slotFinished(QNetworkReply *reply)
{
     if (reply->error() == QNetworkReply::NoError) {
         QByteArray data = reply->readAll();
         QDomDocument doc;
         if (doc.setContent(data)) {
             QDomElement root = doc.documentElement();
             QDomNodeList audioFiles = root.elementsByTagName("audioFile");
             if (audioFiles.size() > 0) {
                 QDomElement audioFile = audioFiles.at(0).toElement();
                 QDomElement fileUrlEl = audioFile.elementsByTagName("fileUrl").at(0).toElement();
                 QUrl url(fileUrlEl.text());
                 emit soundFound(reply->objectName(), Phonon::MediaSource(url));
             }
         }
     }
     reply->deleteLater();
}

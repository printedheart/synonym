#include "merriamwebsterloader.h"
#include <QUrl>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStringList>
#include <QThread>

#define MAIN_PAGE_LEN 9

MerriamWebsterLoader::MerriamWebsterLoader(QObject *parent) :
    AudioPronunciationLoader(parent), m_networkManager(0)
{

}

MerriamWebsterLoader::~MerriamWebsterLoader()
{
    delete m_networkManager;
}

void MerriamWebsterLoader::doGetAudio(const QString &word)
{
    QUrl url = "http://www.merriam-webster.com/dictionary/" + word;
    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager()->get(request);
    reply->setObjectName("main_page" + word);
}

QNetworkAccessManager* MerriamWebsterLoader::networkManager()
{
    if (!m_networkManager) {
        m_networkManager = new QNetworkAccessManager();
        connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
                this, SLOT(slotFinished(QNetworkReply*)));
    }
    return m_networkManager;
}




void MerriamWebsterLoader::slotFinished(QNetworkReply *reply)
{
    if (reply->objectName().startsWith("main_page")) {
        parseMainPage(reply);
    } else {
        parseSoundPage(reply);
    }
}

void MerriamWebsterLoader::parseMainPage(QNetworkReply *reply)
{
    qDebug() << QThread::currentThread();
    QString wordTag = reply->objectName();
    QString word = wordTag.mid(MAIN_PAGE_LEN, wordTag.length() - MAIN_PAGE_LEN);
    while (true) {
        QByteArray data = reply->readLine();
        if (data.isEmpty()) {
            break;
        }
        QString line(data);
        int test = line.indexOf("return au");
        if (test != -1) {
            int startIndex = test + QString("return au").length() + 2;
            int endIndex = line.indexOf("')", test);
            qDebug() << startIndex << " " << endIndex;
            QStringList parts = line.mid(startIndex, endIndex - startIndex).split("', '");
            qDebug() << parts;
            QUrl url("http://www.merriam-webster.com/cgi-bin/audio.pl?" + parts[0] + "=" + parts[1]);
            QNetworkRequest request(url);
            qDebug() << "making request";
            QNetworkReply *newReply = networkManager()->get(request);
            qDebug() << "Got new reply";
            newReply->setObjectName(word);
            break;
        }
    }
    qDebug() << "delete reply";
    reply->deleteLater();
}


void MerriamWebsterLoader::parseSoundPage(QNetworkReply *reply)
{
    qDebug() << QThread::currentThread();
    QString word = reply->objectName();
    while (true) {
        QByteArray data = reply->readLine();
        if (data.isEmpty()) {
            break;
        }
        QString line(data);
        int test = line.indexOf("Click here to listen with your default audio player");
        if (test != -1) {
            int startIndex = line.indexOf("http");
            int endIndex = line.indexOf("\">Click here");
            if (startIndex != -1 && endIndex != -1) {
                QString soundUrl = line.mid(startIndex, endIndex - startIndex);
                QUrl url(soundUrl);
                if (url.isValid()) {
                    emit soundFound(reply->objectName(), Phonon::MediaSource(url));
                }
            }
            break;
        }
    }
    reply->deleteLater();
}


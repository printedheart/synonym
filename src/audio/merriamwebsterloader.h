#ifndef MERRIAMWEBSTERLOADER_H
#define MERRIAMWEBSTERLOADER_H

#include "audiopronunciationloader.h"
#include <QMutex>

class QNetworkAccessManager;
class QNetworkReply;


class MerriamWebsterLoader : public AudioPronunciationLoader
{
    Q_OBJECT
public:
    explicit MerriamWebsterLoader(QObject *parent = 0);
    virtual ~MerriamWebsterLoader();

    void doGetAudio(const QString &word);
    bool isAvailable() const { return true; }

signals:

private slots:
    void slotFinished(QNetworkReply *reply);
private:
    QNetworkAccessManager* networkManager();

    QNetworkAccessManager *m_networkManager;

    QMutex *m_networkMutex;
    QNetworkReply* m_mainPageReply;
    QNetworkReply* m_soundPageReply;
    void stopPrevRequest();

    void parseMainPage(QNetworkReply *reply);
    void parseSoundPage(QNetworkReply *reply);


};

#endif // MERRIAMWEBSTERLOADER_H

#ifndef AUDIOCONFIGPAGE_H
#define AUDIOCONFIGPAGE_H

#include <QWidget>

namespace Ui {
    class AudioConfigPage;
}

class AudioConfigPage : public QWidget
{
    Q_OBJECT

public:
    explicit AudioConfigPage(QWidget *parent = 0);
    ~AudioConfigPage();

private:
    Ui::AudioConfigPage *ui;
};

#endif // AUDIOCONFIGPAGE_H

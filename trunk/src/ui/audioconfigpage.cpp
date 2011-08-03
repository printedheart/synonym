#include "audioconfigpage.h"
#include "ui_audioconfigpage.h"

AudioConfigPage::AudioConfigPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AudioConfigPage)
{
    ui->setupUi(this);
}

AudioConfigPage::~AudioConfigPage()
{
    delete ui;
}

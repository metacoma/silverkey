#include "sksettings.h"
#include "ui_sksettings.h"
#include <QSettings>

SKSettings::SKSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SKSettings)
{
    ui->setupUi(this);
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    ui->skUser->setText(settings.value("user", "").toString());
    ui->skPassword->setText(settings.value("password", "").toString());
    ui->skServer->setText(settings.value("server", "hw2").toString());
    ui->skServerPort->setText(settings.value("port",2379).toString());
}

void SKSettings::accept() {
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("user", ui->skUser->text());
    settings.setValue("password", ui->skPassword->text());
    settings.setValue("server", ui->skServer->text());
    settings.setValue("port", ui->skServerPort->text().toInt());
    done(Accepted);
}

SKSettings::~SKSettings()
{
    delete ui;
}

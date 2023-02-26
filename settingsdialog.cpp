/*! ***********************************************************************************************
 *
 * \file        settingsdialog.cpp
 * \brief       SettingsDialog 类源文件。
 *
 * \version     0.1
 * \date        2023-02-26
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017~2023 karoyqiu。
 *
 **************************************************************************************************/
#include "settingsdialog.h"
#include "ui_settingsdialog.h"


SettingsDialog::SettingsDialog(QWidget *parent /*= nullptr*/)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBrowse, &QPushButton::clicked, this, &SettingsDialog::browse);

    QSettings settings;
    ui->editSkylinesLocation->setText(settings.value(QS("skylines")).toString());

    // Load proxy settings
    settings.beginGroup(QS("proxy"));
    auto enabled = settings.value(QS("enabled")).toBool();
    auto type = settings.value(QS("type")).toInt();
    auto host = settings.value(QS("host")).toString();
    auto port = settings.value(QS("port")).value<quint16>();
    ui->groupProxy->setChecked(enabled);
    ui->comboType->setCurrentIndex(type == QNetworkProxy::HttpProxy ? 0 : 1);
    ui->editHost->setText(host);
    ui->spinPort->setValue(port);
}


SettingsDialog::~SettingsDialog()
{
    delete ui;
}


void SettingsDialog::accept()
{
    QSettings settings;
    settings.setValue(QS("skylines"), ui->editSkylinesLocation->text());

    settings.beginGroup(QS("proxy"));
    settings.setValue(QS("enabled"), ui->groupProxy->isChecked());
    settings.setValue(QS("type"), ui->comboType->currentIndex() == 0 ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy);
    settings.setValue(QS("host"), ui->editHost->text());
    settings.setValue(QS("port"), ui->spinPort->value());

    QDialog::accept();
}


void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);

    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void SettingsDialog::browse()
{
    auto filename = QFileDialog::getOpenFileName(this, {}, ui->editSkylinesLocation->text(), QS("Cities.exe (Cities.exe)"));

    if (!filename.isEmpty())
    {
        QFileInfo info(filename);
        ui->editSkylinesLocation->setText(QDir::toNativeSeparators(info.absolutePath()));
    }
}

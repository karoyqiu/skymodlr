/*! ***********************************************************************************************
 *
 * \file        mainwindow.cpp
 * \brief       MainWindow 类头/源文件。
 *
 * \version     0.1
 * \date        2017/9/22
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017 Roy QIU。
 *
 **************************************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();

    // Load Skylines mods page
    ui->webView->load(QS("https://steamcommunity.com/workshop/browse/?appid=255710&requiredtags[]=Mod"));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);

    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    saveSettings();
    QWidget::closeEvent(e);
}


void MainWindow::loadSettings()
{
    // Load window settings
    QSettings settings;
    restoreGeometry(settings.value(QS("geo")).toByteArray());

    // Load proxy settings
    settings.beginGroup(QS("proxy"));
    auto host = settings.value(QS("host")).toString();
    auto port = settings.value(QS("port")).value<quint16>();

    if (!host.isEmpty() && port != 0)
    {
        auto type = static_cast<QNetworkProxy::ProxyType>(settings.value(QS("type")).toInt());
        QNetworkProxy proxy(type, host, port);
        QNetworkProxy::setApplicationProxy(proxy);
    }
    else
    {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    }
}


void MainWindow::saveSettings() const
{
    QSettings settings;
    settings.setValue(QS("geo"), saveGeometry());
}

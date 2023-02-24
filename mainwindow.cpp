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
#include "downloader.h"
#include "downloadschemehandler.h"


MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();

    // Get the default profile
    auto *profile = QWebEngineProfile::defaultProfile();
    profile->setHttpUserAgent(USER_AGENT);
    profile->settings()->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, true);

    // Register download scheme
    auto *downloader = new Downloader(this);
    auto *dl = new DownloadSchemeHandler(this);
    connect(dl, &DownloadSchemeHandler::downloadRequested, downloader, &Downloader::download);
    profile->installUrlSchemeHandler(QB("dl"), dl);

    // Load user script
    QFile file(QS(":/scripts/steamwd.user.js"));

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        auto js = QSS(file.readAll());
        js.replace(QS("{{tip}}"), tr("Download/Subscribe to the right"))
            .replace(QS("{{download}}"), tr("Download"));

        QWebEngineScript script;
        script.setSourceCode(js);
        profile->scripts()->insert(script);
    }
    else
    {
        qCritical() << "Failed to open user script:" << file.errorString();
    }

    auto *page = new QWebEnginePage(profile, ui->webView);
    ui->webView->setPage(page);

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

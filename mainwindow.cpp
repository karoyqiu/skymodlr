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
    , downloader_(nullptr)
{
    ui->setupUi(this);

    loadSettings();

    // Get the default profile
    auto *profile = QWebEngineProfile::defaultProfile();
    profile->setHttpUserAgent(USER_AGENT);
    profile->settings()->setAttribute(QWebEngineSettings::WebRTCPublicInterfacesOnly, true);

    // Register download scheme
    downloader_ = new Downloader(this);
    connect(downloader_, &Downloader::failed, this, [this](const QString &msg)
    {
        QMessageBox::critical(this, tr("Error"), msg);
    });

    auto *dl = new DownloadSchemeHandler(this);
    connect(dl, &DownloadSchemeHandler::downloadRequested, downloader_, &Downloader::download);
    profile->installUrlSchemeHandler(QB("dl"), dl);

    // Load user script
    loadScript(profile, QS(":/qtwebchannel/qwebchannel.js"), QWebEngineScript::DocumentCreation);
    loadScript(profile, QS(":/scripts/steamwd.user.js"));

    auto *channel = new QWebChannel(this);
    channel->registerObject(QS("dl"), downloader_);

    auto *page = new QWebEnginePage(profile, ui->webView);
    page->setWebChannel(channel, QWebEngineScript::ApplicationWorld);
    ui->webView->setPage(page);

    // Load Skylines mods page
    ui->webView->load(QS("https://steamcommunity.com/workshop/browse/?appid=255710&requiredtags[]=Mod"));

    // Add a tool bar
    auto *toolBar = new QToolBar(this);
    auto *a = ui->webView->pageAction(QWebEnginePage::Back);
    toolBar->addAction(ui->webView->pageAction(QWebEnginePage::Back));
    ui->verticalLayout->insertWidget(0, toolBar);

    auto *progress = new QProgressBar(this);
    connect(ui->webView, &QWebEngineView::loadProgress, progress, &QProgressBar::setValue);
    toolBar->addSeparator();
    toolBar->addWidget(progress);

    // Find out where Skylines is.
    QTimer::singleShot(0, this, &MainWindow::detectSkylines);
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


void MainWindow::loadScript(QWebEngineProfile *profile, const QString &filename, QWebEngineScript::InjectionPoint ip)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        auto js = QSS(file.readAll());
        js.replace(QS("{{tip}}"), tr("Download/Subscribe to the right"))
            .replace(QS("{{download}}"), tr("Download"));

        QWebEngineScript script;
        script.setSourceCode(js);
        script.setInjectionPoint(ip);
        profile->scripts()->insert(script);
    }
    else
    {
        qCritical() << "Failed to open user script:" << filename << file.errorString();
    }
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


void MainWindow::detectSkylines()
{
    QSettings reg(QS(R"(HKEY_CURRENT_USER\Software\Epic Games\EOS)"), QSettings::NativeFormat);
    auto metaDir = reg.value(QS("ModSdkMetadataDir")).toString();

    if (!metaDir.isEmpty())
    {
        QDir dir(metaDir);

        for (const auto &entry : dir.entryList({ QS("*.item") }))
        {
            QFile file(dir.absoluteFilePath(entry));

            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                auto doc = QJsonDocument::fromJson(file.readAll());
                auto appName = doc[QL("MainGameAppName")].toString();

                if (appName == QS("bcbc03d8812a44c18f41cf7d5f849265"))
                {
                    auto location = doc[QL("InstallLocation")].toString();
                    qInfo() << "Skylines install location:" << location;
                    downloader_->setInstallDirectory(location);
                    return;
                }
            }
        }
    }

    QMessageBox::warning(this, {}, tr("Failed to find Skylines."));
}

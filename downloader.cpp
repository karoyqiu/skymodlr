/*! ***********************************************************************************************
 *
 * \file        downloader.cpp
 * \brief       Downloader 类源文件。
 *
 * \version     0.1
 * \date        2023-02-24
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017~2023 karoyqiu。
 *
 **************************************************************************************************/
#include "downloader.h"

#include <QtGui/private/qzipreader_p.h>


Downloader::Downloader(QObject *parent /*= nullptr*/)
    : QObject(parent)
    , net_(nullptr)
{
    net_ = new QNetworkAccessManager(this);
    net_->setProxy(QNetworkProxy::NoProxy);

    setButtonText(tr("Download"));
    connect(this, &Downloader::failed, this, [this]()
    {
        setButtonText(tr("Failed"));
    });
}


void Downloader::setInstallDirectory(const QString &value)
{
    QDir dir(value);
    modDir_ = dir.absoluteFilePath(QS("Files/Mods"));
}


void Downloader::download(const QString &id)
{
    qDebug() << "Downloading" << id;
    setButtonText(tr("Downloading..."));

    QNetworkRequest req(QS("https://api.ggntw.com/steam.request"));
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
    req.setHeader(QNetworkRequest::ContentTypeHeader, QB("application/json"));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);
    req.setRawHeader(QB("Origin"), QB("https://ggntw.com"));
    req.setRawHeader(QB("Referer"), QB("https://ggntw.com/"));

    QJsonObject obj{
        { QS("url"), QString(QS("https://steamcommunity.com/sharedfiles/filedetails/?id=") % id) }
    };
    QJsonDocument doc(obj);
    auto body = doc.toJson(QJsonDocument::Compact);

    auto *reply = net_->post(req, body);
    connect(reply, &QNetworkReply::finished, this, &Downloader::handleReply);
}


void Downloader::resetButtonText(const QString &id)
{
    QDir dir(modDir_);

    if (dir.cd(id))
    {
        setButtonText(tr("Installed!"));
    }
    else
    {
        setButtonText(tr("Download"));
    }
}


void Downloader::setButtonText(const QString &value)
{
    if (buttonText_ != value)
    {
        buttonText_ = value;
        emit buttonTextChanged(value);
    }
}


void Downloader::handleReply()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (status == 200)
    {
        auto body = reply->readAll();
        auto doc = QJsonDocument::fromJson(body);
        auto url = doc[QL("url")].toString();

        if (url.isEmpty())
        {
            qCritical() << "Failed to get the download url." << url;
            emit failed(tr("Failed to get download url (%1).").arg(url));
        }
        else
        {
            downloadZip(url);
        }
    }
    else
    {
        qCritical() << "Failed to get download url." << status << reply->errorString();
        emit failed(tr("Failed to get download url (%1).").arg(status));
    }

    reply->deleteLater();
}


void Downloader::downloadZip(const QUrl &url)
{
    qDebug() << "Zip" << url;
    QNetworkRequest req(url);
    req.setAttribute(QNetworkRequest::Http2AllowedAttribute, true);
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);

    auto *reply = net_->get(req);
    connect(reply, &QNetworkReply::finished, this, &Downloader::handleZip);
}


void Downloader::handleZip()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (status == 200)
    {
        QBuffer buffer;
        buffer.setData(reply->readAll());
        buffer.open(QIODevice::ReadOnly);

        QZipReader zip(&buffer);
        auto ok = zip.extractAll(modDir_);

        if (ok)
        {
            emit downloaded();
            setButtonText(tr("Installed!"));
        }
        else
        {
            qCritical() << "Failed to unzip.";
            emit failed(tr("Failed to unzip."));
        }
    }
    else
    {
        qCritical() << "Failed to download zip." << status;
        emit failed(tr("Failed to download zip (%1).").arg(status));
    }

    reply->deleteLater();
}

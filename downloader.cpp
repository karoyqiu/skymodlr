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
}


void Downloader::setInstallDirectory(const QString &value)
{
    QDir dir(value);
    modDir_ = dir.absoluteFilePath(QS("Files/Mods"));
}


void Downloader::download(const QString &id)
{
    qDebug() << "Downloading" << id;
    QUrlQuery query;
    query.addQueryItem(QS("item"), id);
    query.addQueryItem(QS("app"), QS("255710"));
    auto body = query.toString(QUrl::FullyEncoded).toUtf8();

    QNetworkRequest req(QS("http://steamworkshop.download/online/steamonline.php"));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QB("application/x-www-form-urlencoded"));
    req.setHeader(QNetworkRequest::UserAgentHeader, USER_AGENT);

    auto *reply = net_->post(req, body);
    connect(reply, &QNetworkReply::finished, this, &Downloader::handleReply);
}


void Downloader::handleReply()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    auto status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (status == 200)
    {
        static const QString prefix = QS("href='");
        auto s = QSS(reply->readAll());
        auto start = s.indexOf(prefix);

        if (start == -1)
        {
            qCritical() << "Failed to get the download url." << s;
        }
        else
        {
            start += prefix.length();
            auto end = s.indexOf(QL('\''), start);
            auto url = s.mid(start, end - start);
            downloadZip(url);
        }
    }
    else
    {
        qCritical() << "Failed to get download url." << status;
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

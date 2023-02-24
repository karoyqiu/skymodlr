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


Downloader::Downloader(QObject *parent /*= nullptr*/)
    : QObject(parent)
    , net_(nullptr)
{
    net_ = new QNetworkAccessManager(this);

    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(QS("127.0.0.1"));
    proxy.setPort(8888);
    net_->setProxy(proxy);
    //net_->setProxy(QNetworkProxy::NoProxy);
}


void Downloader::download(const QString &id)
{
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
            qDebug() << "URL" << url;
        }
    }
    else
    {
        qCritical() << "Failed to get download url." << status;
    }

    reply->deleteLater();
}

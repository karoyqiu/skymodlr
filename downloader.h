/*! ***********************************************************************************************
 *
 * \file        downloader.h
 * \brief       Downloader 类头文件。
 *
 * \version     0.1
 * \date        2023-02-24
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017~2023 karoyqiu。
 *
 **************************************************************************************************/
#pragma once
#include <QObject>

#define USER_AGENT  QS("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.50")

class QNetworkAccessManager;


class Downloader : public QObject
{
    Q_OBJECT

public:
    explicit Downloader(QObject *parent = nullptr);

    void setInstallDirectory(const QString &value);
    void download(const QString &id);

signals:
    void downloaded();
    void failed(const QString &error);

private:
    void handleReply();

    void downloadZip(const QUrl &url);
    void handleZip();

private:
    QNetworkAccessManager *net_;
    QString modDir_;
};

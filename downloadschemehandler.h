/*! ***********************************************************************************************
 *
 * \file        downloadschemehandler.h
 * \brief       DownloadSchemeHandler 类头文件。
 *
 * \version     0.1
 * \date        2023-02-24
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017~2023 karoyqiu。
 *
 **************************************************************************************************/
#pragma once
#include <QWebEngineUrlSchemeHandler>


class DownloadSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT

public:
    using QWebEngineUrlSchemeHandler::QWebEngineUrlSchemeHandler;

    virtual void requestStarted(QWebEngineUrlRequestJob *job) override;
};

/*! ***********************************************************************************************
 *
 * \file        downloadschemehandler.cpp
 * \brief       DownloadSchemeHandler 类源文件。
 *
 * \version     0.1
 * \date        2023-02-24
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017~2023 karoyqiu。
 *
 **************************************************************************************************/
#include "downloadschemehandler.h"


void DownloadSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    qDebug() << "Download" << job->requestUrl() << job->requestUrl().path();
    job->fail(QWebEngineUrlRequestJob::RequestAborted);
}

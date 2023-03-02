﻿/*! ***********************************************************************************************
 *
 * \file        mainwindow.h
 * \brief       MainWindow 类头/源文件。
 *
 * \version     0.1
 * \date        2017/9/22
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017 Roy QIU。
 *
 **************************************************************************************************/
#pragma once
#include <QWidget>

class Downloader;

namespace Ui {
class MainWindow;
}


class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow();

protected:
    virtual void changeEvent(QEvent *e) override;
    virtual void closeEvent(QCloseEvent *e) override;

private:
    static void loadScript(
        QWebEngineProfile *profile,
        const QString &filename,
        QWebEngineScript::InjectionPoint ip = QWebEngineScript::Deferred
    );

private:
    void loadSettings();
    void loadProxySettings();
    void saveSettings() const;

    void detectSkylines();
    void showSettings();
    void goTo();

private:
    Ui::MainWindow *ui;
    Downloader *downloader_;
};

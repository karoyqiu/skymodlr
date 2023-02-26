/*! ***********************************************************************************************
 *
 * \file        settingsdialog.h
 * \brief       SettingsDialog 类头文件。
 *
 * \version     0.1
 * \date        2023-02-26
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2017~2023 karoyqiu。
 *
 **************************************************************************************************/
#pragma once
#include <QDialog>

namespace Ui {
class SettingsDialog;
}


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    virtual ~SettingsDialog();

    virtual void accept() override;

protected:
    virtual void changeEvent(QEvent *e) override;

private:
    void browse();

private:
    Ui::SettingsDialog *ui;
};

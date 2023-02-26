/*! ***********************************************************************************************
 *
 * \file        stable.h
 * \brief       预编译头文件。
 *
 * \version     0.1
 * \date        2016/11/22
 *
 * \author      Roy QIU <karoyqiu@gmail.com>
 * \copyright   © 2016 Roy QIU。
 *
 **************************************************************************************************/
#pragma once
#include <QByteArray>
#include <QString>

#define QB(x)   QByteArrayLiteral(x)
#define QS(x)   QStringLiteral(x)
#define QSS(x)  QString::fromUtf8(x)


inline QLatin1String QL(const char *s)
{
    return QLatin1String(s);
}

inline QLatin1Char QL(char ch)
{
    return QLatin1Char(ch);
}

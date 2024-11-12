/*
 * @Author: xixi_
 * @Date: 2024-11-10 16:08:28
 * @LastEditors: xixi_
 * @LastEditTime: 2024-11-11 23:53:45
 * @FilePath: /FHCalc/src/main.cpp
 * Copyright (c) 2020-2024 by xixi_ , All Rights Reserved.
 */
#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages)
    {
        const QString baseName = "FHCalc_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}

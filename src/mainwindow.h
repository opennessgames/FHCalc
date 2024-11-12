/*
 * @Author: xixi_
 * @Date: 2024-11-10 17:47:24
 * @LastEditors: xixi_
 * @LastEditTime: 2024-11-12 19:47:04
 * @FilePath: /FHCalc/src/mainwindow.h
 * Copyright (c) 2020-2024 by xixi_ , All Rights Reserved.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <xixi/DynamicStack.h>
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    /* 槽函数 */
    /* 数字 */
    void FH_ZeroBtnClicked();
    void FH_OneBtnClicked();
    void FH_AddBtnClicked();
    /* 基本操作 */
    void FH_Undo();  /* 撤销 */
    void FH_Clear(); /* 清空 */
    /* 解析 */
    void FH_Eval();                                         /* 转后缀 */
    void FH_CalculatePostfix(ThisDynamicStack *Expression); /* 计算后缀 */

private:
    /* 辅助 */
    int FH_GetPriority(char Ch);                        /* 获取优先级 */
    int FH_IsNumber(QChar Ch);                          /* 是否为数字 */
    int FH_Is_Op(char *op);                             /* 是否为操作符号 */
    double FH_Calc(double num1, double num2, char *op); /* 计算 */

private:
    Ui::MainWindow *ui;
    QString ExpressionStr;
};
#endif // MAINWINDOW_H

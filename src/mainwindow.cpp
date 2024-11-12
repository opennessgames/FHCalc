/*
 * @Author: xixi_
 * @Date: 2024-11-10 17:52:17
 * @LastEditors: xixi_
 * @LastEditTime: 2024-11-12 20:04:38
 * @FilePath: /FHCalc/src/mainwindow.cpp
 * Copyright (c) 2020-2024 by xixi_ , All Rights Reserved.
 */
#include "mainwindow.h"
#include "./ui/ui_mainwindow.h"
/* 平头标准库 */
#include <xixi/DynamicStack.h> /* 动态栈 */
/* 标准库 */
#include <string.h>
#include <stdio.h>
#include <QDebug>
#include <math.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setMaximumSize(260, 300);   /* 最大大小 */
    this->setMinimumSize(260, 300);   /* 最小大小 */
    this->setWindowTitle("FH计算器"); /* 标题 */
    /* 链接数字槽函数 */
    connect(ui->Number0, &QPushButton::clicked, this, &MainWindow::FH_ZeroBtnClicked); /* 数字0被按下 */
    connect(ui->Number1, &QPushButton::clicked, this, &MainWindow::FH_OneBtnClicked);  /* 数字1被按下 */
    connect(ui->BtnADD, &QPushButton::clicked, this, &MainWindow::FH_AddBtnClicked);   /* 加号 */
    /* 链接基本操作槽函数 */
    connect(ui->BtnNo, &QPushButton::clicked, this, &MainWindow::FH_Undo);     /* 删除 */
    connect(ui->BtnClear, &QPushButton::clicked, this, &MainWindow::FH_Clear); /* 清除 */
    connect(ui->BtnEqual, &QPushButton::clicked, this, &MainWindow::FH_Eval);  /* 计算 */
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::FH_ZeroBtnClicked()
{
    ExpressionStr += '0';
    ui->DisplayText->setText(ExpressionStr);
}

void MainWindow::FH_OneBtnClicked()
{
    ExpressionStr += '1';
    ui->DisplayText->setText(ExpressionStr);
}

void MainWindow::FH_AddBtnClicked()
{
    ExpressionStr += '+';
    ui->DisplayText->setText(ExpressionStr);
}

/* 删除 */
void MainWindow::FH_Undo()
{
    ExpressionStr.chop(1);
    ui->DisplayText->setText(ExpressionStr);
}

/* 清除 */
void MainWindow::FH_Clear()
{
    ExpressionStr.clear();
    ui->DisplayText->setText(ExpressionStr);
}

/* 获取优先级 */
int MainWindow::FH_GetPriority(char Ch)
{
    switch (Ch)
    {
    case '+':
    case '-':
        return 0;
    case '*':
    case '/':
        return 1;
    case '^':
        return 2;
    case '(':
        return 3;
    default:
        return -1;
    }
}

/* 是否为数字 */
int MainWindow::FH_IsNumber(QChar Ch)
{
    return Ch >= '0' && Ch <= '9';
}

/* Shunting Yard 算法 */
/* 等于号 */
void MainWindow::FH_Eval()
{
    ThisDynamicStack Expression; /* 输出栈，用于存放后缀表达式的元素 */
    ThisDynamicStack OpStack;    /* 操作符栈 */
    XIXI_DynamicStackInit(&Expression);
    XIXI_DynamicStackInit(&OpStack);

    int ExpressionLen = ExpressionStr.length();
    const char *ExpressionChar = ExpressionStr.toLocal8Bit().data(); /* 转成Char*类型 */

    char CurrNumChar[114] = ""; /* 用于存放当前数字的字符 */

    for (int i = 0; i < ExpressionLen; i++)
    {
        char ch = ExpressionChar[i];
        if (FH_IsNumber(ch)) /* 如果是数字（包括多位数字） */
        {
            strncat(CurrNumChar, &ExpressionChar[i], 1); /* 拼接字符形成数字 */
        }
        else
        {
            if (strlen(CurrNumChar) > 0) /* 如果当前有数字，将其推入输出栈 */
            {
                XIXI_DynamicStackPush(&Expression, CurrNumChar);
                memset(CurrNumChar, 0, sizeof(CurrNumChar)); /* 清空当前数字 */
            }
            if (FH_GetPriority(ch) != -1) /* 如果是操作符 */
            {
                while (!XIXI_DynamicStackIsEmpty(&OpStack) && /* 处理操作符栈中的优先级 */
                       FH_GetPriority(*XIXI_DynamicStackPeek(&OpStack)) >= FH_GetPriority(ch))
                {
                    /* 弹出操作符栈的栈顶操作符并推入输出栈 */
                    char *topOp = XIXI_DynamicStackPop(&OpStack);
                    XIXI_DynamicStackPush(&Expression, topOp);
                    free(topOp); /* 释放栈顶操作符的内存 */
                }
                char CurrStr[2] = {ch, '\0'};
                XIXI_DynamicStackPush(&OpStack, CurrStr); /* 当前操作符入栈 */
            }
            else if (ch == '(')
            {
                char CurrStr[2] = {ch, '\0'};
                XIXI_DynamicStackPush(&OpStack, CurrStr); /* 左括号入栈 */
            }
            else if (ch == ')')
            {
                /* 右括号，弹出操作符栈直到遇到左括号 */
                while (!XIXI_DynamicStackIsEmpty(&OpStack) && *XIXI_DynamicStackPeek(&OpStack) != '(')
                {
                    char *topOp = XIXI_DynamicStackPop(&OpStack);
                    XIXI_DynamicStackPush(&Expression, topOp);
                    free(topOp);
                }
                if (!XIXI_DynamicStackIsEmpty(&OpStack))
                {
                    free(XIXI_DynamicStackPop(&OpStack)); /* 弹出左括号 */
                }
            }
        }
    }

    if (strlen(CurrNumChar) > 0) /* 处理最后剩下的数字 */
    {
        XIXI_DynamicStackPush(&Expression, CurrNumChar);
    }

    /* 将操作符栈中的剩余操作符全部弹出并推入输出栈 */
    while (!XIXI_DynamicStackIsEmpty(&OpStack))
    {
        char *topOp = XIXI_DynamicStackPop(&OpStack);
        XIXI_DynamicStackPush(&Expression, topOp);
        free(topOp);
    }
    FH_CalculatePostfix(&Expression);   /* 计算结果 直接调用计算 */
    XIXI_DynamicStackFree(&Expression); /* 清理栈 */
    XIXI_DynamicStackFree(&OpStack);
}

void MainWindow::FH_CalculatePostfix(ThisDynamicStack *Expression)
{
    ThisDynamicStack Stack;
    XIXI_DynamicStackInit(&Stack);

    for (int index = 0; index <= Expression->ThisTop; index++)
    {
        if (!FH_Is_Op(Expression->ThisCharArr[index]))
        {
            /* 弹出两个操作数 */
            char *a = (char *)XIXI_DynamicStackPop(&Stack);
            char *b = (char *)XIXI_DynamicStackPop(&Stack);
            /* 铀锈的人才 */
            double result = FH_Calc(atof(a), atof(b), Expression->ThisCharArr[index]);
            free(a); /* 释放之前弹出的内存 */
            free(b);
            char *result_str = (char *)malloc(50 * sizeof(char)); /* 将结果压回栈 */
            snprintf(result_str, 50, "%.4f", result);
            XIXI_DynamicStackPush(&Stack, result_str);
            free(result_str);
        }
        else
        {
            XIXI_DynamicStackPush(&Stack, Expression->ThisCharArr[index]); /* 如果是操作数，将其直接压入栈 */
        }
    }

    char *StackTop = (char *)XIXI_DynamicStackPop(&Stack);     /* 弹出栈顶 */
    ExpressionStr.clear();                                     /* 将当前输入清空 */
    ui->DisplayText->setText(QString::number(atof(StackTop))); /* 显示在界面上 */
    XIXI_DynamicStackFree(&Stack);
    free(StackTop);
    /* PS:如果直接返回`atof(StackTop)`会造成内存泄漏 */
}

int MainWindow::FH_Is_Op(char *op)
{
    return strcmp(op, "+") || strcmp(op, "-") || strcmp(op, "*") || strcmp(op, "/");
}

double MainWindow::FH_Calc(double num1, double num2, char *op)
{
    switch (*op)
    {
    case '+':
        return num1 + num2; /* 返回加法的值 */
    case '-':
        return num1 - num2;
    case '*':
        return num1 * num2;
    case '/':
        return num1 / num2;
    case '^':
        return pow(num1, num2);
    default:
        return NAN;
    }
}

/* 如果你想体验超级计算机，就可以把这段代码取消注释，别忘记把上面的注释了 */
// double MainWindow::FH_CalculatePostfix(ThisDynamicStack *Expression)
// {
//     ThisDynamicStack *Stack;
//     for (int index=0;index<=Expression->ThisTop;index++){
//         if (strcmp("+",Expression->ThisCharArr[index]) == 0){
//         } else {
//             XIXI_DynamicStackPush(Stack,Expression->ThisCharArr[index]);
//         }
//     }
//     XIXI_DynamicStackPrint(Stack);
//     XIXI_DynamicStackFree(Stack);
//     return 0;
// }

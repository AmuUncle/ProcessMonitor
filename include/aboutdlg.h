/**
 * @file      aboutdlg.h
 * @brief     关于窗口
 * @author    胡德杰
 * @date      2021-02-03
 * @version   1.0
 * @copyright Copyright(C) 2020 NVR All rights reserved.
 */

#ifndef ABOUTDLG_H
#define ABOUTDLG_H

#include <QWidget>
#include "basedlg.h"

namespace Ui {
class CAboutDlg;
}

class CAboutDlg : public CBaseDlg
{
    Q_OBJECT

public:
    explicit CAboutDlg(QWidget *parent = 0);
    ~CAboutDlg();

private:
    void InitCtrl();
    void Relayout();

private:
    Ui::CAboutDlg *ui;
};

#endif // ABOUTDLG_H

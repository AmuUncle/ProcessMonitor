/**
 * @file      gfxmessagebox.h
 * @brief     自定义MessageBox
 * @author    胡德杰
 * @date      2020-10-19
 * @version   1.0
 * @copyright Copyright(C) 2020 NVR All rights reserved.
 */

#ifndef GFXMESSAGEBOX_H
#define GFXMESSAGEBOX_H

#include <QMutex>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMutex>
#include <QDialog>
#include <QMessageBox>


//弹出信息框类
class CustomMsgBox : public QDialog
{
    Q_OBJECT

public:
    explicit CustomMsgBox(QWidget *parent = 0);
    ~CustomMsgBox();

public:
    void SetMessage(const QString strMsg, const QString strTitle, QMessageBox::StandardButtons buttons = QMessageBox::Ok);

private:
    void InitCtrl();                //初始化控件
    void InitSolts();
    void Relayout();

private slots:
    void OnBtnOkClicked();
    void OnBtnCloseClicked();
    void OnBtnYesClicked();
    void OnBtnNoClicked();

private:
    QVBoxLayout *m_layoutMain;
    QWidget *m_widgetTitle;
    QHBoxLayout *m_hLayoutTitle;
    QLabel *m_labelTitle;
    QPushButton *m_btnClose;
    QWidget *m_widgetMain;
    QVBoxLayout *m_vLayoutMain;
    QFrame *m_frameMain;
    QVBoxLayout *m_vLayoutFrameMain;
    QHBoxLayout *m_hLayoutCenter;
    QLabel *m_labelIcon;
    QSpacerItem *m_horizontalSpacer;
    QLabel *m_labelInfo;
    QHBoxLayout *m_hLayoutToolBar;
    QPushButton *m_btnOk;
    QPushButton *m_btnNo;
    QPushButton *m_btnYes;
    QPushButton *m_btnCancel;
};

int GfxMessageBox(const QString strText, const QString strTitle, QMessageBox::StandardButtons buttons = QMessageBox::Ok);
int GfxMessageBox(const QString strText, QMessageBox::StandardButtons buttons = QMessageBox::Ok, bool bFirstWnd = false);

#endif // GFXMESSAGEBOX_H

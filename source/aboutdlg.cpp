#include "aboutdlg.h"
#include "ui_aboutdlg.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

CAboutDlg::CAboutDlg(QWidget *parent) :
    CBaseDlg(parent),
    ui(new Ui::CAboutDlg)
{
    ui->setupUi(this);

    connect(ui->m_btnOk, SIGNAL(clicked()), this, SLOT(OnBtnCloseClicked()));

    InitCtrl();
    Relayout();
}

CAboutDlg::~CAboutDlg()
{
    delete ui;
}

void CAboutDlg::InitCtrl()
{
    SetTitle(tr("关于"));
    ui->m_labelIcon->setStyleSheet("border-image: url(:/image/Monitor.ico);");
    ui->m_labelIcon->setFixedSize(64, 64);
    ui->m_labelNote->setText(QString("ProcessMonitor v1.0\n%3")
                             .arg(tr("版权所有©1995-2021 阿木大叔")));
}

void CAboutDlg::Relayout()
{
    QVBoxLayout *hLayoutLogo = new QVBoxLayout();
    hLayoutLogo->addStretch();
    hLayoutLogo->addWidget(ui->m_labelIcon);
    hLayoutLogo->addStretch();
    hLayoutLogo->setContentsMargins(0, 0, 0, 0);
    hLayoutLogo->setSpacing(6);

    QHBoxLayout *layoutTop = new QHBoxLayout();
    layoutTop->addLayout(hLayoutLogo);
    layoutTop->addWidget(ui->m_labelNote);
    layoutTop->setContentsMargins(0, 0, 0, 0);
    layoutTop->setSpacing(6);

    QHBoxLayout *layoutBottom = new QHBoxLayout();
    layoutBottom->addStretch();
    layoutBottom->addWidget(ui->m_btnOk);
    layoutBottom->setContentsMargins(0, 0, 0, 0);
    layoutBottom->setSpacing(6);

    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->addLayout(layoutTop);
    layoutMain->addLayout(layoutBottom);
    layoutMain->setContentsMargins(10, 10, 10, 10);
    layoutMain->setSpacing(6);

    centralWidget()->setLayout(layoutMain);

    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

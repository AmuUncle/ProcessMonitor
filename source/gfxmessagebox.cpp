#include "gfxmessagebox.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QStyle>
#include <QGraphicsDropShadowEffect>
#include "iconhelper.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

CustomMsgBox::CustomMsgBox(QWidget *parent) : QDialog(parent)
{
    InitCtrl();
    InitSolts();
}

CustomMsgBox::~CustomMsgBox()
{
    if (NULL != m_widgetMain)
        delete m_widgetMain;
}

void CustomMsgBox::SetMessage( const QString strMsg, const QString strTitle, QMessageBox::StandardButtons buttons /*= QMessageBox::Ok*/)
{
    m_btnYes->setVisible(buttons & QMessageBox::Yes);
    m_btnNo->setVisible(buttons & QMessageBox::No);
    m_btnOk->setVisible(buttons & QMessageBox::Ok);
    m_btnCancel->setVisible(buttons & QMessageBox::Cancel);

    m_labelTitle->setText(strTitle);
    m_labelInfo->setText(strMsg);
    setWindowTitle(m_labelTitle->text());
}

void CustomMsgBox::InitCtrl()
{
    Relayout();

    m_widgetMain->setProperty("form", "messagebox");
    m_widgetTitle->setProperty("form", "title");

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint /*| Qt::WindowStaysOnTopHint*/);
    setFixedSize(280, 150);
    setWindowTitle(m_labelTitle->text());

    int nWidth = 80;
    int nIconWidth = 18;
    int nIconHeight = 18;
    m_labelIcon->setFixedSize(30, 30);

    QList<QPushButton *> listBtns = m_frameMain->findChildren<QPushButton *>();
    foreach (QPushButton *btn, listBtns)
    {
        btn->setMinimumWidth(nWidth);
        btn->setIconSize(QSize(nIconWidth, nIconHeight));
    }

    installEventFilter(this);

    m_widgetMain->setStyleSheet("QDialog{background:transparent;border:2px solid black;}");

    m_btnOk->setText(tr("确定"));
    m_btnCancel->setText(tr("取消"));
    m_btnYes->setText(tr("是"));
    m_btnNo->setText(tr("否"));
}

void CustomMsgBox::InitSolts()
{
    connect(m_btnOk, SIGNAL(clicked()), this, SLOT(OnBtnOkClicked()));
    connect(m_btnClose, SIGNAL(clicked()), this, SLOT(OnBtnCloseClicked()));
    connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCloseClicked()));
    connect(m_btnNo, SIGNAL(clicked()), this, SLOT(OnBtnNoClicked()));
    connect(m_btnYes, SIGNAL(clicked()), this, SLOT(OnBtnYesClicked()));
}

void CustomMsgBox::Relayout()
{
    m_layoutMain = new QVBoxLayout(this);
    m_layoutMain->setSpacing(0);
    m_layoutMain->setMargin(0);

    m_widgetTitle = new QWidget(this);
    QSizePolicy sizetPolicyTitle(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizetPolicyTitle.setHorizontalStretch(0);
    sizetPolicyTitle.setVerticalStretch(0);
    sizetPolicyTitle.setHeightForWidth(m_widgetTitle->sizePolicy().hasHeightForWidth());
    m_widgetTitle->setSizePolicy(sizetPolicyTitle);
    m_widgetTitle->setMinimumSize(QSize(0, 30));

    m_hLayoutTitle = new QHBoxLayout(m_widgetTitle);
    m_hLayoutTitle->setSpacing(0);
    m_hLayoutTitle->setContentsMargins(4, 0, 0, 0);

    m_labelTitle = new QLabel(m_widgetTitle);
    m_labelTitle->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    m_hLayoutTitle->addWidget(m_labelTitle);

    m_btnClose = new QPushButton();
    QSizePolicy sizePolicyClose(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicyClose.setHorizontalStretch(0);
    sizePolicyClose.setVerticalStretch(0);
    sizePolicyClose.setHeightForWidth(m_btnClose->sizePolicy().hasHeightForWidth());
    m_btnClose->setSizePolicy(sizePolicyClose);
    m_btnClose->setMinimumSize(QSize(30, 0));
    m_btnClose->setMaximumSize(QSize(30, 16777215));
    m_btnClose->setCursor(QCursor(Qt::ArrowCursor));
    m_btnClose->setFocusPolicy(Qt::NoFocus);
    m_btnClose->setFlat(true);
    IconHelper::Instance()->setIcon(m_btnClose, QChar(0xf00d));

    m_hLayoutTitle->addWidget(m_btnClose);
    m_layoutMain->addWidget(m_widgetTitle);

    m_widgetMain = new QWidget(this);
    m_vLayoutMain = new QVBoxLayout(m_widgetMain);
    m_vLayoutMain->setSpacing(5);
    m_vLayoutMain->setContentsMargins(5, 5, 5, 5);

    m_frameMain = new QFrame(m_widgetMain);
    m_frameMain->setFrameShape(QFrame::NoFrame);
    m_frameMain->setFrameShadow(QFrame::Sunken);

    m_vLayoutFrameMain = new QVBoxLayout(m_frameMain);
    m_vLayoutFrameMain->setContentsMargins(-1, 9, -1, -1);

    m_hLayoutCenter = new QHBoxLayout();
    m_labelIcon = new QLabel(m_frameMain);
    m_labelIcon->setMinimumSize(QSize(30, 30));
    m_labelIcon->setMaximumSize(QSize(30, 30));

    QStyle* style = QApplication::style();
    QIcon icon = style->standardIcon(QStyle::SP_MessageBoxInformation);
    m_labelIcon->setPixmap(icon.pixmap(m_labelIcon->size()));

    m_hLayoutCenter->addWidget(m_labelIcon);

    m_horizontalSpacer = new QSpacerItem(5, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_hLayoutCenter->addItem(m_horizontalSpacer);

    m_labelInfo = new QLabel(m_frameMain);
    QSizePolicy sizePolicyInfo(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicyInfo.setHorizontalStretch(0);
    sizePolicyInfo.setVerticalStretch(0);
    sizePolicyInfo.setHeightForWidth(m_labelInfo->sizePolicy().hasHeightForWidth());
    m_labelInfo->setSizePolicy(sizePolicyInfo);
    m_labelInfo->setMinimumSize(QSize(0, 33));
    m_labelInfo->setScaledContents(false);
    m_labelInfo->setWordWrap(true);

    m_hLayoutCenter->addWidget(m_labelInfo);
    m_vLayoutFrameMain->addLayout(m_hLayoutCenter);

    m_hLayoutToolBar = new QHBoxLayout();
    QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_hLayoutToolBar->addItem(horizontalSpacer);

    m_btnOk = new QPushButton(m_frameMain);
    m_btnOk->setMinimumSize(QSize(80, 0));
    m_btnOk->setFocusPolicy(Qt::StrongFocus);
    m_hLayoutToolBar->addWidget(m_btnOk);

    m_btnYes = new QPushButton(m_frameMain);
    m_btnYes->setMinimumSize(QSize(80, 0));
    m_btnYes->setFocusPolicy(Qt::StrongFocus);
    m_hLayoutToolBar->addWidget(m_btnYes);

    m_btnNo = new QPushButton(m_frameMain);
    m_btnNo->setMinimumSize(QSize(80, 0));
    m_btnNo->setFocusPolicy(Qt::StrongFocus);
    m_hLayoutToolBar->addWidget(m_btnNo);

    m_btnCancel = new QPushButton(m_frameMain);
    m_btnCancel->setMinimumSize(QSize(80, 0));
    m_btnCancel->setFocusPolicy(Qt::StrongFocus);
    m_hLayoutToolBar->addWidget(m_btnCancel);

    m_vLayoutFrameMain->addLayout(m_hLayoutToolBar);
    m_vLayoutMain->addWidget(m_frameMain);
    m_layoutMain->addWidget(m_widgetMain);

    m_widgetTitle->raise();
    m_widgetMain->raise();
    m_frameMain->raise();
}

void CustomMsgBox::OnBtnOkClicked()
{
    done(QMessageBox::Ok);
    close();
}

void CustomMsgBox::OnBtnCloseClicked()
{
    done(QMessageBox::Cancel);
    close();
}

void CustomMsgBox::OnBtnYesClicked()
{
    done(QMessageBox::Yes);
    close();
}

void CustomMsgBox::OnBtnNoClicked()
{
    done(QMessageBox::No);
    close();
}

int GfxMessageBox( QString strText, QString strTitle, QMessageBox::StandardButtons buttons /*= QMessageBox::Ok*/)
{
    CustomMsgBox msg;     // 设置一个父窗口，避免出现两个窗口
    msg.SetMessage(strText, strTitle, buttons);
    return msg.exec();
}

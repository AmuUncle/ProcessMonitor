﻿#include "basedlg.h"
#include <QVariant>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include "iconhelper.h"

CBaseDlg::CBaseDlg(QWidget *parent) : QDialog(parent)
{
    m_layoutMain = NULL;
    m_widgetMain = NULL;
    m_vLayoutWidgetMain = NULL;
    m_widgetTitle = NULL;
    m_hLayoutTitle = NULL;
    m_labelTitle = NULL;
    m_widgetMenu = NULL;
    horizontalLayout = NULL;
    m_btnMin = NULL;
    m_btnMax = NULL;
    m_btnClose = NULL;
    m_widgetMainPane = NULL;

    m_bWndMaxSize = false;
    m_bEnableMove = false;

    InitCtrl();
}

CBaseDlg::~CBaseDlg()
{
#define SAFE_DELETE(pObj) \
    if (pObj != NULL) { delete pObj; pObj = NULL; }

    // TODO:资源释放优化，SAFE_DELETE(m_vLayoutWidgetMain);崩溃
//    SAFE_DELETE(m_layoutMain);
//    SAFE_DELETE(m_widgetMain);
//    SAFE_DELETE(m_vLayoutWidgetMain);
//    SAFE_DELETE(m_widgetTitle);
//    SAFE_DELETE(m_hLayoutTitle);
//    SAFE_DELETE(m_labelTitle);
//    SAFE_DELETE(m_widgetMenu);
//    SAFE_DELETE(horizontalLayout);
//    SAFE_DELETE(m_btnMin);
//    SAFE_DELETE(m_btnMax);
//    SAFE_DELETE(m_btnClose);
//    SAFE_DELETE(m_widgetMainPane);
}

void CBaseDlg::InitCtrl()
{
    m_layoutMain = new QVBoxLayout(this);
    m_layoutMain->setSpacing(0);
    m_layoutMain->setMargin(0);

    m_widgetMain = new QWidget(this);
    m_widgetMain->setStyleSheet(QString::fromUtf8(""));

    m_vLayoutWidgetMain = new QVBoxLayout(m_widgetMain);
    m_vLayoutWidgetMain->setSpacing(0);
    m_vLayoutWidgetMain->setContentsMargins(0, 0, 0, 0);

    // 标题栏
    m_widgetTitle = new QWidget(m_widgetMain);

    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(m_widgetTitle->sizePolicy().hasHeightForWidth());
    m_widgetTitle->setSizePolicy(sizePolicy);
    m_widgetTitle->setMinimumSize(QSize(0, 45));

    m_hLayoutTitle = new QHBoxLayout(m_widgetTitle);
    m_hLayoutTitle->setSpacing(0);
    m_hLayoutTitle->setContentsMargins(11, 0, 0, 0);

    m_labelTitle = new QLabel(m_widgetTitle);
    QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(m_labelTitle->sizePolicy().hasHeightForWidth());
    m_labelTitle->setSizePolicy(sizePolicy2);
    m_hLayoutTitle->addWidget(m_labelTitle);

    m_widgetMenu = new QWidget(m_widgetTitle);
    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy1.setHeightForWidth(m_widgetMenu->sizePolicy().hasHeightForWidth());
    m_widgetMenu->setSizePolicy(sizePolicy1);

    horizontalLayout = new QHBoxLayout(m_widgetMenu);
    horizontalLayout->setSpacing(0);
    horizontalLayout->setContentsMargins(11, 11, 11, 11);
    horizontalLayout->setContentsMargins(0, 0, 0, 0);

    m_btnMin = new QPushButton(m_widgetMenu);
    IconHelper::Instance()->setIcon(m_btnMin, QChar(0xf068));
    QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(m_btnMin->sizePolicy().hasHeightForWidth());
    m_btnMin->setSizePolicy(sizePolicy4);
    m_btnMin->setMinimumSize(QSize(44, 0));
    m_btnMin->setMaximumSize(QSize(44, 16777215));
    m_btnMin->setCursor(QCursor(Qt::ArrowCursor));
    m_btnMin->setFocusPolicy(Qt::NoFocus);
    horizontalLayout->addWidget(m_btnMin);

    m_btnMax = new QPushButton(m_widgetMenu);
    IconHelper::Instance()->setIcon(m_btnMax, QChar(0xf069));
    QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Expanding);
    sizePolicy3.setHeightForWidth(m_btnMax->sizePolicy().hasHeightForWidth());
    m_btnMax->setSizePolicy(sizePolicy3);
    m_btnMax->setMinimumSize(QSize(44, 0));
    m_btnMax->setMaximumSize(QSize(44, 16777215));
    m_btnMax->setCursor(QCursor(Qt::ArrowCursor));
    m_btnMax->setFocusPolicy(Qt::NoFocus);
    horizontalLayout->addWidget(m_btnMax);

    m_btnClose = new QPushButton(m_widgetMenu);
    IconHelper::Instance()->setIcon(m_btnClose, QChar(0xf00d));
    sizePolicy3.setHeightForWidth(m_btnClose->sizePolicy().hasHeightForWidth());
    m_btnClose->setSizePolicy(sizePolicy3);
    m_btnClose->setMinimumSize(QSize(44, 0));
    m_btnClose->setMaximumSize(QSize(44, 16777215));
    m_btnClose->setCursor(QCursor(Qt::ArrowCursor));
    m_btnClose->setFocusPolicy(Qt::NoFocus);

    horizontalLayout->addWidget(m_btnClose);
    m_hLayoutTitle->addWidget(m_widgetMenu);
    m_vLayoutWidgetMain->addWidget(m_widgetTitle);

    m_widgetMainPane = new QWidget(m_widgetMain);
    m_widgetMainPane->setProperty("form", "basedlg");

    m_vLayoutWidgetMain->addWidget(m_widgetMainPane);
    m_layoutMain->addWidget(m_widgetMain);

    connect(m_btnMin, SIGNAL(clicked()), this, SLOT(OnBtnMinClicked()));
    connect(m_btnMax, SIGNAL(clicked()), this, SLOT(OnBtnMaxClicked()));
    connect(m_btnClose, SIGNAL(clicked()), this, SLOT(OnBtnCloseClicked()));

    setProperty("form", true);
    m_widgetTitle->setProperty("form", "title");
    setWindowFlags(windowFlags() | (Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint));

    //设置标题
    SetTitle("进程监控");
    SetTitleHeight(30);
    SetDlgFlags(Qt::WindowCloseButtonHint);

    //绑定事件过滤器监听鼠标移动
    m_widgetTitle->installEventFilter(this);

    m_widgetMain->setStyleSheet("QDialog{background:transparent;border:2px solid black;}");
}

void CBaseDlg::SetTitle( const QString &title )
{
    m_labelTitle->setText(title);
}

void CBaseDlg::SetTitleHeight( int nHeight )
{
    m_widgetTitle->setFixedHeight(nHeight);
    m_btnMin->setFixedWidth(nHeight);
    m_btnMax->setFixedWidth(nHeight);
    m_btnClose->setFixedWidth(nHeight);
}

void CBaseDlg::SetDlgFlags( Qt::WindowFlags type )
{
    // Qt::WindowMinimizeButtonHint  显示最小化按钮
    // Qt::WindowMaximizeButtonHint  显示最大化按钮
    // Qt::WindowMinMaxButtonsHint   显示最小化按钮和最大化按钮
    // Qt::WindowCloseButtonHint     显示关闭按钮

    m_btnMin->setVisible(false);
    m_btnMax->setVisible(false);
    m_btnClose->setVisible(false);

    if (type & Qt::WindowMinimizeButtonHint)
        m_btnMin->setVisible(true);
    if (type & Qt::WindowMaximizeButtonHint)
        m_btnMax->setVisible(true);
    if (type & Qt::WindowCloseButtonHint)
        m_btnClose->setVisible(true);
}

QWidget *CBaseDlg::centralWidget() const
{
    Q_ASSERT(NULL != m_widgetMainPane);

    return m_widgetMainPane;
}

bool CBaseDlg::eventFilter( QObject *obj, QEvent *evt )
{
    if (!m_bEnableMove || m_bWndMaxSize)
        return QWidget::eventFilter(obj, evt);

    static QPoint ptMouse;
    static bool bMousePressed = false;

    QMouseEvent *event = static_cast<QMouseEvent *>(evt);
    if (QEvent::MouseButtonPress == event->type())
    {
        if (obj == m_widgetTitle && Qt::LeftButton == event->button())
        {
            bMousePressed = true;
            ptMouse = event->globalPos() - pos();
            return true;
        }
    }
    else if (QEvent::MouseButtonRelease == event->type())
    {
        bMousePressed = false;
        return true;
    }
    else if (QEvent::MouseMove == event->type())
    {
        if (bMousePressed && (event->buttons() & Qt::LeftButton))
        {
            move(event->globalPos() - ptMouse);
            return true;
        }
    }

    return QWidget::eventFilter(obj, evt);
}

void CBaseDlg::OnBtnMinClicked()
{
    showMinimized();
}

void CBaseDlg::OnBtnMaxClicked()
{
    if (m_bWndMaxSize)
        setWindowState(Qt::WindowNoState);
    else
        setWindowState(Qt::WindowMaximized);

    m_bWndMaxSize = !m_bWndMaxSize;
}

void CBaseDlg::OnBtnCloseClicked()
{
    reject();
}

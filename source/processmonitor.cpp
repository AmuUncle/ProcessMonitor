#include "processmonitor.h"
#include "ui_processmonitor.h"
#include <QProcess>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtCore/QTime>
#include <QtCharts/QBarCategoryAxis>
#include <QFileDialog>
#include "iconhelper.h"
#include "tipwidget.h"
#include "aboutdlg.h"
#include "appkey.h"

#ifdef Q_OS_WIN32
#include <tlhelp32.h>
#include <tchar.h>
#include <ShellAPI.h>
#endif // Q_OS_WIN32

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#define LOG_PATH QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)

//接收调试信息的函数
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    QString strCurrentTime = (QDateTime::currentDateTime()).toString("yyyy-MM-dd");

    //将调试信息写入文件
    QFile file(QString("%1/%2.log").arg(LOG_PATH).arg(strCurrentTime));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << msg << "\r\n";
    file.flush();
    file.close();
}

ProcessMonitor::ProcessMonitor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProcessMonitor)
{
    ui->setupUi(this);
    m_bStart = false;
    m_thread = NULL;

    m_thread = new CMonitorThread(this);
    m_thread->Pause(true);
    m_thread->start();

    qDebug() << QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

    QChart *chart = new QChart();
    chart->setTitle("CPU(%)");
    chart->legend()->hide();
    chart->createDefaultAxes();
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTheme(QChart::ChartThemeBlueNcs);

    QLineSeries *seriesCpu = new QLineSeries(chart);
    chart->addSeries(seriesCpu);

    m_chartCpu = new QChartView(chart);
    m_chartCpu->setRenderHint(QPainter::Antialiasing);

    QChart *chartMem = new QChart();
    chartMem->setTitle("内存(MB)");
    chartMem->legend()->hide();
    chartMem->createDefaultAxes();
    chartMem->setAnimationOptions(QChart::SeriesAnimations);
    chartMem->setTheme(QChart::ChartThemeBlueNcs);

    QLineSeries *seriesMem = new QLineSeries(chartMem);
    chartMem->addSeries(seriesMem);

    m_chartMem = new QChartView(chartMem);
    m_chartMem->setRenderHint(QPainter::Antialiasing);

    QGridLayout *baseLayout = new QGridLayout(ui->m_widgetChart);
    baseLayout->addWidget(m_chartMem, 1, 0);
    baseLayout->addWidget(m_chartCpu, 1, 1);

    TTipWidget::Instance()->setParent(this);   //设置实例的引用者
    TTipWidget::Instance()->setVisible(false);

    InitCtrl();
    InitSolts();
    UpdateCtrlStatus();
}

ProcessMonitor::~ProcessMonitor()
{
#define SAFE_DELETE(pObj) \
    if (pObj != NULL) { delete pObj; pObj = NULL; }

    m_thread->StopTask();

    //停止线程
    m_thread->quit();

    //等待线程处理完手头工作
    m_thread->wait();

    TTipWidget::ExitInstance();
    SAFE_DELETE(m_thread);
    SAFE_DELETE(m_chartMem);
    SAFE_DELETE(m_chartCpu);

    delete ui;
}

void ProcessMonitor::contextMenuEvent( QContextMenuEvent *event )
{
    CreateMemu();
    event->accept();
}

void ProcessMonitor::InitCtrl()
{
    QDir dir;
    dir.mkpath(LOG_PATH);

    setProperty("canMove", true);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    ui->m_widgetMainToolBar->setProperty("form", "title");

    IconHelper::Instance()->setIcon(ui->m_btnMin, QChar(0xf068));
    IconHelper::Instance()->setIcon(ui->m_btnExit, QChar(0xf00d));

    ui->m_comboFreq->clear();
    ui->m_comboFreq->addItem("1s", 1);
    ui->m_comboFreq->addItem("2s", 2);
    ui->m_comboFreq->addItem("5s", 5);
    ui->m_comboFreq->addItem("10s", 10);
    ui->m_comboFreq->addItem("30s", 30);
    ui->m_comboFreq->addItem("1 min", 60);
    ui->m_comboFreq->addItem("2 min", 120);
    ui->m_comboFreq->addItem("5 min", 300);
    ui->m_comboFreq->addItem("10 min", 600);
    ui->m_comboFreq->addItem("30 min", 1800);
    ui->m_comboFreq->addItem("1 hour", 3600);
    ui->m_comboFreq->setCurrentIndex(0);

    ui->m_tableLog->setFocusPolicy(Qt::NoFocus);
    ui->m_tableLog->setSelectionBehavior(QAbstractItemView::SelectRows);  // 整行选中的方式
    ui->m_tableLog->setEditTriggers(QAbstractItemView::NoEditTriggers);    // 不可编辑
    ui->m_tableLog->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList strListHeader;
    strListHeader << tr("时间") << tr("程序") << tr("PID") << tr("内存(MB)") << tr("CPU(%)");
    ui->m_tableLog->setColumnCount(strListHeader.count());
    ui->m_tableLog->setHorizontalHeaderLabels(strListHeader);

    ui->m_tableLog->verticalHeader()->setHidden(true);    // 隐藏行号
    ui->m_tableLog->horizontalHeader()->setStretchLastSection(true);
    ui->m_tableLog->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->m_tableLog->setColumnWidth(0, 160);
    ui->m_tableLog->setColumnWidth(1, 130);
    ui->m_tableLog->setColumnWidth(2, 50);
    ui->m_tableLog->sortItems(0, Qt::DescendingOrder);

    ui->m_editPid->setValidator(new QIntValidator(1, 65535));
    ui->m_editPid->setText(QString("%1").arg(GetCurrentProcessId()));

    TTipWidget::Instance()->SetMesseage(tr("当前为试用版本，软件将在%1分钟后停止运行").arg(AppKey::Instance()->keyRunTimeOut()));  //提示信息
}

void ProcessMonitor::InitSolts()
{
    connect(ui->m_btnStart, SIGNAL(clicked()), this, SLOT(OnBtnClickedStart()));
    connect(ui->m_btnMin, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(ui->m_btnExit, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->m_checkChart, SIGNAL(stateChanged(int)), this, SLOT(OnCheckChartStateChanged(int)));
    connect(ui->m_checkLog, SIGNAL(stateChanged(int)), this, SLOT(OnCheckLogChanged(int)));
    connect(m_thread, SIGNAL(SignalProcState(bool, double, int, QString)), this, SLOT(OnProcState(bool, double, int, QString)));
    connect(AppKey::Instance(), SIGNAL(SignalIsOver()), this, SLOT(OnStop()));
}

void ProcessMonitor::UpdateCtrlStatus()
{
    if (m_bStart)
    {
        ui->m_btnStart->setText(tr("停止"));
    }
    else
    {
        ui->m_btnStart->setText(tr("开始"));
    }

    ui->m_widgetChart->setVisible(ui->m_checkChart->isChecked());
    ui->m_editPid->setDisabled(m_bStart);
    ui->m_comboFreq->setDisabled(m_bStart);
}

void ProcessMonitor::InsertChartData(int nIndex, double fUsedMem, int nCpu)
{
    if (fUsedMem >= 0.0)
    {
        QList<QAbstractSeries *>  listSeries = m_chartMem->chart()->series();
        if (listSeries.count() > 0)
        {
            QLineSeries *pSeries = (QLineSeries *)listSeries.at(0);

            while (pSeries->count() >= 100)
                pSeries->remove(0);

            pSeries->append(nIndex, fUsedMem);

            m_chartMem->chart()->createDefaultAxes();

            QList<QPointF> listPt = pSeries->points();
            if (listPt.count() > 0)
                m_chartMem->chart()->axisX()->setRange(listPt.at(0).x(), listPt.at(0).x() + 100);
            else
                m_chartMem->chart()->axisX()->setRange(0, 100);
        }

        double fMaxUsedMem = fUsedMem;
        for (int i = 0; i < ui->m_tableLog->rowCount(); i++) // 取出每个格子的内容
        {
            double fUsedMemTmp = 0.0;
            QTableWidgetItem *item = ui->m_tableLog->item(i, 3);
            if(item != NULL)
                fUsedMemTmp = item->text().toDouble();

            fMaxUsedMem = qMax(fMaxUsedMem, fUsedMemTmp);
        }

        m_chartMem->chart()->axisY()->setRange(0, fMaxUsedMem + 100);
    }

    if (nCpu >= 0)
    {
        QList<QAbstractSeries *>  listSeriesCpu = m_chartCpu->chart()->series();
        if (listSeriesCpu.count() > 0)
        {
            QLineSeries *pSeries = (QLineSeries *)listSeriesCpu.at(0);
            while (pSeries->count() >= 100)
                pSeries->remove(0);

            pSeries->append(nIndex, nCpu);

            m_chartCpu->chart()->createDefaultAxes();

            QList<QPointF> listPt = pSeries->points();
            if (listPt.count() > 0)
                m_chartCpu->chart()->axisX()->setRange(listPt.at(0).x(), listPt.at(0).x() + 100);
            else
                m_chartCpu->chart()->axisX()->setRange(0, 100);
        }

        m_chartCpu->chart()->axisY()->setRange(0, 100);
    }
}

void ProcessMonitor::CreateMemu()
{
    // 创建菜单对象
    QMenu *pMenu = new QMenu(this);

    {
        QAction *action = new QAction(tr("打开日志目录"), this);
        action->setData(0);
        pMenu->addAction(action);
    }

    if (ui->m_checkChart->isChecked())
    {
        QAction *action = new QAction(tr("保存实时曲线图"), this);
        action->setData(1);
        pMenu->addAction(action);
    }

    {
        QAction *action = new QAction(tr("关于"), this);
        action->setData(2);
        pMenu->addAction(action);
    }

    connect(pMenu, SIGNAL(triggered(QAction *)), this, SLOT(OnMenuTriggered(QAction *)));
    pMenu->exec(cursor().pos());

    // 释放内存
    QList<QAction*> listAction = pMenu->actions();
    foreach (QAction* action, listAction)
        delete action;

    delete pMenu;
}

void ProcessMonitor::OnBtnClickedStart()
{
    int nPid = ui->m_editPid->text().toInt();

    if (m_bStart)
    {
        m_bStart = false;
        m_thread->Pause(true);
    }
    else
    {
        if (nPid <= 0)
        {
            TTipWidget::Instance()->SetMesseage(tr("进程不存在!"));  //提示信息
            return;
        }

        QList<QAbstractSeries *>  listSeries = m_chartMem->chart()->series();
        if (listSeries.count() > 0)
        {
            QLineSeries *pSeries = (QLineSeries *)listSeries.at(0);
            pSeries->clear();
        }

        listSeries = m_chartCpu->chart()->series();
        if (listSeries.count() > 0)
        {
            QLineSeries *pSeries = (QLineSeries *)listSeries.at(0);
            pSeries->clear();
        }

        ui->m_tableLog->setRowCount(0);
        ui->m_tableLog->clearContents();
        m_bStart = true;

        m_thread->SetInterval(ui->m_comboFreq->currentData().toInt());
        m_thread->SetParam(nPid);
        m_thread->Pause(false);
    }

    UpdateCtrlStatus();
}

void ProcessMonitor::OnProcState(bool bSuc, double fUsedMem, int nCpu, QString strExeName)
{
    if (!bSuc)
    {
        m_bStart = false;
        m_thread->Pause(true);
        TTipWidget::Instance()->SetMesseage(tr("进程不存在!"));  //提示信息
        UpdateCtrlStatus();
        return;
    }
    else
    {
        QString strCurrentTime = (QDateTime::currentDateTime()).toString("yyyy-MM-dd hh:mm:ss.zzz");

        ui->m_tableLog->setSortingEnabled(false);
        int nRowIndex = ui->m_tableLog->rowCount();
        ui->m_tableLog->setRowCount(nRowIndex + 1);         // 总行数增加1

        ui->m_tableLog->setItem(nRowIndex, 0, new QTableWidgetItem(QString("%1").arg(strCurrentTime)));
        ui->m_tableLog->setItem(nRowIndex, 1, new QTableWidgetItem(strExeName));
        ui->m_tableLog->setItem(nRowIndex, 2, new QTableWidgetItem(ui->m_editPid->text()));
        ui->m_tableLog->setItem(nRowIndex, 3, new QTableWidgetItem(QString::number(fUsedMem / 1024, 'g', 4)));
        ui->m_tableLog->setItem(nRowIndex, 4, new QTableWidgetItem(QString("%1").arg(nCpu)));
        ui->m_tableLog->setSortingEnabled(true);

        if (nRowIndex + 1 > 100)
            ui->m_tableLog->removeRow(nRowIndex + 1);

        InsertChartData(nRowIndex, fUsedMem / 1024, nCpu);

        qDebug() << qPrintable(QString("%1").arg(strCurrentTime))
                     << qPrintable(strExeName)
                     << qPrintable(ui->m_editPid->text())
                     << qPrintable(QString("%1MB").arg(QString::number(fUsedMem / 1024, 'g', 4)) )
                     << qPrintable(QString("%1%").arg(nCpu));
    }
}

void ProcessMonitor::OnCheckChartStateChanged(int nState)
{
    UpdateCtrlStatus();
}

void ProcessMonitor::OnCheckLogChanged(int nState)
{
    if (ui->m_checkLog->isChecked())
    {
        qInstallMessageHandler(outputMessage);
        TTipWidget::Instance()->SetMesseage(tr("日志保存目录:%1").arg(LOG_PATH));


    }
    else
        qInstallMessageHandler(nullptr);
}

void ProcessMonitor::OnMenuTriggered(QAction *action)
{
    int nType = action->data().toInt();
    switch (nType)
    {
    case 0:
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(LOG_PATH.replace("\\", "/")));
        }
        break;

    case 1:
        {
            QString fileName = QFileDialog::getSaveFileName(this,
                                tr("保存文件"),
                                "member_and_cpu.jpg",
                                tr("图片文件(*.jpg)"));

            if (!fileName.isNull())
            {
                QPixmap pixChart = ui->m_widgetChart->grab();
                pixChart.toImage().save(fileName);
            }
        }
        break;

    case 2:
        {
            CAboutDlg dlg(this);
            dlg.EnableMoveWindow(true);
            dlg.exec();
        }
        break;
    }
}

void ProcessMonitor::OnStop()
{
    m_bStart = false;
    m_thread->Pause(true);
    UpdateCtrlStatus();
}

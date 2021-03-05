#ifndef PROCESSMONITOR_H
#define PROCESSMONITOR_H

#include <QWidget>
#include <QTimer>
#include "monitorthread.h"
#include <QtWidgets/QWidget>
#include <QtCharts/QChartGlobal>

QT_CHARTS_BEGIN_NAMESPACE
class QChartView;
class QChart;
QT_CHARTS_END_NAMESPACE

namespace Ui {
class ProcessMonitor;
}

QT_CHARTS_USE_NAMESPACE

class ProcessMonitor : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessMonitor(QWidget *parent = 0);
    ~ProcessMonitor();

private:
    void InitCtrl();
    void InitSolts();
    void Relayout();
    void UpdateCtrlStatus();
    void InsertChartData(int nIndex, double fUsedMem, int nCpu);

private slots:
    void OnBtnClickedStart();
    void OnProcState(bool bSuc, double fUsedMem, int nCpu, QString strExeName);
    void OnCheckChartStateChanged(int nState);

private:
    Ui::ProcessMonitor *ui;

    bool m_bStart;
    CMonitorThread *m_thread;
    QChartView * m_chartCpu;
    QChartView * m_chartMem;
};

#endif // PROCESSMONITOR_H

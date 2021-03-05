#include "monitorthread.h"
#include <time.h>
#include <windows.h>
#include <psapi.h>
#include <assert.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <ShellAPI.h>
#include <QProcess>
#include <QDebug>
#include <QDateTime>

#pragma comment( lib, "psapi.lib" )

CMonitorThread::CMonitorThread(QObject *parent) : QThread(parent)
{
    m_bQuitQueryTask = false;
    m_nPid = 0;
    m_nInterval = 1;
    m_bPause = false;
}

CMonitorThread::~CMonitorThread()
{
    m_bQuitQueryTask = true;
}

static double GetUsedMem(int nPid, QString &strExeName)
{
    // 获取当前进程PID
    QProcess p;
    p.start("tasklist /FI \"PID EQ " + QString::number(nPid) + " \"");
    p.waitForFinished();

    QString result = QString::fromLocal8Bit(p.readAllStandardOutput());
    p.close();

    if (!result.contains(".exe"))
    {
//        m_pTimer->stop();
//        //QMessageBox::information(NULL, tr("错误"), tr("进程不存在!"), QMessageBox::Yes, QMessageBox::Yes);
//        ui->m_labelTip->setText(tr("进程不存在!"));
        return -1;
    }

    // 替换掉","
    result = result.replace(",", "");

    QRegExp rxExeName("(\\S+)(.exe)");
    if(rxExeName.indexIn(result) != -1)
        strExeName = rxExeName.cap(0);

    // 匹配 '数字+空格+K'部分。
    QRegExp rxUsedMem("(\\d+)(\\s)(K)");
    // 初始化结果
    QString strUsedMem("");
    if(rxUsedMem.indexIn(result) != -1)
        strUsedMem = rxUsedMem.cap(0);

    // 截取K前面的字符串，转换为数字，供换算单位使用。
    strUsedMem = strUsedMem.left(strUsedMem.length() - 1);
    return strUsedMem.toDouble();
}

// 时间转换
static __int64 file_time_2_utc(const FILETIME* ftime)
{
  LARGE_INTEGER li;

  li.LowPart = ftime->dwLowDateTime;
  li.HighPart = ftime->dwHighDateTime;
  return li.QuadPart;
}

// 获得CPU的核数
static int get_processor_number()
{
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return (int)info.dwNumberOfProcessors;
}

// 获取进程CPU占用
static int get_cpu_usage(int pid)
{
    //cpu数量
    static int processor_count_ = -1;
    //上一次的时间
    static __int64 last_time_ = 0;
    static __int64 last_system_time_ = 0;

    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    __int64 system_time;
    __int64 time;
    __int64 system_time_delta;
    __int64 time_delta;

    int cpu = -1;

    if(processor_count_ == -1)
    {
        processor_count_ = get_processor_number();
    }

    GetSystemTimeAsFileTime(&now);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
    {
        return -1;
    }
    system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / processor_count_;
    time = file_time_2_utc(&now);

    if ((last_system_time_ == 0) || (last_time_ == 0))
    {
        last_system_time_ = system_time;
        last_time_ = time;
        return -1;
    }

    system_time_delta = system_time - last_system_time_;
    time_delta = time - last_time_;

    if (time_delta == 0)
        return -1;

    cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time_ = system_time;
    last_time_ = time;
    return cpu;
}

void CMonitorThread::SetParam(int nPid)
{
    m_nPid = nPid;
}

void CMonitorThread::SetInterval(int nInterval)
{
    m_nInterval = nInterval;
}

void CMonitorThread::run()
{
    int nInterval = 0;
    QString strExeName;
    while (!m_bQuitQueryTask)
    {
        if (m_bPause || nInterval < m_nInterval)
        {
            nInterval++;
            QThread::sleep(1);
            continue;
        }

        if (m_nPid <= 0)
            continue;

        double fUsedMem = GetUsedMem(m_nPid, strExeName);
        if (fUsedMem < 0)
        {
            emit SignalProcState(false, 0, 0, strExeName);
            continue;
        }

        int nCpu = get_cpu_usage(m_nPid);
        if (nCpu >= 0 && nCpu <= 100)
        {
            nInterval = 0;
            emit SignalProcState(true, fUsedMem, nCpu, strExeName);
        }

        QThread::msleep(100);
    }
}


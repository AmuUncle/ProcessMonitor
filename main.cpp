#include "processmonitor.h"
#include <QApplication>
#include <QMutex>
#include <QMutexLocker>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QIcon>
#include "appinit.h"

//接收调试信息的函数
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    QMutexLocker lock(&mutex);

    QString strCurrentTime = (QDateTime::currentDateTime()).toString("yyyy-MM-dd");

    //将调试信息写入文件
    QFile file(QString("%1.log").arg(strCurrentTime));
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << msg << "\r\n";
    file.flush();
    file.close();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(outputMessage);

    QApplication a(argc, argv);

    AppInit::Instance()->start();  // 使整个窗口可以拖动

    //加载样式表
    QFile file(":/qss/black.css");
    if (file.open(QFile::ReadOnly))
    {
        QString qss = QLatin1String(file.readAll());
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(QColor(paletteColor)));
        qApp->setStyleSheet(qss);
        file.close();
    }

    //更改程序启动后任务栏图标
    a.setWindowIcon(QIcon(":/image/Monitor.ico"));

    ProcessMonitor w;
    w.show();

    return a.exec();
}

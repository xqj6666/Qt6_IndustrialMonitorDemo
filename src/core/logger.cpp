#include "logger.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QCoreApplication>
#include <QDir>

Logger::Logger(QObject *parent)
    :QObject(parent)
{
    initLogFile();
}

Logger::~Logger()
{
    if(m_stream){//???
        m_stream->flush();
        delete m_stream;
    }
    if(m_logFile){
        m_logFile->close();
        delete m_logFile;
    }
}

Logger* Logger::instance()
{
    static Logger instance;
    return &instance;
}

void Logger::debug(const QString &message)
{
    instance()->write(Level::Debug,message);
}

void Logger::info(const QString &message)
{
    instance()->write(Level::Info,message);
}

void Logger::warning(const QString &message)
{
    instance()->write(Level::Warning,message);
}

void Logger::error(const QString &message)
{
    instance()->write(Level::Error,message);
}

void Logger::initLogFile()
{
    //日志目录：可执行文件同级目录/logs/
    QString logDir = QCoreApplication::applicationDirPath() + "/logs";
    QDir dir(logDir);
    if(!dir.exists()){//如何logs不存在则创建
        dir.mkpath(logDir);
    }

    //日志文件名按照日期命名:2026-08-21.log
    QString fileName = logDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd")+".log";
    m_logFile = new QFile(fileName);
    m_logFile->open(QIODevice::Append | QIODevice::Text);;//追加模式
    m_stream = new QTextStream(m_logFile);
    m_stream->setEncoding(QStringConverter::Utf8);
}

void Logger::write(Level level, const QString &message)
{
    QMutexLocker locker(&m_mutex);//加锁，线程安全

    QString formatted = formatMessage(level,message);//???

    if(m_stream){
        *m_stream<<formatted<<"\n";
        m_stream->flush();
    }

    //发信号给UI
    emit logMessageReady(formatted, level);
}

QString Logger::formatMessage(Level level, const QString &message) const
{
    //格式：[2025-07-08 14:30:05] [INFO] 谁被已连接
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    return QString("[%1] [%2] %3").arg(timestamp,levelToString(level),message);
}

QString Logger::levelToString(Level level) const
{
    switch(level)
    {
    case Level::Debug:   return "DEBUG";
    case Level::Info:    return "INFO";
    case Level::Warning: return "WARNING";
    case Level::Error:   return "ERROR";
    }
    return "UNKNOWN";
}















































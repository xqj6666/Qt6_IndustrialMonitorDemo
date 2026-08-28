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
    if(m_stream){
        m_stream->flush();//把缓冲区残留日志强制刷入磁盘，防止程序退出日志丢在内存没写入硬盘
        delete m_stream;
        m_stream = nullptr;//如果析构之后有人意外访问此指针，置nullptr可以让程序奔溃在明确的位置，而不是随机崩溃
    }
    if(m_logFile){
        m_logFile->close();//关闭磁盘文件句柄
        delete m_logFile;
        m_logFile = nullptr;
    }
}

Logger* Logger::instance()
{
    static Logger instance;
    return &instance;
}

void Logger::debug(const QString &message)//这个写法调用代码的时候更加简洁
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
    QString logDir = QCoreApplication::applicationDirPath() + "/logs";//拼接日志文件夹
    QDir dir(logDir);//不会创建目录，只是一个目录句柄
    if(!dir.exists()){//如果logs文件夹不存在则创建
        dir.mkpath(logDir);//mkpath会处理文件存在的情况,但是先判断文件是否存在，可读性好，语义清晰
    }

    //日志文件名按照日期命名:2026-08-21.log
    QString fileName = logDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd")+".log";//按照当前日期生成日志文件名
    m_logFile = new QFile(fileName);//在堆上创建QFile对象，只是把对象和文件路径绑定，不会创建磁盘文件也不会打开文件
    m_logFile->open(QIODevice::Append | QIODevice::Text);;//追加模式,如果文件不存在自动创建，WriteOnly会把之前的清空
    m_stream = new QTextStream(m_logFile);
    m_stream->setEncoding(QStringConverter::Utf8);//确保中文日志不会乱码
}

void Logger::write(Level level, const QString &message)
{
    QMutexLocker locker(&m_mutex);//加锁，线程安全，多线程同时写日志，保证串行执行
    //RALL包装器核心思想：构造函数获取资源，析构函数自动释放资源，来开作用域自动清理，不用手写释放代码，避免泄露
    //RALL包装器=把裸资源(指针、句柄、fd、socket、锁)封装成一个类/模版对象,这个对象就是RALL包装器
    //等价于m_mutex.lock();...执行操作...m_mutex.unlock();即使中间抛出异常，析构函数也会执行，锁一定会释放，手动lock/unlcok的话，异常可能导致死锁

    QString formatted = formatMessage(level,message);//把时间、级别、日志内容拼接成格式化后的字符串

    if(m_stream){
        *m_stream<<formatted<<"\n";//先写把消息写入缓冲区，换行
        m_stream->flush();//强制缓冲区内容写入磁盘，防止崩溃丢失日志，不flush的情况下，会攒够一定数据量才会写一次
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

//用QMetaEnum进行转换，后续添加枚举值不用修改函数，但是没有上面的函数可读性好
//QString Logger::levelToString(Level level) const
//{
//    const QMetaEnum meta = QMetaEnum::fromType<Level>();
//   const char *key = meta.valueToKey(static_cast<int>(level));
//    return key ? QString(key) : QStringLiteral("UNKNOWN");
//}















































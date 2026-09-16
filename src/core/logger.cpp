#include "logger.h"

#include <QFile>
#include <QTextStream>
//处理日期+时间组合
#include <QDateTime>
//Qt 框架控制台非GUI程序的应用程序入口类
#include <QCoreApplication>
//目录操作类
#include <QDir>

Logger::Logger(QObject *parent)
    :QObject(parent)
{
    m_available = (QCoreApplication::instance() != nullptr);
    if(!m_available){
        qWarning()<<"Logger：在 QCoreApplication 创建之前构造，文件日志不可用";
    }
}

//新增init():显示初始化，幂等，幂等是什么意思？？？
void Logger::init()
{
    auto *logger = instance();
    if(logger->m_initialized){
        return;//已经初始化过，直接返回
    }
    logger->m_initialized = true;
    if(logger->m_available){
        logger->initLogFile();
    }
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
    //拼接日志文件夹
    QString logDir = QCoreApplication::applicationDirPath() + "/logs";
    QDir dir(logDir);//把路径绑定给一个QDir对象，相当于给路径起了个能调方法的句柄
    if(!dir.exists()){//如果logs文件夹不存在则创建
        dir.mkpath(logDir);//递归创建，mkpath会处理文件存在的情况,但是先判断文件是否存在，可读性好，语义清晰，如果创建失败代码没有感知，需要修复()()()
    }

    //日志文件名按照日期命名:2026-08-21.log
    QString fileName = logDir + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd")+".log";//按照当前日期生成日志文件名，（）（）缺陷：日期只在构造函数里取一次，如果程序跨天运行，则出错
    //先创建局部对象，不直接赋值给成员
    QFile* file = new QFile(fileName);
    if(!file->open(QIODevice::Append | QIODevice::Text))
    {
        //失败的话把错误打出来,release版看不到
        qWarning() << "Logger：打开日志文件失败：" << fileName << " (" << file->errorString() << ")";
        //释放刚分配的内存
        delete file;
        return;
        //为什么不用QMessageBox提醒用户：
        //1.时机不允许，main里的任何一行都可能触发，QMessageBox要求QApplication存在、时间循环可用;
        //2.职责分离(架构原则)：Logger的职责是写日志，不是弹窗。弹窗是UI层的职责，日志系统应该在没有GUI的环境下也能工作
        //3.模态弹窗会阻塞，如果在工作线程弹窗，GUI线程可能正在等这个工作线程的结果，死锁
        //4.QMessageBox的正确使用场景是“用户主动操作失败，且用户能做出决策”
        //想UI层提示可以发送信号，让UI层决定如何呈现
    }

    m_logFile = file;
    m_stream = new QTextStream(m_logFile);
    m_stream->setEncoding(QStringConverter::Utf8);
}

void Logger::write(Level level, const QString &message)
{
    QMutexLocker locker(&m_mutex);

    QString formatted = formatMessage(level,message);

    if (m_available && m_stream) {
        *m_stream << formatted << "\n";
        m_stream->flush();
    }

    //发信号给UI
    emit messageLogged(formatted, level);
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

















































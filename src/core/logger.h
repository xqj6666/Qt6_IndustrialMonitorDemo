#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMutex>//互斥锁，保护线程安全

QT_BEGIN_NAMESPACE
class QFile;
class QTextStream;
QT_END_NAMESPACE

class Logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Logger)

public:
    //日志级别枚举
    enum class Level{
        Debug,
        Info,
        Warning,
        Error
    };
    Q_ENUM(Level);//作用是什么？？？

    //全局访问点
    static Logger *instance();

    //对外接口：写日志
    static void debug  (const QString &message);
    static void info   (const QString &message);
    static void warning(const QString &message);
    static void error  (const QString &message);

signals:
    //发送给UI的信号，带格式化好的日志字符串
    void logMessageReady(const QString &formattedMsg,Logger::Level level);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger() override;

    void initLogFile();
    void write(Level level,const QString &message);
    QString formatMessage(Level level,const QString &message) const;
    QString levelToString(Level level) const;

private:
    QFile       *m_logFile = nullptr;
    QTextStream *m_stream  = nullptr;
    QMutex       m_mutex;

};

#endif // LOGGER_H



















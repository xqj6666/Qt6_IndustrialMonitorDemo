#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMutex>//互斥锁，保护线程安全

QT_BEGIN_NAMESPACE//减少编译依赖，加快编译的速度
class QFile;
class QTextStream;
QT_END_NAMESPACE

class Logger : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Logger)//防止双重释放和悬空指针

public:
    //强类型枚举，不会污染外部作用域,必须用Level::Debug访问,不可以隐式转换为int,命名不冲突
    enum class Level{
        Debug,
        Info,
        Warning,
        Error
    };
    Q_ENUM(Level);//把C++枚举注册到Qt元对象系统，1.信号槽参数可以传这个枚举，2.用QMetaEnum进行枚举值和字符串的互转
                  //3.在QML中直接使用 4.配合qDebug()自动大于枚举名而不是数字

    //全局单例访问点
    static Logger *instance();//C++11保证线程安全,两个线程同时第一次调用，不会创建两个对象，编译器会保证线程安全
                              //即使多个线程同时第一次进入，也指挥有一个线程执行初始化，另一个等初始化完成后直接使用

    //对外接口：写日志
    static void debug  (const QString &message);
    static void info   (const QString &message);
    static void warning(const QString &message);
    static void error  (const QString &message);

signals:
    //发送给UI的信号，带格式化好的日志字符串,信号不需要自己实现,连接信号的时候要inclue
    void logMessageReady(const QString &formattedMsg,Logger::Level level);

private:
    explicit Logger(QObject *parent = nullptr);
    ~Logger() override;

    void initLogFile();//初始化日志文件，只在构造函数中调用一次
    void write(Level level,const QString &message);//核心写日志方法
    QString formatMessage(Level level,const QString &message) const;//将原始消息格式化为完整日志行
    QString levelToString(Level level) const;//将枚举转换成可读字符串，不是可以用QmetaEnum吗？？？

private:
    QFile       *m_logFile = nullptr;
    QTextStream *m_stream  = nullptr;
    QMutex       m_mutex;

};

#endif // LOGGER_H



















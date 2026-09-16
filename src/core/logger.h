//===全局单例，日志类===
#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
//互斥锁，保护线程安全
#include <QMutex>
//Qt元对象系统提供的枚举操作类：枚举值多且频繁增删改(开发期工具，不稳定契约)；需要双向转换；需要遍历所有枚举、要和QML/序列化系统对接

QT_BEGIN_NAMESPACE
//Qt跨平台文件IO通道类，只能写入字节，支持文件/二进制读写
class QFile;
//负责字符和字节互转，本生不操作文件必须绑定QIODevice(QFile、QBuffer、QTcpSokcet都可以)
class QTextStream;
QT_END_NAMESPACE

class Logger : public QObject
{
    Q_OBJECT
    //禁止拷贝和移动：1.QObject自带父子对象内存管理机制，如果允许拷贝，一个子对象被多个父对象重复析构，内存崩溃 2.拷贝后信号槽和线程归属混乱
    Q_DISABLE_COPY_MOVE(Logger)

public:
    //强类型枚举，不会污染外部作用域,必须用Level::Debug访问,不可以隐式转换为int,命名不冲突，可以指定底层类型，和enum struct一样
    //无作用域枚举enum Level名字暴露在当前作用域，隐式转换，不能单独指定底层类型
    enum class Level{
        Debug,
        Info,
        Warning,
        Error
    };
    //注册到Qt元系统，在跨线程发送信号时，可以成功识别并打包，解决的是运行时问题。  inclue解决的是编译器问题
    Q_ENUM(Level);

    //全局单例访问点
    //C++11保证线程安全,两个线程同时第一次调用，不会创建两个对象，编译器会保证线程安全
    //即使多个线程同时第一次进入，也指挥有一个线程执行初始化，另一个等初始化完成后直接使用
    static Logger *instance();
    static void init();//9.15新增，显示初始化入口：对象创建和资源初始化分离

    //对外接口：写日志
    static void debug  (const QString &message);
    static void info   (const QString &message);
    static void warning(const QString &message);
    static void error  (const QString &message);

signals:
    //Logger -> mainwindow通知主窗口更新日志框
    void messageLogged(const QString &formattedMsg,Logger::Level level);


private:
    //禁止构造函数进行隐式类型转换
    explicit Logger(QObject *parent = nullptr);
    ~Logger() override;

    void initLogFile();//初始化日志文件，只在构造函数中调用一次
    void write(Level level,const QString &message);//核心写日志方法
    QString formatMessage(Level level,const QString &message) const;//将原始消息格式化为完整日志行
    //更符合工业真实用法：编译器检查、输出格式可空、运行开销为零
    QString levelToString(Level level) const;

private:
    //默认用值成员，只有四种情况才用指针：可空、需要多态、需要延迟创建、生命周期独立于父对象
    QFile       *m_logFile = nullptr;
    QTextStream *m_stream  = nullptr;
    //值成员，生命周期天然匹配，避免悬空指针，性能好
    QMutex       m_mutex;

    bool m_initialized = false;
    bool m_available = false;

};

#endif // LOGGER_H



















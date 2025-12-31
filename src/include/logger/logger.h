/*************************************************
 * 描述：
 *
 * File：logger.h
 * Author：Cipher
 * Date：2025/12/31-10:45
 * Update：
 * ************************************************/

#ifndef RAREVOYAGER_LOGGER_H
#define RAREVOYAGER_LOGGER_H

#include <cstdint>
#include <list>
#include <memory>
#include <string>

namespace RareVoyager {
    /**
     * @brief: 一条完整的日志记录
     */
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();
    private:
        const char* m_file = nullptr;       // 文件名
        int32_t m_line = 0;                 // 行号
        uint32_t m_elapse = 0;              // 程序启动到现在的毫秒数
        uint32_t m_threadId = 0;            // 线程id
        uint32_t m_fiberId = 0;             // 协程id(一个线程包含多个协程)
        uint64_t m_time;                    // 时间戳
        std::string m_content;              // 输出文本
    };

    /**
     * @brief: 日志等级
     */
    class LogLevel {
        enum Level {
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
    };

    /**
     * @brief: 日志最终输出形式 json 或 一串字符串
     */
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        std::string format(LogEvent::std::shared_ptr<LogEvent> event);
    };

    /**
     * @brief: 日志最终输出位置，例如控制台或文件
     */
    class LogAppender {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender();
        void log(LogLevel::Level level,LogEvent::ptr event);
    private:
        LogLevel::Level m_level;
    };

    /**
     * @brief: 用户使用
     */
    class Logger {
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(const std::string& name = "root");
        void log(LogLevel::Level level,LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
    private:
        std::string m_name;                     // 日志名称
        LogLevel::Level m_level;                // 日志级别
        std::list<LogAppender::ptr> m_appenders;// Appender集合

    };
}

#endif //RAREVOYAGER_LOGGER_H
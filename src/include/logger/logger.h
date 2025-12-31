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
#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

namespace RareVoyager {

    class Logger;
#pragma region LogEvent
    /**
     * @brief: 一条完整的日志记录
     */
    class LogEvent {
    public:
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(const char* file,int32_t line,uint32_t elapse
            ,uint32_t threadId,uint32_t fiberId,
            uint32_t time);

        const char *getFile() { return m_file; }

        [[nodiscard]] int32_t getLine() const { return m_line; }
        [[nodiscard]] uint32_t getElapse() const { return m_elapse; }
        [[nodiscard]] uint32_t getThreadId() const { return m_threadId; }
        [[nodiscard]] uint32_t getFiberId() const { return m_fiberId; }
        [[nodiscard]] uint64_t getTime() const { return m_time; }
        [[nodiscard]] std::string getContent() const { return m_ss.str(); }
        [[nodiscard]] const std::stringstream& getSS() const { return m_ss; }
    private:
        const char *m_file = nullptr; // 文件名
        int32_t m_line = 0; // 行号
        uint32_t m_elapse = 0; // 程序启动到现在的毫秒数
        uint32_t m_threadId = 0; // 线程id
        uint32_t m_fiberId = 0; // 协程id(一个线程包含多个协程)
        uint64_t m_time; // 时间戳
        std::stringstream m_ss; // 输出文本
    };
#pragma endregion LogEvent

#pragma region LogLevel
    /**
     * @brief: 日志等级
     */
    class LogLevel {
    public:
        enum Level {
            UNKONW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };

        // 转换成一条字符串输出
        const static char *ToString(LogLevel::Level level);
    };
#pragma endregion LogLevel

#pragma region LogFormatter
    /**
     * @brief: 日志最终输出形式 json 或 一串字符串
     */
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(std::string pattern);

        /**
         * @brief 返回格式化日志文本
         * @param[in] logger 日志器
         * @param[in] level 日志级别
         * @param[in] event 日志事件
         */
        std::string format(const std::shared_ptr<Logger>& logger, LogLevel::Level level, const LogEvent::ptr& event);

    public:
        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            FormatItem(std::string format = "");
            virtual ~FormatItem() = default;

            /**
             * @brief 格式化日志到流
             * @param[in, out] os 日志输出流
             * @param[in] logger 日志器
             * @param[in] level 日志等级
             * @param[in] event 日志事件
             */
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        };

        /**
         * @brief: 这段代码是日志系统的解析核心，
         * 其目的是将一个类似 %d{%Y-%m-%d} [%p] %m%n 的模板字符串
         * 解析成一系列可执行的格式化单元（FormatItem）。
         */
        void init();

    private:
        /// 日志格式模板
        std::string m_pattern;
        /// 日志格式解析后格式
        std::vector<FormatItem::ptr> m_items;
    };
#pragma endregion LogFormatter

#pragma region LogAppender
    /**
     * @brief: 日志最终输出位置，例如控制台或文件
     */
    class LogAppender {
    public:
        typedef std::shared_ptr<LogAppender> ptr;

        virtual ~LogAppender() = default;

        virtual void log(std::shared_ptr<Logger> logger ,LogLevel::Level level, LogEvent::ptr event) = 0;

        virtual LogFormatter::ptr getFormatter();

        virtual void setFormatter(LogFormatter::ptr formatter);


    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };
#pragma endregion LogAppender

#pragma region Logger
    /**
     * @brief: 用户使用
     * std::enable_shared_from_this<Logger> 用来让对象在成员函数内部，获得指向自身的指针
     */
    class Logger: public std::enable_shared_from_this<Logger> {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(std::string name = "root");

        void log(LogLevel::Level level, const LogEvent::ptr &event);

        void debug(const LogEvent::ptr &event);

        void info(const LogEvent::ptr &event);

        void warn(const LogEvent::ptr &event);

        void error(const LogEvent::ptr &event);

        void fatal(const LogEvent::ptr &event);

        // 返回值不能被忽略
        [[nodiscard]] LogLevel::Level getLevel() const;

        void setLevel(LogLevel::Level level);

        void addAppender(const LogAppender::ptr &appender);

        void delAppender(const LogAppender::ptr &appender);

        [[nodiscard]] std::string getName() const{return m_name;}

    private:
        std::string m_name; // 日志名称
        LogLevel::Level m_level; // 日志级别
        std::list<LogAppender::ptr> m_appenders; // Appender集合
        LogFormatter::ptr m_formatter;
    };
#pragma endregion Logger

#pragma region StdoutLogAppender
    // 输出到控制台的Appender
    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;

        void log(std::shared_ptr<Logger> ptr,LogLevel::Level level, LogEvent::ptr event) override;
    };
#pragma endregion StdoutLogAppender

#pragma region FileLogAppender
    // 输出到文件的Appender
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;

        FileLogAppender(const std::string &filename);

        void log(std::shared_ptr<Logger> ptr ,LogLevel::Level level, LogEvent::ptr event) override;

        bool reopen();

    private:
        std::string m_filename; // 文件名
        std::ofstream m_fileStream; // 文件写入流
    };
#pragma endregion FileLogAppender
}

#endif //RAREVOYAGER_LOGGER_H

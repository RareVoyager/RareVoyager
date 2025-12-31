#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <utility>

#include <include/logger/logger.h>


namespace RareVoyager {
#pragma region LogEvent
    LogEvent::LogEvent(const char *file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId,
    uint32_t time)
    :m_file(file), m_line(line),m_elapse(elapse),
    m_threadId(threadId),m_fiberId(fiberId),m_time(time)
    {}
#pragma endregion LogEvent

#pragma region  FormatItem
    LogFormatter::FormatItem::FormatItem(std::string format) {
    }
    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            // 获取文本
            os << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            // 获取日志级别
            os << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            // 获取程序运行时间(ms)
            os << event->getElapse();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << logger->getName();
        }
    };

    class ThreadIdFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadIdFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };

    // class ThreadNameFormatItem : public LogFormatter::FormatItem {
    // public:
    //     ThreadNameFormatItem(const std::string &str = "") {
    //     }
    //
    //     void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
    //         os << event->getThreadName();
    //     }
    // };

    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
            : m_format(format) {
            if (m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            // struct tm tm;
            // time_t time = event->getTime();
            // localtime_r(&time, &tm);
            // char buf[64];
            // strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            // os << buf;
            os << event->getTime();
        }

    private:
        std::string m_format;
    };

    class FilenameFormatItem : public LogFormatter::FormatItem {
    public:
        FilenameFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFile();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };


    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string &str)
            : m_string(str) {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << m_string;
        }

    private:
        std::string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string &str = "") {
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << "\t";
        }

    private:
        std::string m_string;
    };
#pragma endregion FormatItem

#pragma region  LogLevel
    // TODO: X-Macro 优化
    const char *LogLevel::ToString(LogLevel::Level level) {
        switch (level) {
#define XX(name) \
      case LogLevel::name:\
                return #name;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
            default:
                return "UNKNOW";
        }
        return "UNKNOW";
    }

    LogFormatter::LogFormatter(std::string pattern) : m_pattern(std::move(pattern)) {
        init();
    }

    std::string LogFormatter::format(const std::shared_ptr<Logger> &logger, LogLevel::Level level,
                                     const LogEvent::ptr &event) {
        std::stringstream ss;
        for (const auto &i: m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }



#pragma endregion LogLevel

#pragma region LogFormatter
    void LogFormatter::FormatItem::format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level,
                                          LogEvent::ptr event) {
    }

    // 日志格式解析
    void LogFormatter::init() {
    // 1. 所有的元组解析逻辑
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;

    for (size_t i = 0; i < m_pattern.size(); ++i) {
        if (m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue; // 这里跳过，直到遇到 %
        }

        // 遇到 %%
        if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%') {
            nstr.append(1, '%');
            i++; // 跳过第二个 %
            continue;
        }

        // 解析 %xxx{xxx}
        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;
        std::string str, fmt;

        while (n < m_pattern.size()) {
            if (!fmt_status && !isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}') {
                break;
            }
            if (fmt_status == 0 && m_pattern[n] == '{') {
                str = m_pattern.substr(i + 1, n - i - 1);
                fmt_status = 1;
                fmt_begin = n;
            } else if (fmt_status == 1 && m_pattern[n] == '}') {
                fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                fmt_status = 2;
                n++; // 消费掉 '}'
                break;
            }
            n++;
        }

        if (fmt_status == 0) {
            str = m_pattern.substr(i + 1, n - i - 1);
        } else if (fmt_status == 1) {
            // 解析错误：只有 { 没有 }
            str = "<<error>>";
        }

        // 写入普通文本
        if (!nstr.empty()) {
            vec.emplace_back(nstr, "", 0);
            nstr.clear();
        }
        // 写入格式化项
        vec.emplace_back(str, fmt, 1);

        i = n - 1; // 【关键修复】更新 i，跳过已解析的字符
    }

    if (!nstr.empty()) {
        vec.emplace_back(nstr, "", 0);
    }

    // 2. 将解析出的 vec 转换为具体的 FormatItem (这一步要放在 for 循环外面)
    static std::map<std::string, std::function<FormatItem::ptr(const std::string&)>> s_format_items = {
        #define XX(str, C) {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt)); }}
        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(r, ElapseFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(n, NewLineFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FilenameFormatItem),
        XX(l, LineFormatItem),
        XX(T, TabFormatItem),
        XX(F, FiberIdFormatItem),
        #undef XX
    };

    for (auto& i : vec) {
        if (std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
    }
}


#pragma endregion LogFormatter

#pragma region Logger

    Logger::Logger(std::string name)
        : m_name(std::move(name))
          , m_level(LogLevel::DEBUG) {
        m_formatter = std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
    }

    void Logger::addAppender(const LogAppender::ptr &appender) {
        if (!appender->getFormatter()) {
            appender->setFormatter(m_formatter);
        }
        m_appenders.emplace_back(appender);
    }

    void Logger::delAppender(const LogAppender::ptr &appender) {
        for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }
        }
    }

    void Logger::log(LogLevel::Level level, const LogEvent::ptr &event) {
        if (level >= m_level) {
            // shared_from_this() 与当前对象 共享同一个引用计数控制块
            // 防止自身被多次引用造成的严重错误如 多次delete
            auto self = shared_from_this();
            // MutexType::Lock lock(m_mutex);
            if (!m_appenders.empty()) {
                for (auto &i: m_appenders) {
                    i->log(self, level, event);
                }
            }
            // else if(m_root) {
            //     m_root->log(level, event);
            // }
        }
    }

    void Logger::debug(const LogEvent::ptr &event) {
        log(LogLevel::Level::DEBUG, event);
    }

    void Logger::info(const LogEvent::ptr &event) {
        log(LogLevel::Level::INFO, event);
    }

    void Logger::warn(const LogEvent::ptr &event) {
        log(LogLevel::Level::WARN, event);
    }

    void Logger::error(const LogEvent::ptr &event) {
        log(LogLevel::Level::ERROR, event);
    }

    void Logger::fatal(const LogEvent::ptr &event) {
        log(LogLevel::Level::FATAL, event);
    }

    LogLevel::Level Logger::getLevel() const {
        return m_level;
    }

    void Logger::setLevel(LogLevel::Level level) {
        m_level = level;
    }


#pragma endregion Logger

#pragma region LogAppender
    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        // 为了实现日志过滤
        if (level >= m_level) {
            // TODO: 加锁
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(logger, level, event);
        }
    }

    bool FileLogAppender::reopen() {
        if (m_fileStream) {
            m_fileStream.close();
        }
        m_fileStream.open(m_filename);
        return m_fileStream.is_open();
    }

    LogFormatter::ptr LogAppender::getFormatter() {
        return m_formatter;
    }

    void LogAppender::setFormatter(LogFormatter::ptr formatter) {
        m_formatter = std::move(formatter);
    }

#pragma endregion LogAppender
}

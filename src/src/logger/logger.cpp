#include <cstdarg>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <utility>

#if __cplusplus >= 202002L
#include <chrono>
#include <format>
#endif

// C++17 的filesystem
#if __cplusplus >= 201703
#include <filesystem>

namespace fs = std::filesystem;
#endif
#include <include/logger/logger.h>

namespace RareVoyager
{
#pragma region LogEvent
	LogEvent::LogEvent(const char* file, LogLevel::Level level, int32_t line,
	                   uint32_t elapse, uint32_t threadId, uint32_t fiberId, std::shared_ptr<Logger> logger,
	                   uint32_t time, std::string threadname = "")
		: m_file(file)
		  , m_level(level)
		  , m_line(line)
		  , m_elapse(elapse)
		  , m_threadId(threadId)
		  , m_fiberId(fiberId)
		  , m_logger(std::move(logger))
		  , m_time(time)
		  , m_threadName(std::move(threadname))
	{
	}

	// 把可变参数（...）收集起来，交给真正的实现函数。
	void LogEvent::format(const char* fmt, ...)
	{
		// 用来保存所有 ... 里的参数
		va_list al;
		// 告诉编译器 fmt后面全是 可变参数
		va_start(al,fmt);
		// 具体方法
		format(fmt,al);
		// 结束可变参数访问
		va_end(al);
	}

	void LogEvent::format(const char* fmt, va_list al)
	{
		if (!fmt) return;

#if defined(_WIN32) || defined(_WIN64)
		// 一个va_list 只能被消费一次，因此必须要复制一份
		va_list al_copy;
		va_copy(al_copy, al);
		// 计算最终字符串长度
		int len = _vscprintf(fmt, al_copy);
		va_end(al_copy);

		if (len < 0) return;

		// 申请一块刚刚好的缓冲区
		std::vector<char> buf(static_cast<size_t>(len) + 1);

		// vsnprintf 写入缓冲区（包含 '\0'）
		va_list al_copy2;
		va_copy(al_copy2, al);
		// 所有参数按照fmt规则转成字符，写入到buf
		int written = vsnprintf(buf.data(), buf.size(), fmt, al_copy2);
		va_end(al_copy2);

		// 写入日志缓冲区
		if (written > 0) {
			m_ss.write(buf.data(), written);
		}

#else
		// POSIX/Linux/macOS：推荐同样用 vsnprintf（避免依赖 vasprintf）
		char* buf = nullptr;
		// 获取字符数量
		int len = vasprintf(&buf, fmt, al);
		if(len != -1) {
			m_ss << std::string(buf, len);
			free(buf);
		}
#endif
	}
#pragma endregion LogEvent

#pragma region  FormatItem
	LogFormatter::FormatItem::FormatItem(std::string format)
	{
	}

	class MessageFormatItem : public LogFormatter::FormatItem
	{
	public:
		MessageFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			// 获取文本
			os << event->getContent();
		}
	};

	class LevelFormatItem : public LogFormatter::FormatItem
	{
	public:
		LevelFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			// 获取日志级别
			os << LogLevel::ToString(level);
		}
	};

	class ElapseFormatItem : public LogFormatter::FormatItem
	{
	public:
		ElapseFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			// 获取程序运行时间(ms)
			os << event->getElapse();
		}
	};

	class NameFormatItem : public LogFormatter::FormatItem
	{
	public:
		NameFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << logger->getName();
		}
	};

	class ThreadIdFormatItem : public LogFormatter::FormatItem
	{
	public:
		ThreadIdFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << event->getThreadId();
		}
	};

	class FiberIdFormatItem : public LogFormatter::FormatItem
	{
	public:
		FiberIdFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << event->getFiberId();
		}
	};

	class ThreadNameFormatItem : public LogFormatter::FormatItem
	{
	public:
		ThreadNameFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << event->getThreadName();
		}
	};

	class DateTimeFormatItem : public LogFormatter::FormatItem
	{
	public:
		DateTimeFormatItem(std::string format = "%Y-%m-%d %H:%M:%S")
			: m_format(std::move(format))
		{
			if (m_format.empty())
			{
				m_format = "%Y-%m-%d %H:%M:%S";
			}
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
#if __cplusplus < 202002L
			// C++20 可以使用chrono库来避免这种定义
			time_t t = event->getTime();
			// 从 1970-01-01 00:00:00 UTC 起到现在经过的秒数
			// 本身只是一个描述时间经过的量
			struct tm tm_time;
			// 将时间戳t 转换为具体当前时间,写入到tm结构体中
#if defined(_WIN32)
			// Windows：线程安全版本
			localtime_s(&tm_time, &t);
#else
			// Linux / macOS：POSIX 版本
			localtime_r(&t, &tm_time);
#endif
			// 转换为具体的时间字符串，写入到buf中
			char buf[64] = {0};
			strftime(buf, sizeof(buf), m_format.c_str(), &tm_time);
			os << buf;
#else
			// C++ 20 的写法，更高级
			auto tp = std::chrono::system_clock::from_time_t(event->getTime());
			auto local_tp = std::chrono::zoned_time(std::chrono::current_zone(), tp);
			os << std::format("{:%Y-%m-%d %H:%M:%S}", local_tp);
#endif
		}

	private:
		std::string m_format;
	};

	class FilenameFormatItem : public LogFormatter::FormatItem
	{
	public:
		FilenameFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << event->getFile();
		}
	};

	class LineFormatItem : public LogFormatter::FormatItem
	{
	public:
		LineFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << event->getLine();
		}
	};

	class NewLineFormatItem : public LogFormatter::FormatItem
	{
	public:
		NewLineFormatItem(const std::string& str = "")
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << std::endl;
		}
	};

	// 主要输出 '[' 和 ']'
	class StringFormatItem : public LogFormatter::FormatItem
	{
	public:
		StringFormatItem(const std::string& str)
			: m_string(str)
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << m_string;
		}

	private:
		std::string m_string;
	};

	class TabFormatItem : public LogFormatter::FormatItem
	{
	public:
		TabFormatItem(std::string str = "") : m_string(std::move(str))
		{
		}

		void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
		{
			os << " ";
		}

	private:
		std::string m_string;
	};
#pragma endregion FormatItem

#pragma region  LogLevel
	// TODO: X-Macro 优化
	const char* LogLevel::ToString(LogLevel::Level level)
	{
		switch (level)
		{
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
	}

	LogFormatter::LogFormatter(std::string pattern) : m_pattern(std::move(pattern))
	{
		init();
	}

	std::string LogFormatter::format(const std::shared_ptr<Logger>& logger, LogLevel::Level level,
	                                 const LogEvent::ptr& event)
	{
		std::stringstream ss;
		// 遍历格式化工厂，输出该条日志的完整信息到 stringstream流中
		for (const auto& i: m_items)
		{
			i->format(ss, logger, level, event);
		}
		return ss.str();
	}


#pragma endregion LogLevel

#pragma region LogFormatter
	void LogFormatter::FormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level,
	                                      LogEvent::ptr event)
	{
	}

	// 日志格式解析
	void LogFormatter::init()
	{
		// 1. 所有的元组解析逻辑
		std::vector<std::tuple<std::string, std::string, int> > vec;
		std::string nstr;

		for (size_t i = 0; i < m_pattern.size(); ++i)
		{
			if (m_pattern[i] != '%')
			{
				nstr.append(1, m_pattern[i]);
				continue;// 这里跳过，直到遇到 %
			}

			// 遇到 %%
			if ((i + 1) < m_pattern.size() && m_pattern[i + 1] == '%')
			{
				nstr.append(1, '%');
				++i;// 跳过第二个 %
				continue;
			}

			// 解析 %xxx{xxx} 一般是解析时间格式 %d{%Y-%M-%D}
			size_t n = i + 1;
			int fmt_status = 0;
			size_t fmt_begin = 0;
			std::string str, fmt;

			while (n < m_pattern.size())
			{
				if (!fmt_status && !isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}')
				{
					break;
				}
				if (fmt_status == 0 && m_pattern[n] == '{')
				{
					str = m_pattern.substr(i + 1, n - i - 1);
					fmt_status = 1;
					fmt_begin = n;
				}
				else if (fmt_status == 1 && m_pattern[n] == '}')
				{
					fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
					fmt_status = 2;
					n++;// 消费掉 '}'
					break;
				}
				n++;
			}

			if (fmt_status == 0)
			{
				str = m_pattern.substr(i + 1, n - i - 1);
			}
			else if (fmt_status == 1)
			{
				// 解析错误：只有 '{' 没有 '}'
				str = "<<error>>";
			}

			// 写入普通文本
			if (!nstr.empty())
			{
				vec.emplace_back(nstr, "", 0);
				nstr.clear();
			}
			// 写入格式化项
			vec.emplace_back(str, fmt, 1);

			i = n - 1;// 【关键修复】更新 i，跳过已解析的字符
		}

		if (!nstr.empty())
		{
			vec.emplace_back(nstr, "", 0);
		}

		// 2. 将解析出的 vec 转换为具体的 FormatItem (这一步要放在 for 循环外面)
		static std::map<std::string, std::function<FormatItem::ptr(const std::string&)> > s_format_items = {
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
				XX(N, ThreadNameFormatItem),
#undef XX
		};

		// for (auto& [value,value2,a] : vec)
		// {
		// 	std::cout <<value << " " << value2 << " " << a << std::endl;
		// }
		// std::cout << std::endl;

		// TODO:
		for (auto& i: vec)
		{
			if (std::get<2>(i) == 0)
			{
				m_items.push_back(std::make_shared<StringFormatItem>(std::get<0>(i)));
			}
			else
			{
				auto it = s_format_items.find(std::get<0>(i));
				if (it == s_format_items.end())
				{
					m_items.push_back(
							FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
				}
				else
				{
					m_items.push_back(it->second(std::get<1>(i)));
				}
			}
		}
	}


#pragma endregion LogFormatter

#pragma region Logger

	Logger::Logger(std::string name)
		: m_name(std::move(name))
		  , m_level(LogLevel::DEBUG)
	{
		m_formatter = std::make_shared<LogFormatter>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%n%m%n");
	}

	void Logger::addAppender(const LogAppender::ptr& appender)
	{
		if (!appender->getFormatter())
		{
			appender->setFormatter(m_formatter);
		}
		m_appenders.emplace_back(appender);
	}

	void Logger::delAppender(const LogAppender::ptr& appender)
	{
		for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
		{
			if (*it == appender)
			{
				m_appenders.erase(it);
				break;
			}
		}
	}

	void Logger::log(LogLevel::Level level, const LogEvent::ptr& event)
	{
		// 过滤低级日志
		if (level >= m_level)
		{
			// shared_from_this() 与当前对象 共享同一个引用计数控制块
			// 防止自身被多次引用造成的严重错误如 多次delete
			auto self = shared_from_this();
			// MutexType::Lock lock(m_mutex);
			if (!m_appenders.empty())
			{
				for (auto& i: m_appenders)
				{
					i->log(self, level, event);
				}
			}
			// else if(m_root) {
			//     m_root->log(level, event);
			// }
		}
	}

	void Logger::debug(const LogEvent::ptr& event)
	{
		log(LogLevel::Level::DEBUG, event);
	}

	void Logger::info(const LogEvent::ptr& event)
	{
		log(LogLevel::Level::INFO, event);
	}

	void Logger::warn(const LogEvent::ptr& event)
	{
		log(LogLevel::Level::WARN, event);
	}

	void Logger::error(const LogEvent::ptr& event)
	{
		log(LogLevel::Level::ERROR, event);
	}

	void Logger::fatal(const LogEvent::ptr& event)
	{
		log(LogLevel::Level::FATAL, event);
	}

	LogLevel::Level Logger::getLevel() const
	{
		return m_level;
	}

	void Logger::setLevel(LogLevel::Level level)
	{
		m_level = level;
	}


#pragma endregion Logger

#pragma region LogAppender
	void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
	{
		// 为了实现日志过滤
		if (level >= m_level)
		{
			// TODO: 加锁
			// 循环遍历m_items 输出完整日志信息
			std::cout << m_formatter->format(logger, level, event);
		}
	}

	FileLogAppender::FileLogAppender(LogLevel::Level level,const std::string& filename="")
	{
		m_level = level;
		m_filename = filename + LogLevel::ToString(level);
		reopen();
	}

	void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
	{
		if (level < m_level) return;

		const std::string text = m_formatter->format(logger, level, event);

		std::lock_guard<std::mutex> lock(m_mutex);
		m_fileStream << text;
	}

	bool FileLogAppender::reopen()
	{
		if (m_fileStream)
		{
			m_fileStream.close();
		}
		m_fileStream.open(m_filename,std::ios::app);
		return m_fileStream.is_open();
	}

	LogFormatter::ptr LogAppender::getFormatter()
	{
		return m_formatter;
	}

	void LogAppender::setFormatter(LogFormatter::ptr formatter)
	{
		m_formatter = std::move(formatter);
	}

#pragma endregion LogAppender

#pragma region LogEventWarp
	LogEventWarp::LogEventWarp(LogEvent::ptr e):m_event(std::move(e)){}

	LogEventWarp::~LogEventWarp()
	{
		m_event->getLogger()->log(m_event->getLevel(),m_event);
	}

#pragma endregion LogEventWarp

#pragma region LogManager
	LogManager::LogManager()
	{
		m_root.reset(new Logger);
		m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
		// m_loggers[m_root->m_name] = m_root;
		init();
	}

	Logger::ptr LogManager::getLogger(const std::string& name)
	{
		auto it = m_loggers.find(name);
		if (it != m_loggers.end())
		{
			return it->second;
		}
		// 如果没有这个Logger 那么添加一个
		Logger::ptr logger(new Logger(name));
		m_loggers[name] = logger;
		return logger;
	}

	void LogManager::init()
	{
	}

#pragma endregion LogManager
}
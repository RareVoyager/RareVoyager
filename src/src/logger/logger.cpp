#include <cstdarg>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <utility>

#include <yaml-cpp/yaml.h>
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
#include <include/config/config.h>


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
	/**
	 * @brief: 实现了一个类似printf的打印方法
	 * @param fmt
	 * @param ...
	 */
	void LogEvent::format(const char* fmt, ...)
	{
		// 用来保存所有 ... 里的参数
		va_list al;
		// 告诉编译器 fmt后面全是 可变参数
		va_start(al, fmt);
		// 具体方法
		format(fmt, al);
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
		if (written > 0)
		{
			m_ss.write(buf.data(), written);
		}

#else
		// POSIX/Linux/macOS：推荐同样用 vsnprintf（避免依赖 vasprintf）
		char* buf = nullptr;
		// 获取字符数量
		int len = vasprintf(&buf, fmt, al);
		if (len != -1)
		{
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
			time_t t = event->getTime();
			tm tm_time;
#if defined(_WIN32) // windows 线程安全版本
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

	LogLevel::Level LogLevel::FromString(const std::string& str)
	{
#define XX(name) \
		if (str ==  #name) {\
			return LogLevel::name;\
	}
		XX(DEBUG);
		XX(INFO);
		XX(WARN);
		XX(ERROR);
		XX(FATAL);
#undef XX
		return LogLevel::UNKNOW;
	}
#pragma endregion LogLevel

#pragma region LogFormatter
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

	void LogFormatter::setFormatter(const std::string& val)
	{
		m_pattern = val;
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
				m_error = true;
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

	LogFormatter::ptr LogAppender::getFormatter()
	{
		Mutex::Lock lock(&m_mutex);
		return m_formatter;
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
		Mutex::Lock lock(&m_mutex);
		if (!appender->getFormatter())
		{
			Mutex::Lock ll(&appender->m_mutex);
			appender->setFormatter(m_formatter);
		}
		m_appenders.emplace_back(appender);
	}

	void Logger::delAppender(const LogAppender::ptr& appender)
	{
		Mutex::Lock lock(&m_mutex);
		for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
		{
			if (*it == appender)
			{
				m_appenders.erase(it);
				break;
			}
		}
	}

	void Logger::clearAppenders()
	{
		Mutex::Lock lock(&m_mutex);
		m_appenders.clear();
	}

	void Logger::setFormatter(LogFormatter::ptr val)
	{
		// MutexType::Lock lock(m_mutex);
		m_formatter = val;

		for (auto& i: m_appenders)
		{
			// MutexType::Lock ll(i->m_mutex);
			if (!i->m_hasFormatter)
			{
				i->m_formatter = m_formatter;
			}
		}
	}

	void Logger::setFormatter(const std::string& val)
	{
		Mutex::Lock lock(&m_mutex);
		LogFormatter::ptr new_val(new LogFormatter(val));
		if (new_val->isError())
		{
			std::cout << "Logger setFormatter name=" << m_name
					<< " value=" << val << " invalid formatter"
					<< std::endl;
			return;
		}
		//m_formatter = new_val;
		setFormatter(new_val);
	}

	LogFormatter::ptr Logger::getFormatter()
	{
		return m_formatter;
	}

	std::string Logger::toYamlString() {
		Mutex::Lock lock(&m_mutex);
		YAML::Node node;
		node["name"] = m_name;
		if(m_level != LogLevel::UNKNOW) {
			node["level"] = LogLevel::ToString(m_level);
		}
		if(m_formatter) {
			node["formatter"] = m_formatter->getPattern();
		}

		for(auto& i : m_appenders) {
			node["appenders"].push_back(YAML::Load(i->toYamlString()));
		}
		std::stringstream ss;
		ss << node;
		return ss.str();
	}


	void Logger::log(LogLevel::Level level, const LogEvent::ptr& event)
	{
		// 过滤低级日志
		if (level >= m_level)
		{
			// shared_from_this() 与当前对象 共享同一个引用计数控制块
			// 防止自身被多次引用造成的严重错误如 多次delete
			auto self = shared_from_this();
			Mutex::Lock lock(&m_mutex);
			if (!m_appenders.empty())
			{
				for (auto& i: m_appenders)
				{
					i->log(self, level, event);
				}
			}
			else if (m_root)
			{
				m_root->log(level, event);
			}
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
			Mutex::Lock lock(&m_mutex);
			// 循环遍历m_items 输出完整日志信息
			std::cout << m_formatter->format(logger, level, event);
		}
	}

	std::string StdoutLogAppender::toYamlString() {
		// MutexType::Lock lock(m_mutex);
		YAML::Node node;
		node["type"] = "StdoutLogAppender";
		if(m_level != LogLevel::UNKNOW) {
			node["level"] = LogLevel::ToString(m_level);
		}
		if(m_hasFormatter && m_formatter) {
			node["formatter"] = m_formatter->getPattern();
		}
		std::stringstream ss;
		ss << node;
		return ss.str();
	}

	FileLogAppender::FileLogAppender(const std::string& filename)
		: m_filename(filename)
	{
		reopen();
	}

	void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
	{
		if (level >= m_level)
		{
			uint64_t now = time(0);
			if (now != m_lastTime)
			{
				reopen();
				m_lastTime = now;
			}
			Mutex::Lock lock(&m_mutex);
			m_fileStream << m_formatter->format(logger, level, event);
		}

	}

	std::string FileLogAppender::toYamlString() {
		Mutex::Lock lock(&m_mutex);
		YAML::Node node;
		node["type"] = "FileLogAppender";
		node["file"] = m_filename;
		if(m_level != LogLevel::UNKNOW) {
			node["level"] = LogLevel::ToString(m_level);
		}
		if(m_hasFormatter && m_formatter) {
			node["formatter"] = m_formatter->getPattern();
		}
		std::stringstream ss;
		ss << node;
		return ss.str();
	}

	bool FileLogAppender::reopen()
	{
		Mutex::Lock lock(&m_mutex);
		if (m_fileStream)
		{
			m_fileStream.close();
		}
		m_fileStream.open(m_filename, std::ios::app);
		return m_fileStream.is_open();
	}

	void LogAppender::setFormatter(LogFormatter::ptr formatter)
	{
		Mutex::Lock lock(&m_mutex);
		m_formatter = std::move(formatter);
		if (m_formatter)
		{
			m_hasFormatter = true;
		}
		else
		{
			m_hasFormatter = false;
		}
	}

	void LogAppender::setLevel(LogLevel::Level level)
	{
		m_level = level;
	}

#pragma endregion LogAppender

#pragma region LogEventWarp
	LogEventWarp::LogEventWarp(LogEvent::ptr e) : m_event(std::move(e))
	{
	}

	LogEventWarp::~LogEventWarp()
	{
		m_event->getLogger()->log(m_event->getLevel(), m_event);
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
		Mutex::Lock lock(&m_mutex);
		auto it = m_loggers.find(name);
		// 找到了对应的logger
		if (it != m_loggers.end())
		{
			return it->second;
		}
		// 如果没有这个Logger 那么添加一个
		Logger::ptr logger(new Logger(name));
		m_loggers[name] = logger;
		return logger;
	}

	std::string LogManager::toYamlString()
	{
		Mutex::Lock lock(&m_mutex);
		YAML::Node node;
		for (auto& i: m_loggers)
		{
			node.push_back(YAML::Load(i.second->toYamlString()));
		}
		std::stringstream ss;
		ss << node;
		return ss.str();
	}

	void LogManager::init()
	{
	}

#pragma endregion LogManager

#pragma region LogDefine

	struct LogAppenderDefine
	{
		int type = 0;//1 File, 2 Stdout
		LogLevel::Level level = LogLevel::UNKNOW;
		std::string formatter;
		std::string file;

		bool operator==(const LogAppenderDefine& oth) const
		{
			return type == oth.type
			       && level == oth.level
			       && formatter == oth.formatter
			       && file == oth.file;
		}
	};

	// 配置文件使用的日志相关信息结构体
	struct LogDefine
	{
		// 日志文件名
		std::string name;
		// 日志等级
		LogLevel::Level level = LogLevel::UNKNOW;
		// 日志格式
		std::string formatter;
		// 日志输出器
		std::vector<LogAppenderDefine> appenders;

		// 重载了 == 运算符
		bool operator==(const LogDefine& oth) const
		{
			return name == oth.name
			       && level == oth.level
			       && formatter == oth.formatter
			       && appenders == appenders;
		}

		// 重载了 < 运算符，用在set比较
		bool operator<(const LogDefine& oth) const
		{
			return name < oth.name;
		}

		// 是否有效
		bool isValid() const
		{
			return !name.empty();
		}
	};

	// 偏特化
	template<>
	class LexicalCast<std::string, LogDefine>
	{
	public:
		LogDefine operator()(const std::string& v)
		{
			YAML::Node n = YAML::Load(v);
			LogDefine ld;
			if (!n["name"].IsDefined())
			{
				std::cout << "log config error: name is null, " << n
						<< std::endl;
				throw std::logic_error("log config name is null");
			}
			ld.name = n["name"].as<std::string>();
			ld.level = LogLevel::FromString(n["level"].IsDefined() ? n["level"].as<std::string>() : "");
			if (n["formatter"].IsDefined())
			{
				ld.formatter = n["formatter"].as<std::string>();
			}

			if (n["appenders"].IsDefined())
			{
				//std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
				for (size_t x = 0; x < n["appenders"].size(); ++x)
				{
					auto a = n["appenders"][x];
					if (!a["type"].IsDefined())
					{
						std::cout << "log config error: appender type is null, " << a
								<< std::endl;
						continue;
					}
					std::string type = a["type"].as<std::string>();
					LogAppenderDefine lad;
					if (type == "FileLogAppender")
					{
						lad.type = 1;
						if (!a["file"].IsDefined())
						{
							std::cout << "log config error: fileappender file is null, " << a
									<< std::endl;
							continue;
						}
						lad.file = a["file"].as<std::string>();
						if (a["formatter"].IsDefined())
						{
							lad.formatter = a["formatter"].as<std::string>();
						}
					}
					else if (type == "StdoutLogAppender")
					{
						lad.type = 2;
						if (a["formatter"].IsDefined())
						{
							lad.formatter = a["formatter"].as<std::string>();
						}
					}
					else
					{
						std::cout << "log config error: appender type is invalid, " << a
								<< std::endl;
						continue;
					}

					ld.appenders.push_back(lad);
				}
			}
			return ld;
		}
	};

	template<>
	class LexicalCast<LogDefine, std::string>
	{
	public:
		std::string operator()(const LogDefine& i)
		{
			YAML::Node n;
			n["name"] = i.name;
			if (i.level != LogLevel::UNKNOW)
			{
				n["level"] = LogLevel::ToString(i.level);
			}
			if (!i.formatter.empty())
			{
				n["formatter"] = i.formatter;
			}

			for (auto& a: i.appenders)
			{
				YAML::Node na;
				if (a.type == 1)
				{
					na["type"] = "FileLogAppender";
					na["file"] = a.file;
				}
				else if (a.type == 2)
				{
					na["type"] = "StdoutLogAppender";
				}
				if (a.level != LogLevel::UNKNOW)
				{
					na["level"] = LogLevel::ToString(a.level);
				}

				if (!a.formatter.empty())
				{
					na["formatter"] = a.formatter;
				}

				n["appenders"].push_back(na);
			}
			std::stringstream ss;
			ss << n;
			return ss.str();
		}
	};

	auto g_log_defines = RareVoyager::Config::Lookup("logs", std::set<LogDefine>(), "logs config");

	/**
	 * @brief :给“日志配置项”注册一个监听器：只要配置发生变化，就自动创建/更新/删除 Logger，
	 * 并按新配置重建它的 Appender（输出目标）、等级、格式器
	 */
	struct LogIniter
	{
		LogIniter()
		{
			/**
			 * @param: old_value 旧的日志配置集合
			 * @param: new_value 新的日志配置集合
			 */
			g_log_defines->addListener(	[](const std::set<LogDefine>& old_value,
			                                        const std::set<LogDefine>& new_value) {
				RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "on_logger_conf_changed";

				// 处理新增 和 修改
				for (auto& i: new_value)
				{
					// 从旧配置里面找新的配置，如果没有则新增。如果有并且不
					auto it = old_value.find(i);
					Logger::ptr logger;
					// 处理新增和修改
					if (it == old_value.end() || !(*it == i))
					{
						logger = RAREVOYAGER_LOG_NAME(i.name);
					}
					logger->setLevel(i.level);

					// 如果输出格式为空
					if (!i.formatter.empty())
					{
						logger->setFormatter(i.formatter);
					}

					logger->clearAppenders();
					// type == 1 是文件
					// type == 2 是控制台
					for (auto& a: i.appenders)
					{
						LogAppender::ptr ap;
						if (a.type == 1)
						{
							ap.reset(new FileLogAppender(a.file));
						}
						else if (a.type == 2)
						{
							ap.reset(new StdoutLogAppender);
						}
						ap->setLevel(a.level);
						if (!a.formatter.empty())
						{
							LogFormatter::ptr fmt(new LogFormatter(a.formatter));
							if (!fmt->isError())
							{
								ap->setFormatter(fmt);
							}
							else
							{
								std::cout << "log.name=" << i.name << " appender type=" << a.type
										<< " formatter=" << a.formatter << " is invalid" << std::endl;
							}
						}
						logger->addAppender(ap);
					}
				}

				for (auto& i: old_value)
				{
					auto it = new_value.find(i);
					// 如果在新的LogDefine 集合里面没有找到，就删除
					if (it == new_value.end())
					{
						//删除logger
						auto logger = RAREVOYAGER_LOG_NAME(i.name);
						logger->setLevel((LogLevel::Level)0);
						logger->clearAppenders();
					}
				}
			});
		}
	};

	static LogIniter __log_init;
#pragma endregion LogDefine
}
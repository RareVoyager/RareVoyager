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
#include <map>
#include <mutex>


#include <include/singleton.h>
#define RUNKONW RareVoyager::LogLevel::Level::UNKNOW
#define RDEBUG RareVoyager::LogLevel::Level::DEBUG
#define RINFO RareVoyager::LogLevel::Level::INFO
#define RWARN RareVoyager::LogLevel::Level::DWARN
#define RERROR RareVoyager::LogLevel::Level::ERROR
#define RFATAL RareVoyager::LogLevel::Level::FATAL

#define RAREVOYAGER_LOG_LEVEL(logger, level) \
if(logger->getLevel() <= level) \
RareVoyager::LogEventWarp(RareVoyager::LogEvent::ptr(new RareVoyager::LogEvent( \
__FILE__, \
level, \
__LINE__,\
0,\
RareVoyager::getThreadPid(), \
RareVoyager::getFiberId(), \
logger, \
(uint32_t)time(0), \
std::string("xxx") \
))).getSS()


#define RAREVOYAGER_LOG_DEBUG(logger) RAREVOYAGER_LOG_LEVEL(logger, RareVoyager::LogLevel::DEBUG)
#define RAREVOYAGER_LOG_INFO(logger) RAREVOYAGER_LOG_LEVEL(logger, RareVoyager::LogLevel::INFO)
#define RAREVOYAGER_LOG_WARN(logger) RAREVOYAGER_LOG_LEVEL(logger, RareVoyager::LogLevel::WARN)
#define RAREVOYAGER_LOG_ERROR(logger) RAREVOYAGER_LOG_LEVEL(logger, RareVoyager::LogLevel::ERROR)
#define RAREVOYAGER_LOG_FATAL(logger) RAREVOYAGER_LOG_LEVEL(logger, RareVoyager::LogLevel::FATAL)

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 * 使用起来类似与C语言的printf("",...);
 */
#define RAREVOYAGER_LOG_FMT_LEVEL(logger, level, fmt, ...) \
if(logger->getLevel() <= level) \
RareVoyager::LogEventWarp(RareVoyager::LogEvent::ptr(new RareVoyager::LogEvent( \
__FILE__, \
level, \
__LINE__,\
0,\
RareVoyager::getThreadPid(), \
RareVoyager::getFiberId(), \
logger, \
(uint32_t)time(0), \
std::string("xxx") \
))).getEvent()->format(fmt, __VA_ARGS__)

#define RAREVOYAGER_LOG_FMT_DEBUG(logger, fmt, ...) RAREVOYAGER_LOG_FMT_LEVEL(logger, RareVoyager::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define RAREVOYAGER_LOG_FMT_INFO(logger, fmt, ...)  RAREVOYAGER_LOG_FMT_LEVEL(logger, RareVoyager::LogLevel::INFO, fmt, __VA_ARGS__)
#define RAREVOYAGER_LOG_FMT_WARN(logger, fmt, ...)  RAREVOYAGER_LOG_FMT_LEVEL(logger, RareVoyager::LogLevel::WARN, fmt, __VA_ARGS__)
#define RAREVOYAGER_LOG_FMT_ERROR(logger, fmt, ...) RAREVOYAGER_LOG_FMT_LEVEL(logger, RareVoyager::LogLevel::ERROR, fmt, __VA_ARGS__)
#define RAREVOYAGER_LOG_FMT_FATAL(logger, fmt, ...) RAREVOYAGER_LOG_FMT_LEVEL(logger, RareVoyager::LogLevel::FATAL, fmt, __VA_ARGS__)


namespace RareVoyager
{

	class Logger;
#pragma region LogLevel
	/**
	 * @brief: 日志等级
	 */
	class LogLevel
	{
	public:
		enum Level
		{
			UNKONW = 0,// 未知等级
			DEBUG = 1,// Debug级别日志
			INFO = 2,// Info级别日志
			WARN = 3,// Warn级别日志
			ERROR = 4,// error级别日志
			FATAL = 5// 严重错误级别日志
		};

		/**
		 * @brief: 将日志等级转换为普通字符串(char*)
		 * @param level
		 * @return
		 */
		const static char* ToString(Level level);
	};
#pragma endregion LogLevel

#pragma region LogEvent
	/**
	 * @brief: 一条完整的日志记录
	 */
	class LogEvent
	{
	public:
		typedef std::shared_ptr<LogEvent> ptr;

		/**
		 * @brief: 构造函数。
		 * @param file 日志输出的文件名
		 * @param line 日志输出的行数
		 * @param elapse 程序运行开始到日志输出的时间(ms)
		 * @param threadId 线程id
		 * @param fiberId 协程id
		 * @param time 时间
		 * @param threadname 线程名称
		 */
		LogEvent(const char* file,LogLevel::Level level, int32_t line, uint32_t elapse
		         , uint32_t threadId, uint32_t fiberId,std::shared_ptr<Logger> logger,
		         uint32_t time, std::string threadname);

		// 下面是一些FormatterItem 会用到的方法。访问级别为public
		const char* getFile() { return m_file; }
		[[nodiscard]] int32_t getLine() const { return m_line; }
		[[nodiscard]] uint32_t getElapse() const { return m_elapse; }
		[[nodiscard]] uint32_t getThreadId() const { return m_threadId; }
		[[nodiscard]] uint32_t getFiberId() const { return m_fiberId; }
		[[nodiscard]] uint64_t getTime() const { return m_time; }
		[[nodiscard]] std::string getContent() const { return m_ss.str(); }
		[[nodiscard]]  std::stringstream& getSS() { return m_ss; }
		[[nodiscard]] const std::string& getThreadName() const { return m_threadName; }
		[[nodiscard]] std::shared_ptr<Logger> getLogger() const { return m_logger; }
		[[nodiscard]] LogLevel::Level getLevel() const { return m_level; }

		/**
	 * @brief 格式化写入日志内容
	 */
		void format(const char* fmt, ...);

		/**
		 * @brief 格式化写入日志内容
		 */
		void format(const char* fmt, va_list al);

	private:
		const char* m_file = nullptr;// 文件名
		int32_t m_line = 0;// 行号
		uint32_t m_elapse = 0;// 程序启动到现在的毫秒数
		uint32_t m_threadId = 0;// 线程id
		uint32_t m_fiberId = 0;// 协程id(一个线程包含多个协程)
		uint64_t m_time;// 时间戳
		std::stringstream m_ss;// 输出文本
		std::string m_threadName;//线程名称 %N
		LogLevel::Level m_level;
		std::shared_ptr<Logger> m_logger;
	};
#pragma endregion LogEvent

#pragma region LogEventWarp 日志信息包装器
	class LogEventWarp
	{
	public:
		LogEventWarp(LogEvent::ptr e);
		~LogEventWarp();

		[[nodiscard]]  std::stringstream& getSS() {return m_event->getSS();}
		LogEvent::ptr getEvent() const { return m_event;}
	private:
		LogEvent::ptr m_event;

	};
#pragma endregion LogEvent



#pragma region LogFormatter
	/**
	 * @brief: 日志最终输出形式 json 或 一串字符串
	 * 此处为抽象基类，主要是定义了方法名称以及参数。
	 * 后面实现了输出到控制台的日志以及写入文件的日志
	 */
	class LogFormatter
	{
	public:
		typedef std::shared_ptr<LogFormatter> ptr;

		/**
		 * @brief: 日志格式化
		 * @param pattern 默认构造 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
		 */
		LogFormatter(std::string pattern);

		/**
		 * @brief 返回格式化日志文本
		 * @param[in] logger 日志器
		 * @param[in] level 日志级别
		 * @param[in] event 日志事件
		 */
		std::string format(const std::shared_ptr<Logger>& logger, LogLevel::Level level, const LogEvent::ptr& event);

	public:
		/**
		 * @brief: 输出信息基类。对应着LogEvent里面的私有字段
		 */
		class FormatItem
		{
		public:
			typedef std::shared_ptr<FormatItem> ptr;

			FormatItem(std::string format = "");

			virtual ~FormatItem() = default;

			/**
			 * @brief 格式化日志到流 主要是os流接受所有相关信息，最后输出
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
		// TODO: 优化解析流程。原解析流程繁琐复杂。
		void init();

	private:
		/// 日志格式模板
		std::string m_pattern;
		/// 日志格式解析后格式
		/// TODO: 没有搞明白init最后是如何写入这些信息的。
		std::vector<FormatItem::ptr> m_items;
	};
#pragma endregion LogFormatter

#pragma region LogAppender
	/**
	 * @brief: 日志最终输出位置，例如控制台或文件
	 * 这里也是一个抽象基类
	 */
	class LogAppender
	{
	public:
		typedef std::shared_ptr<LogAppender> ptr;

		virtual ~LogAppender() = default;

		virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

		/**
		 * @brief: 获取解器
		 * @return
		 */
		virtual LogFormatter::ptr getFormatter();

		/**
		 * @brief: 设置解析器。(一定会走 LogFormatter 的init() )
		 * @param formatter
		 */
		virtual void setFormatter(LogFormatter::ptr formatter);

	protected:
		// 日志等级
		LogLevel::Level m_level = LogLevel::DEBUG;
		// 格式化日志工具
		LogFormatter::ptr m_formatter;
	};
#pragma endregion LogAppender

#pragma region Logger
	/**
	 * @brief: 用户使用
	 * 继承自 std::enable_shared_from_this<Logger> 用来让对象在成员函数内部，获得指向自身的指针
	 */
	class Logger : public std::enable_shared_from_this<Logger>
	{
	public:
		typedef std::shared_ptr<Logger> ptr;

		Logger(std::string name = "root");

		void log(LogLevel::Level level, const LogEvent::ptr& event);

		void debug(const LogEvent::ptr& event);

		void info(const LogEvent::ptr& event);

		void warn(const LogEvent::ptr& event);

		void error(const LogEvent::ptr& event);

		void fatal(const LogEvent::ptr& event);

		// 返回值不能被忽略
		[[nodiscard]] LogLevel::Level getLevel() const;

		void setLevel(LogLevel::Level level);

		void addAppender(const LogAppender::ptr& appender);

		void delAppender(const LogAppender::ptr& appender);

		[[nodiscard]] std::string getName() const { return m_name; }

	public:
		std::string m_name;// 日志名称
		LogLevel::Level m_level;// 日志级别
		std::list<LogAppender::ptr> m_appenders;// Appender集合
		LogFormatter::ptr m_formatter;
	};
#pragma endregion Logger

#pragma region StdoutLogAppender
	// 输出到控制台的Appender
	class StdoutLogAppender : public LogAppender
	{
	public:
		typedef std::shared_ptr<StdoutLogAppender> ptr;

		void log(std::shared_ptr<Logger> ptr, LogLevel::Level level, LogEvent::ptr event) override;
	};
#pragma endregion StdoutLogAppender

#pragma region FileLogAppender
	// 输出到文件的Appender
	class FileLogAppender : public LogAppender
	{
	public:
		typedef std::shared_ptr<FileLogAppender> ptr;

		FileLogAppender(LogLevel::Level level,const std::string& filename);

		void log(std::shared_ptr<Logger> ptr, LogLevel::Level level, LogEvent::ptr event) override;

		/**
		 * TODO: 未来给出C++ 17及以上的特化版本
		 * @return
		 */
		bool reopen();

	private:
		std::mutex m_mutex;
		std::string m_filename;// 文件名
		std::ofstream m_fileStream;// 文件写入流
	};
#pragma endregion FileLogAppender

#pragma region LogManager
	class LogManager
	{
		public:
		LogManager();
		Logger::ptr getLogger(const std::string& name);
	private:
		void init();
	private:
		std::map<std::string,Logger::ptr> m_loggers;
		Logger::ptr m_root;
	};

	typedef Singleton<LogManager> LoggerMgr;
#pragma endregion LogManager
}


#endif //RAREVOYAGER_LOGGER_H
#include "include/config/config.h"

#include <include/logger/logger.h>
#include <include/util.h>
#include <iostream>


void test_custom_format()
{
	// 1. 获取日志器
	auto logger = RAREVOYAGER_LOG_ROOT();
	logger->addAppender(std::make_shared<RareVoyager::FileLogAppender>("log.log"));

	// 2. 定义格式模板
	// %d: 时间
	// %t: 线程ID
	// %p: 级别
	// %f: 文件名
	// %l: 行号
	// %m: 消息
	// %n: 换行
	std::string pattern = "[%d{%H:%M:%S}] [%t] [%p] %f:%l - %m%n";

	// 3. 应用格式
	logger->setFormatter(pattern);

	// 4. 测试输出
	RAREVOYAGER_LOG_INFO(logger) << "This is a custom format log";

	pattern = "[%d{%Y-%M-%D %H:%M:%S}] [%t] [%p] %f:%l - %m%n";
	logger->setFormatter(pattern);
	RAREVOYAGER_LOG_INFO(logger) << "print info";
}

int main()
{
	auto logger = RAREVOYAGER_LOG_ROOT();
	// 添加一个控制台级别日志
	logger->addAppender(std::make_shared<RareVoyager::StdoutLogAppender>());

	//logger->addAppender(std::make_shared<RareVoyager::FileLogAppender>(RDEBUG,RareVoyager::GetCurrentDateStr()));

	// 构造一条完整日志LogEent, 主要包含以下信息
	// auto event = std::make_shared<RareVoyager::LogEvent>(
	// 		__FILE__, __LINE__, 0, std::hash<std::thread::id>{}(std::this_thread::get_id()), 2, (uint32_t)time(0),std::string("xx")
	// 		);

	// 给定日志级别 和 完整给日志记录
	// 打印日志
	// logger->log(RareVoyager::LogLevel::WARN, event);

	// 宏最终拿到了一个 stringstream 对象
	RAREVOYAGER_LOG_DEBUG(logger) << "Error message:";
	RAREVOYAGER_LOG_INFO(logger) << "Info message:" << "hello " << " = " << "world";
	RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "aaa";

	// --- 场景 2：带整数参数 ---
	int id = 1001;
	// 输出: ... user_id: 1001 process ok
	RAREVOYAGER_LOG_FMT_DEBUG(logger, "user_id: %d process ok", id);

	// --- 场景 3：混合类型 ---
	const char* name = "RareVoyager";
	float progress = 55.5f;
	// 输出: ... Project: RareVoyager, Progress: 55.50%
	RAREVOYAGER_LOG_FMT_DEBUG(logger, "Project: %s, Progress: %.2f%%", name, progress);

	RareVoyager::ConfigVar<int> config_var("1145", 265);
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << config_var.getName();

	test_custom_format();
	return 0;
}

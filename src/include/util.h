/*************************************************
 * 描述：
 *
 * File：util.h
 * Author：Cipher
 * Date：2026/1/2-14:59
 * Update：
 * ************************************************/

#ifndef RAREVOYAGER_UTIL_H
#define RAREVOYAGER_UTIL_H
#include <cstdint>
#include <string>
#include <vector>
#include <sys/types.h>


namespace RareVoyager
{
	// 获取线程Id
	pid_t getThreadPid();

	// 获取协程Id
	uint32_t getFiberId();

	// 获取当前时间字符串
	std::string GetCurrentDateStr();

	// 断言信息assert
	void Backtrace(std::vector<std::string>& bt,int size ,int skip = 1);

	std::string BacktraceToString(int size,int skip =2,const std::string& prefix = "");
}

#endif //RAREVOYAGER_UTIL_H
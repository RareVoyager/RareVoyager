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
#include <sys/types.h>


namespace RareVoyager
{
	// 获取线程Id
	pid_t getThreadPid();

	// 获取协程Id
	uint32_t getFiberId();

	// 获取当前时间字符串
	std::string GetCurrentDateStr();
}

#endif //RAREVOYAGER_UTIL_H
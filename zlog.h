/*
 * zlog.h
 *
 *  Created on: 2014年9月30日
 *      Author: Fifi Lyu
 *        Desc: 无任何非系统库依赖，功能简单的日志系统
 */

#pragma once

// Platform recognition
#if defined(WIN32) || defined(_WIN32)
#define PLATFORM_WIN32 1

#elif defined(__unix__) || defined(unix)
#define PLATFORM_LINUX 1

#else
#error Unknown platform

#endif

#include <string>
using namespace std;

namespace zlog
{

	typedef enum
	{
		LOGOUTPUTSTREAM_FILE,
		LOGOUTPUTSTREAM_STDOUT,
		LOGOUTPUTSTREAM_STDERR
	} LogOutputStream_t;

	typedef enum
	{
		LOG_TRACE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR,
		LOG_FATAL,
		LOG_OFF
	} LogLevel_t;

	typedef struct
	{
		LogLevel_t level;
		LogOutputStream_t output_to;
		size_t max_byte;
		string log_dir;
		bool use_file_output;
		string cur_prog_name;
	} LogConfig_t;

	extern LogConfig_t log_config;

#define trace_print(...) print_log(LOG_TRACE, __VA_ARGS__)
#define debug_print(...) print_log(LOG_DEBUG, __VA_ARGS__)
#define info_print(...) print_log(LOG_INFO, __VA_ARGS__)
#define warn_print(...) print_log(LOG_WARN, __VA_ARGS__)
#define error_print(...) print_log(LOG_ERROR, __VA_ARGS__)
#define fatal_print(...) print_log(LOG_FATAL, __VA_ARGS__)

#define print_log(level, ...) _print_log(level, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

	void _print_log(LogLevel_t level, const char *file, const char *fn, int line, const char *fmt, ...);

	void close_log_stream();

} /* namespace zlog */

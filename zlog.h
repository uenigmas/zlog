/*
 * zlog.h
 *
 *  Created on: 2014年9月30日
 *      Author: Fifi Lyu
 *        Desc: 无任何非系统库依赖，功能简单的日志系统
 */

#ifndef INCLUDE_ZPSOE_ZLOG_H_
#define INCLUDE_ZPSOE_ZLOG_H_

#ifndef INCLUDE_ZPSOE_CONFIG_PLATFORM_H
#define INCLUDE_ZPSOE_CONFIG_PLATFORM_H

 // Platform recognition
#if defined(WIN32) || defined(_WIN32)

#define PLATFORM_WIN32 1

#elif defined(__unix__) || defined(unix)

#define PLATFORM_LINUX 1

#else

#error Unknown platform

#endif

#endif // INCLUDE_ZPSOE_CONFIG_PLATFORM_H

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

	#define trace_print(fmt, ...) print_log(LOG_TRACE, fmt, __VA_ARGS__)
	#define debug_print(fmt, ...) print_log(LOG_DEBUG, fmt, __VA_ARGS__)
	#define info_print(fmt, ...)  print_log(LOG_INFO,  fmt, __VA_ARGS__)
	#define warn_print(fmt, ...)  print_log(LOG_WARN,  fmt, __VA_ARGS__)
	#define error_print(fmt, ...) print_log(LOG_ERROR, fmt, __VA_ARGS__)
	#define fatal_print(fmt, ...) print_log(LOG_FATAL, fmt, __VA_ARGS__)

	#define print_log(level, ...) _print_log(level, __FILE__, __LINE__, __VA_ARGS__)

	void _print_log(LogLevel_t level, const char *file, int line, const char *fmt, ...);

	void close_log_stream();

} /* namespace zlog */

#endif /* INCLUDE_ZPSOE_ZLOG_H_ */

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

#include <iostream>
#include <string>

namespace zlog
{

	typedef enum
	{
		LOGOUTPUTSTREAM_FILE,
		LOGOUTPUTSTREAM_STDOUT,
		LOGOUTPUTSTREAM_STDERR,
		LOGOUTPUTSTREAM_NULL,
	} LogOutputStream_t;

	typedef enum
	{
		LOG_TRACE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_WARN,
		LOG_ERROR,
		LOG_FATAL,
		LOG_OFF,
	} LogLevel_t;

	typedef struct
	{
		LogLevel_t level;
		LogOutputStream_t output_to;
		size_t max_byte;
		std::string log_dir;
		std::string cur_prog_name;
		bool use_file_output;
		int flush_interval;
	} LogConfig_t;

	extern LogConfig_t log_config;
	extern char *buff;

#define trace_print(msg)                     \
	if (LOG_TRACE >= zlog::log_config.level) \
		std::cout << "[trace] [" << __FILE__ << "] [" << __FUNCTION__ << ":" << __LINE__ << "] " << msg << endl;

#define debug_print(msg)                     \
	if (LOG_DEBUG >= zlog::log_config.level) \
		std::cout << "[debug] [" << __FILE__ << "] [" << __FUNCTION__ << ":" << __LINE__ << "] " << msg << endl;

#define info_print(msg)                     \
	if (LOG_INFO >= zlog::log_config.level) \
		std::cout << "[info] [" << __FILE__ << "] [" << __FUNCTION__ << ":" << __LINE__ << "] " << msg << endl;

#define warn_print(msg)                     \
	if (LOG_WARN >= zlog::log_config.level) \
		std::cout << "[warn] [" << __FILE__ << "] [" << __FUNCTION__ << ":" << __LINE__ << "] " << msg << endl;

#define error_print(msg)                     \
	if (LOG_ERROR >= zlog::log_config.level) \
		std::cout << "[error] [" << __FILE__ << "] [" << __FUNCTION__ << ":" << __LINE__ << "] " << msg << endl;

#define fatal_print(msg)                     \
	if (LOG_FATAL >= zlog::log_config.level) \
		std::cout << "[fatal] [" << __FILE__ << "] [" << __FUNCTION__ << ":" << __LINE__ << "] " << msg << endl;

#define trace_log(...) print_log(zlog::LOG_TRACE, __VA_ARGS__)
#define debug_log(...) print_log(zlog::LOG_DEBUG, __VA_ARGS__)
#define info_log(...) print_log(zlog::LOG_INFO, __VA_ARGS__)
#define warn_log(...) print_log(zlog::LOG_WARN, __VA_ARGS__)
#define error_log(...) print_log(zlog::LOG_ERROR, __VA_ARGS__)
#define fatal_log(...) print_log(zlog::LOG_FATAL, __VA_ARGS__)

#define print_log(level, ...)                                      \
	{                                                              \
		sprintf(zlog::buff, __VA_ARGS__);                          \
		zlog::_print_log(level, __FILE__, __FUNCTION__, __LINE__); \
	}

	void _print_log(LogLevel_t level, const char *file, const char *fn, int line);

	void set_log_buffer_size(int size);

	void flush_log_stream();

	void close_log_stream();

} /* namespace zlog */

/*
 * zlog.cxx
 *
 *  Created on: 2014年9月30日
 *      Author: Fifi Lyu
 */
#include "stdafx.h"
#include "zlog.h"

#ifdef PLATFORM_WIN32
#include <process.h>
#include <Windows.h>
#else
#include <errno.h>
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <sstream>
#include <ctime>
#include <iostream>
#include <thread>

#ifdef PLATFORM_WIN32
static const string OS_LINE_FEED("\r\n");
#else
static const string OS_LINE_FEED("\n");
#endif

namespace zlog
{
	// 内部变量
	static FILE *log_file = nullptr;
	static FILE *log_std = nullptr;
	static bool haveInit = false;
	static char *LogLevelName[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL", " OFF "};

#ifdef PLATFORM_WIN32
	// 104857600 byte == 100 MiB
	LogConfig_t log_config = {LOG_INFO, LOGOUTPUTSTREAM_STDOUT, 104857600, "", true, "demo.exe"};
	static int pid_ = _getpid();

	string getTempDir()
	{
		char userpath[512] = {};
		char cname[20] = {};
		DWORD size = 20;
		TCHAR tname[20] = {};
		char *mode = "c:\\Users\\%s\\AppData\\Local\\Temp\\";
		GetUserName(tname, &size);
		WideCharToMultiByte(CP_ACP, 0, tname, -1, cname, 20, NULL, NULL);
		sprintf(userpath, mode, cname);
		return string(userpath);
	}
#else
	// 104857600 byte == 100 MiB
	static LogConfig_t log_config = {LOG_INFO, LOGOUTPUTSTREAM_STDOUT, 104857600, "/tmp/", true, "demo.exe"};
	static int pid_ = getpid();
#endif

	string int_to_str(const int &value)
	{
		ostringstream ss_;
		ss_ << value;
		return ss_.str();
	}

	string errno_to_str()
	{
#ifdef PLATFORM_WIN32
		string msg_(_strerror(NULL));
		const size_t size_ = msg_.size();

		if (size_)
			msg_.erase(size_ - 1);

		return msg_ + "(errno: " + int_to_str(errno) + ")";
#else
		return string(strerror(errno)) + "(errno: " + int_to_str(errno) + ")";
#endif
	}

	size_t get_size_in_byte(const string &file_name)
	{
		struct stat buf_;

		if (stat(file_name.c_str(), &buf_) == 0)
			return buf_.st_size;

		return 0;
	}

	// 获取当前时间,比如 2011-11-16 14:06:36
	string get_current_date()
	{
		char buf[80];
		time_t t = time(NULL);
		strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&t));
		return string(buf);
	}

	void open_log_file(FILE **file)
	{
#ifdef PLATFORM_WIN32
		log_config.log_dir = getTempDir();
#endif

		auto len = log_config.cur_prog_name.length();
		auto p = log_config.cur_prog_name.c_str();
		size_t i = 0;

		for (i = len; i >= 0; i--)
		{
			if (p[i] == '\\' || p[i] == '/')
			{
				i++;
				break;
			}
		}

		string progname = p + i;
		auto log_name = log_config.log_dir + progname + "_" + int_to_str(pid_) + ".log";
		const size_t old_log_size_ = get_size_in_byte(log_name);

		if (old_log_size_ >= log_config.max_byte)
			// 覆盖
			*file = fopen(log_name.c_str(), "wb");
		else
			// 追加
			*file = fopen(log_name.c_str(), "ab+");

		if (*file == NULL)
		{
			const string err_("cannot open \"" + log_config.log_dir + "\": " + errno_to_str());
			cerr << err_ << endl;
			exit(EXIT_FAILURE);
		}
	}

	void log_to_stream(FILE *log_stream, LogLevel_t level, const char *file, int line, const char *fmt, ...)
	{

		fprintf(log_stream, "[%s] [%s] [%d] ", LogLevelName[level], get_current_date().c_str(), line);

		if (level <= LOG_DEBUG)
			fprintf(log_stream, "%s:%d ", file, line);

		va_list ap_;
		va_start(ap_, fmt);
		vfprintf(log_stream, fmt, ap_);
		va_end(ap_);

		fprintf(log_stream, OS_LINE_FEED.c_str());
	}

	void _print_log(
		LogLevel_t level, const char *file, int line, const char *fmt, ...)
	{
		// 关闭日志
		if (level == LOG_OFF)
			return;

		// 关闭比用户设置更加详细的日志
		//if(level < log_config.level) return;

		if (haveInit == false)
		{
			haveInit = true;
			(new std::thread([=] {
				while (true)
				{
					std::this_thread::sleep_for(std::chrono::seconds(60));
					if (log_file != nullptr)
						fflush(log_file);
				}
			}))->detach();
		}

		if (log_std == nullptr)
		{
			switch (log_config.output_to)
			{
			case LOGOUTPUTSTREAM_STDOUT:
				log_std = stdout;
				break;
			case LOGOUTPUTSTREAM_STDERR:
				log_std = stderr;
				break;
			case LOGOUTPUTSTREAM_FILE:
				//log_std = ostringstream();
				break;
			default:
				log_std = stdout;
				break;
			}
		}

		if (log_file == nullptr)
		{
			open_log_file(&log_file);
		}

		va_list ap_;
		va_start(ap_, fmt);

		if (log_std != nullptr && level > log_config.level)
			log_to_stream(log_std, level, file, line, fmt, ap_);
		if (log_file != nullptr && log_config.use_file_output)
			log_to_stream(log_file, level, file, line, fmt, ap_);

		va_end(ap_);
	}

	void close_log_stream()
	{
		fflush(log_std);

		fflush(log_file);
		fclose(log_file);
		log_file = nullptr;
	}

} /* namespace zlog */

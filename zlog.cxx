/*
 * zlog.cxx
 *
 *  Created on: 2014年9月30日
 *      Author: Fifi Lyu
 */
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
#include <mutex>

using namespace std;

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
	static const char *LogLevelName[] = {"TRACE", "DEBUG", "INFO ", "WARN ", "ERROR", "FATAL", " OFF "};
	char *buff = new char[1024];
	static mutex mu;

#ifdef PLATFORM_WIN32
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

	// 104857600 byte == 100 MiB
	LogConfig_t log_config = {LOG_INFO, LOGOUTPUTSTREAM_STDOUT, 104857600, getTempDir(), "demo.exe", true, 60};
	static int pid_ = _getpid();

#else
	// 104857600 byte == 100 MiB
	LogConfig_t log_config = {LOG_INFO, LOGOUTPUTSTREAM_STDOUT, 104857600, "/tmp/", "demo.exe", true, 60};
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
			return static_cast<size_t>(buf_.st_size);

		return 0;
	}

	// 获取当前时间,比如 2011-11-16 14:06:36
	string get_current_date(const char *format = "%Y-%m-%d %H:%M:%S")
	{
		char buf[80];
		time_t t = time(nullptr);
		strftime(buf, 80, format, localtime(&t));
		return string(buf);
	}

	string get_file_name(const char *path, int len)
	{
		int i = len - 1;

		for (; i >= 0; i--)
		{
			if (path[i] == '\\' || path[i] == '/')
			{
				i++;
				break;
			}
		}

		return string(path + i);
	}

	void open_log_file(FILE **file)
	{
		auto len = log_config.cur_prog_name.length();
		auto progname = get_file_name(log_config.cur_prog_name.c_str(), len);
		auto log_name = log_config.log_dir + progname + get_current_date("_%Y-%m-%d %H-%M-%S_") + int_to_str(pid_) + ".log";
		const auto old_log_size_ = get_size_in_byte(log_name);

		if (old_log_size_ >= log_config.max_byte)
			// 覆盖
			*file = fopen(log_name.c_str(), "wb");
		else
			// 追加
			*file = fopen(log_name.c_str(), "ab+");

		if (*file == nullptr)
		{
			const string err_("cannot open \"" + log_config.log_dir + "\": " + errno_to_str());
			cerr << err_ << endl;
			exit(EXIT_FAILURE);
		}
	}

#define log_to_stream(log_stream, level, file, fn, line, ...)         \
	{                                                                 \
		fprintf(log_stream, "[%s] [%s] [%s:%s:%d] ",                  \
				LogLevelName[level], get_current_date().c_str(),      \
				get_file_name(file, strlen(file)).c_str(), fn, line); \
		fprintf(log_stream, __VA_ARGS__);                             \
		fprintf(log_stream, OS_LINE_FEED.c_str());                    \
	}

	void flush_file()
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(log_config.flush_interval));

			mu.lock();
			if (log_file != nullptr)
				fflush(log_file);
			mu.unlock();
		}
	}

	void _print_log(LogLevel_t level, const char *file, const char *fn, int line)
	{
		// 关闭日志
		if (level == LOG_OFF)
			return;

		if (haveInit == false)
		{
			haveInit = true;
			(new thread(flush_file))->detach();

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

				case LOGOUTPUTSTREAM_NULL:
					log_std = nullptr;
					break;
				}
			}

			if (log_file == nullptr && log_config.use_file_output)
			{
				open_log_file(&log_file);
			}
		}

		if (log_std != nullptr && level > log_config.level)
			log_to_stream(log_std, level, file, fn, line, buff);

		mu.lock();
		if (log_file != nullptr && log_config.use_file_output)
			log_to_stream(log_file, level, file, fn, line, buff);
		mu.unlock();
	}

	void flush_log_stream()
	{
		mu.lock();

		fflush(log_std);
		fflush(log_file);

		mu.unlock();
	}

	void close_log_stream()
	{
		mu.lock();

		if (log_std)
		{
			fflush(log_std);
		}

		if (log_file)
		{
			fflush(log_file);
			fclose(log_file);
			log_file = nullptr;
		}

		mu.unlock();
	}

	void set_log_buffer_size(int size)
	{
		delete buff;
		buff = new char[size];
	}

} /* namespace zlog */

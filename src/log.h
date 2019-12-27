#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <vector>
#include <stdarg.h>

#include "config.h"

#define G_LOG_H

enum LogLevel {
	Error,  // 错误信息
	Warn,   // 警告信息，不影响程序执行
	Info,   // 业务信息
	Debug,  // 调试信息
	Trace   // 详细信息
};

extern const std::string default_file;

class Logger {
public:
	explicit Logger(bool log = false, LogLevel level = LogLevel::Error,
					std::string file = default_file);
	~Logger() {}

	void loadConfig();

	// 设置日志记录文件名
	// bool setLogFile(std::string &file){ _file = file; }
	// 获取日志记录文件名
	std::string logFile() const { return _file; }
	// 设置日志记录等级
	// bool setLogLevel(const LogLevel &level){ _level = level; }
	// 获取日志记录等级
	LogLevel logLevel() const { return _level; }

	// 写日志
	template <typename... Aargs>
	bool log(const LogLevel level, const char *file, const int line,
			 const Aargs &... args) {
		if (_log) {
			std::unique_lock<std::mutex> lck(_mutex);
			if (_ofs.is_open()) {
				std::string str;
				std::string str_file = file;
				str += (logLeveltoString(level) + " " + str_file + " " +
						std::to_string(line) + " ") +
					   currentTime() + " ";
				logExpand(str, args...);
				logExpand("\n");
				return true;
			}
		}
		return false;
	}

	// 日志关闭
	bool logClose();

private:
	bool fileOpen(const std::string &file, std::ios::openmode mode);

	// 获取当前系统时间
	std::string currentTime() const;
	std::string currentTime(const std::string &format, const bool) const;

	// 生成日志文件名
	std::string generateLogFile() const;

	std::string logLeveltoString(const LogLevel &level);

private:
	template <typename T>
	void logExpand(const T &t) {
		_ofs << t;
	}
	template <typename T, typename... Tn>
	void logExpand(const T &t, const Tn &... args) {
		_ofs << t << " ";
		logExpand(args...);
	}

private:
	bool _log;			 // 日志启动标志
	LogLevel _level;	 // 日志等级
	std::mutex _mutex;   // 日志记录锁互斥量
	std::string _file;   // 日志记录文件名
	std::ofstream _ofs;  // 日志记录文件流
	// bool _sync;	// 日志同步标志，true:同步写入；false:异步写入

	// std::stringstream _ss;	// 数据暂存流
	// int _ss_len;			// 数据暂存流长度
};

extern Logger g_log;
#ifdef G_LOG_H
#define Log(level, ...) g_log.log(level, __FILE__, __LINE__, __VA_ARGS__)
#else
#define Log(level, ...)
#endif // G_LOG_H


#endif // __LOG_H__

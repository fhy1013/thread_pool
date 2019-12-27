#include "log.h"
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

const std::string default_file = "./run.log"; // 默认日志文件名

Logger g_log;

Logger::Logger(bool log, LogLevel level, std::string file):_log(log), _level(level), _file(file){
	loadConfig();
	if (_file == default_file) {
		_file = generateLogFile();
	}
	if (_log) {
		if (!fileOpen(_file, std::ios::out | std::ios::trunc)) {
			std::cout << "open log file error";
		} else {
			//	std::cout<< " open log file " << _file << " success"<<
			//std::endl;
		}
	}
}

std::string Logger::logLeveltoString(const LogLevel &level) {
	std::string level_str = "";
	switch (level) {
		case LogLevel::Error:
			level_str = "Error ";
			break;
		case LogLevel::Warn:
			level_str = "Warn ";
			break;
		case LogLevel::Info:
			level_str = "Info ";
			break;
		case LogLevel::Debug:
			level_str = "Debug ";
			break;
		case LogLevel::Trace:
			level_str = "Trace ";
			break;
	}
	return level_str;
}

void Logger::loadConfig() {
	Config config(g_config_file);

	// 日志启动标志
	std::string log;
	log = config.Read("log", log);
	_log = (log.length() > 0) ? ((std::stoi(log) == 0) ? false : true) : false;

	// 日志等级
	std::string level;
	level = config.Read("log_level", level);
	if (level == "Error")
		_level = LogLevel::Error;
	else if (level == "Warn")
		_level = LogLevel::Warn;
	else if (level == "Info")
		_level = LogLevel::Info;
	else if (level == "Debug")
		_level = LogLevel::Debug;
	else if (level == "Trace")
		_level = LogLevel::Trace;
	else
		_level = LogLevel::Error;
}

// 描述：
//      打开文件
// 参数：
//      file    I   打开文件文件名
//      mode    I   打开文件模式
// 返回值：
//      false   打开文件失败
//      true    打开文件成功
bool Logger::fileOpen(const std::string &file, std::ios::openmode mode) {
	_ofs.open(file, mode);
	if (!_ofs.is_open()) {
		return false;
	}
	return true;
}

// 描述：
//      关闭文件
// 返回值：
//      true    关闭文件成功
bool Logger::logClose() {
	std::unique_lock<std::mutex> lck(_mutex);
	if (_ofs.is_open()) {
		_ofs.close();
	}
	return true;
}

std::string Logger::currentTime() const {
	std::string format = "%Y-%m-%d %H:%M:%S";
	return currentTime(format, true);
}

std::string Logger::currentTime(const std::string &format,
								const bool type) const {
	auto now = std::chrono::system_clock::now();
	auto now_second = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	struct tm now_time;
	localtime_r(&now_second, &now_time);
	ss << std::put_time(&now_time, format.c_str());
	std::string now_str = ss.str();
	if (type) {
		auto mill = std::chrono::duration_cast<std::chrono::milliseconds>(
						now.time_since_epoch())
						.count();
		return now_str + "." + std::to_string(mill - now_second * 1000) + " ";
	} else {
		return now_str;
	}
}

std::string Logger::generateLogFile() const{
	std::string format = "%Y%m%d%H%M%S";
	std::string file;
	file += currentTime(format, false);
	file += ".log";
	return file;
}

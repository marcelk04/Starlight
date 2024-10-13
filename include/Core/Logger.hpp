#pragma once

#include <iostream>

// Always enable fatal and error log messages
#define LOG_ENABLE_FATAL 1
#define LOG_ENABLE_ERROR 1

// Only enable warn, info and debug log messages in debug mode
#ifndef NDEBUG

#define LOG_ENABLE_WARN 1
#define LOG_ENABLE_INFO 1
#define LOG_ENABLE_DEBUG 1

#endif

#if LOG_ENABLE_FATAL == 1
#define SFATAL(...) stl::Logger::get().print(stl::LogLevel::Fatal, __VA_ARGS__);
#else
#define SFATAL(...)
#endif

#if LOG_ENABLE_ERROR == 1
#define SERROR(...) stl::Logger::get().print(stl::LogLevel::Error, __VA_ARGS__);
#else
#define SERROR(...)
#endif

#if LOG_ENABLE_WARN == 1
#define SWARN(...) stl::Logger::get().print(stl::LogLevel::Warn, __VA_ARGS__);
#else
#define SWARN(...)
#endif

#if LOG_ENABLE_INFO == 1
#define SINFO(...) stl::Logger::get().print(stl::LogLevel::Info, __VA_ARGS__);
#else
#define SINFO(...)
#endif

#if LOG_ENABLE_DEBUG == 1
#define SDEBUG(...) stl::Logger::get().print(stl::LogLevel::Debug, __VA_ARGS__);
#else
#define SDEBUG(...)
#endif

namespace stl {

enum LogLevel {
	Fatal = 0,
	Error = 1,
	Warn = 2,
	Info = 3,
	Debug = 4
};

class Logger {
public:
	Logger() = default;

	static Logger get() {
		static Logger logger;
		return logger;
	}

	template<typename... Args>
	void print(LogLevel logLevel, Args&&... args) {
		std::cout << '[' << m_LogLevelStrings[logLevel] << "] ";

		print(args...);
	}

private:
	template<typename T>
	void print(T&& t) {
		std::cout << t << '\n';
	}

	template<typename T, typename... Args>
	void print(T&& t, Args&&... args) {
		std::cout << t;

		print(args...);
	}

private:
	const char* m_LogLevelStrings[5] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG" };
};

}
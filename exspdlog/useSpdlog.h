#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/fmt/bundled/printf.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"
//是否使用fmt格式
#define USE_FMT_LOG
class useSpdlog {
private:
    useSpdlog() = default;
    useSpdlog(const useSpdlog&) = delete;
    useSpdlog& operator=(const useSpdlog&) = delete;
public:
    static useSpdlog* config() {
        static useSpdlog a;
        return &a;
    }
    int default_save_days = 15;
    std::string global_debug_logger_name = "global_debug_logger";
    std::string global_info_logger_name = "global_info_logger";
    std::string global_error_logger_name = "global_error_logger";
    std::string global_debug_logger_path = "cvlogs/vm_global_debug.log";
    std::string global_info_logger_path = "cvlogs/vm_global_info.log";
    std::string global_error_logger_path = "cvlogs/vm_global_error.log";
    std::string default_pattern = "%^[%Y-%m-%d %H:%M:%S.%e] [%-8l] [%s:%#] %v%$";
    static std::shared_ptr<spdlog::logger> createAsyncRotatingFileLogger(spdlog::level::level_enum level, const std::string& logName = "spdlog", const std::string& logPath = "logs/spdlog.log", bool enableConsole = true, bool enableFile = true, const std::string& pattern = useSpdlog::config()->default_pattern)
    {
        assert(enableConsole || enableFile);
        auto p_logger = std::make_shared<spdlog::async_logger>(spdlog::async_logger(logName, {}, spdlog::thread_pool(), spdlog::async_overflow_policy::block));
        //auto p_logger = std::make_shared<spdlog::logger>(spdlog::logger(logName, {}));

        if (enableConsole)
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            p_logger->sinks().push_back(console_sink);
        }
        if (enableFile)
        {
            auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logPath, 0, 0, false, useSpdlog::config()->default_save_days);
            p_logger->sinks().push_back(file_sink);
        }
        p_logger->set_level(level);
        p_logger->set_pattern(pattern);
        return p_logger;

    }
    static void initSpdlog(spdlog::level::level_enum level, bool enableconsole = true, bool enableFile = true)
    {
        spdlog::init_thread_pool(100, 4);
        auto g_debug_logger = createAsyncRotatingFileLogger(spdlog::level::trace, useSpdlog::config()->global_debug_logger_name, useSpdlog::config()->global_debug_logger_path, enableconsole, enableFile);
        auto g_info_logger = createAsyncRotatingFileLogger(spdlog::level::info, useSpdlog::config()->global_info_logger_name, useSpdlog::config()->global_info_logger_path, enableconsole, enableFile, "%^[%Y-%m-%d %H:%M:%S.%e] [%-8l] %v%$");
        auto g_error_logger = createAsyncRotatingFileLogger(spdlog::level::warn, useSpdlog::config()->global_error_logger_name, useSpdlog::config()->global_error_logger_path, enableconsole, enableFile);
        spdlog::register_logger(g_debug_logger);
        spdlog::register_logger(g_info_logger);
        spdlog::register_logger(g_error_logger);
        spdlog::flush_every(std::chrono::seconds(3));
        spdlog::set_level(level);

    }

    static void register_AsyncRotatingFileLogger(spdlog::level::level_enum level, const std::string& logName, const std::string& logPath, bool enableConsole = true, bool enableFile = true, const std::string& pattern = useSpdlog::config()->default_pattern)
    {
        auto p_logger = createAsyncRotatingFileLogger(level, logName, logPath, enableConsole, enableFile, pattern);
        spdlog::register_logger(p_logger);

    }

    static void spd_printf_log(std::shared_ptr<spdlog::logger> logger, spdlog::level::level_enum level, char* file, int line, char* func, char* format, ...)
    {

        va_list args;
        va_start(args, format);
        char buff[1024];
        vsnprintf(buff, sizeof(buff), format, args);
        va_end(args);
        std::string msg = buff;
        logger->log(spdlog::source_loc{file, line, func}, level, msg);

    }

    static void set_level(spdlog::level::level_enum level)
    {
        spdlog::set_level(level);
    }
};
//printf格式
#define MODULE_LOG_TRACE_PRINTF(name,...) useSpdlog::spd_printf_log(spdlog::get(name),spdlog::level::level_enum::trace, __FILE__,__LINE__,__FUNCTION__, __VA_ARGS__)
#define MODULE_LOG_DEBUG_PRINTF(name,...) useSpdlog::spd_printf_log(spdlog::get(name),spdlog::level::level_enum::debug, __FILE__,__LINE__,__FUNCTION__, __VA_ARGS__)
#define MODULE_LOG_INFO_PRINTF(name,...) useSpdlog::spd_printf_log(spdlog::get(name),spdlog::level::level_enum::info, __FILE__, __LINE__, __FUNCTION__,__VA_ARGS__)
#define MODULE_LOG_WARN_PRINTF(name,...) useSpdlog::spd_printf_log(spdlog::get(name),spdlog::level::level_enum::warn, __FILE__, __LINE__,__FUNCTION__, __VA_ARGS__)
#define MODULE_LOG_ERROR_PRINTF(name,...) useSpdlog::spd_printf_log(spdlog::get(name),spdlog::level::level_enum::err, __FILE__, __LINE__, __FUNCTION__,__VA_ARGS__)
#define MODULE_LOG_CRITICAL_PRINTF(name,...) useSpdlog::spd_printf_log(spdlog::get(name),spdlog::level::level_enum::critical, __FILE__, __LINE__, __FUNCTION__,__VA_ARGS__)

//fmt格式
#define MODULE_LOG_TRACE_FMT(name,...) (spdlog::get(name))->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::trace, __VA_ARGS__)
#define MODULE_LOG_DEBUG_FMT(name,...) (spdlog::get(name))->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::debug, __VA_ARGS__)
#define MODULE_LOG_INFO_FMT(name,...) (spdlog::get(name))->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::info, __VA_ARGS__)
#define MODULE_LOG_WARN_FMT(name,...) (spdlog::get(name))->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::warn, __VA_ARGS__)
#define MODULE_LOG_ERROR_FMT(name,...) (spdlog::get(name))->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::err, __VA_ARGS__)
#define MODULE_LOG_CRITICAL_FMT(name,...) (spdlog::get(name))->log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, spdlog::level::level_enum::critical, __VA_ARGS__)

//fmt和printf格式选择
#ifndef USE_FMT_LOG
#define MODULE_LOG_TRACE(name,...) MODULE_LOG_TRACE_PRINTF(name, __VA_ARGS__)
#define MODULE_LOG_DEBUG(name,...) MODULE_LOG_DEBUG_PRINTF(name, __VA_ARGS__)
#define MODULE_LOG_INFO(name,...) MODULE_LOG_INFO_PRINTF(name, __VA_ARGS__)
#define MODULE_LOG_WARN(name,...) MODULE_LOG_WARN_PRINTF(name, __VA_ARGS__)
#define MODULE_LOG_ERROR(name,...) MODULE_LOG_ERROR_PRINTF(name, __VA_ARGS__)
#define MODULE_LOG_CRITICAL(name,...) MODULE_LOG_CRITICAL_PRINTF(name, __VA_ARGS__)
#else
#define MODULE_LOG_TRACE(name,...) MODULE_LOG_TRACE_FMT(name, __VA_ARGS__)
#define MODULE_LOG_DEBUG(name,...) MODULE_LOG_DEBUG_FMT(name, __VA_ARGS__)
#define MODULE_LOG_INFO(name,...) MODULE_LOG_INFO_FMT(name, __VA_ARGS__)
#define MODULE_LOG_WARN(name,...) MODULE_LOG_WARN_FMT(name, __VA_ARGS__)
#define MODULE_LOG_ERROR(name,...) MODULE_LOG_ERROR_FMT(name, __VA_ARGS__)
#define MODULE_LOG_CRITICAL(name,...) MODULE_LOG_CRITICAL_FMT(name, __VA_ARGS__)
#endif

#define LOG_TRACE(...) MODULE_LOG_TRACE(useSpdlog::config()->global_debug_logger_name, __VA_ARGS__)
#define LOG_DEBUG(...) MODULE_LOG_DEBUG(useSpdlog::config()->global_debug_logger_name, __VA_ARGS__)
#define LOG_INFO(...) MODULE_LOG_INFO(useSpdlog::config()->global_info_logger_name, __VA_ARGS__)
#define LOG_WARN(...) MODULE_LOG_WARN(useSpdlog::config()->global_error_logger_name, __VA_ARGS__)
#define LOG_ERROR(...) MODULE_LOG_ERROR(useSpdlog::config()->global_error_logger_name, __VA_ARGS__)
#define LOG_CRITICAL(...) MODULE_LOG_CRITICAL(useSpdlog::config()->global_error_logger_name, __VA_ARGS__)
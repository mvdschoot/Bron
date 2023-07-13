#include "Logger.h"

namespace Steve
{
	Ref<spdlog::logger> Logger::_core_logger;
	Ref<spdlog::logger> Logger::_app_logger;

	void Logger::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		_core_logger = spdlog::stdout_color_mt("CHEETS");
		_app_logger = spdlog::stdout_color_mt("APP");

		_core_logger->set_level(spdlog::level::trace);
		_app_logger->set_level(spdlog::level::trace);
	}
}

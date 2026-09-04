#include "Logger.h"

namespace bron {

Ref<spdlog::logger> Logger::core_logger_;
Ref<spdlog::logger> Logger::app_logger_;

void Logger::Init() {
	spdlog::set_pattern("%^[%T] %n: %v%$");
	core_logger_ = spdlog::stdout_color_mt("BRON");
	app_logger_ = spdlog::stdout_color_mt("APP");

	core_logger_->set_level(spdlog::level::trace);
	app_logger_->set_level(spdlog::level::trace);
}

} // namespace bron

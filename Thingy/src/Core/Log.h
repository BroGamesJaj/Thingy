#pragma once
#include "tpch.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace Thingy {

	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() noexcept { return s_ClientLogger; }
	};
}
//log macros
#define T_TRACE(...) ::Thingy::Log::GetClientLogger()->trace(__VA_ARGS__)
#define T_INFO(...)  ::Thingy::Log::GetClientLogger()->info(__VA_ARGS__)
#define T_WARN(...)  ::Thingy::Log::GetClientLogger()->warn(__VA_ARGS__)
#define T_ERROR(...) ::Thingy::Log::GetClientLogger()->error(__VA_ARGS__)
#define T_FATAL(...) ::Thingy::Log::GetClientLogger()->fatal(__VA_ARGS__)
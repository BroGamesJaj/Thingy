#pragma once

#include "tpch.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h"

namespace SpaceEngine {

	class Log
	{
	private:
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	};
}
//Client log macros
#define T_TRACE(...) ::SpaceEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define T_INFO(...)  ::SpaceEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define T_WARN(...)  ::SpaceEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define T_ERROR(...) ::SpaceEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define T_FATAL(...) ::SpaceEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)
#pragma once
#include "tpch.h"
namespace Thingy {

#define MODULE_CLASS_NAME(name) virtual std::string GetModuleName() const override { return name; }

	class Module {
	public:
		virtual ~Module() = default;

		virtual void OnUpdate() = 0;
		virtual void OnRender() = 0;

		virtual std::string GetModuleName() const = 0;

	};
}
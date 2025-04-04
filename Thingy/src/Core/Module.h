#pragma once
#include "tpch.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>


#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"
#include "Core\Image.h"

namespace Thingy {
	
#define MODULE_CLASS_NAME(name) virtual std::string GetModuleName() const override { return name; }

	class Module {
	public:
		virtual ~Module() = default;

		virtual void SetupSubscriptions() = 0;
		virtual void OnLoad(const std::variant<int, std::string> moduleState) = 0;
		virtual void OnUpdate() = 0;
		virtual void Window() = 0;
		virtual uint16_t OnRender() = 0;

		virtual std::string GetModuleName() const = 0;

		virtual const int DefaultSize() const = 0;

		ImVec2 GetSize() const { return ImGui::FindWindowByName(GetModuleName().data())->Size; };
		ImVec2 GetPos() const { return ImGui::FindWindowByName(GetModuleName().data())->Pos; };


	protected:
		ImGuiWindowFlags defaultWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	};
}
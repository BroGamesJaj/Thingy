#pragma once
#include "tpch.h"
#include "imgui.h"
#include "imgui_internal.h"

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
		virtual void OnLoad() = 0;
		virtual void OnUpdate() = 0;
		virtual void Window() = 0;
		virtual uint16_t OnRender() = 0;

		virtual std::string GetModuleName() const = 0;

		virtual int DefaultWidth() const = 0;

		int CurrentWidth() const { return (ImGui::FindWindowByName(GetModuleName().data())) ? ImGui::FindWindowByName(GetModuleName().data())->Size.x : DefaultWidth(); };

		ImVec2 GetSize() const { return ImGui::FindWindowByName(GetModuleName().data())->Size; };
		ImVec2 GetPos() const { return ImGui::FindWindowByName(GetModuleName().data())->Pos; };


	protected:
		ImGuiWindowFlags defaultWindowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
	};
}
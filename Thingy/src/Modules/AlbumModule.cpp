#include "AlbumModule.h"

namespace Thingy {

	void AlbumModule::SetupSubscriptions() {
		m_MessageManager->Subscribe("albumShare", "albumModule", [this](MessageData name) {
			std::visit([this](auto&& value) {
				using T = std::decay_t<decltype(value)>;
				if constexpr (std::is_same_v<T, Album>) {
					album = value;
					std::cout << album.toString() << std::endl;
				} else {
					T_ERROR("SceneManager: Invalid data type for scene name");
				}
				}, name);
			});
	}

	void AlbumModule::OnLoad() {
		textures[album.id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTexture(album.imageURL));
	}
	
	void AlbumModule::OnUpdate() {
	}
	
	void AlbumModule::Window() {

		ImVec2 bar_size = ImVec2(GetSize().x - 20, 30);
		ImGui::InvisibleButton("DragBar", bar_size);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			upProps |= BIT(0);
		}
		if (upProps & BIT(1)) upProps &= ~BIT(1);
		if (upProps & BIT(0) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			upProps |= BIT(1);
			upProps &= ~BIT(0);
		}
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);

		ImGui::Image((ImTextureID)(intptr_t)textures[album.id].get(), { 300.0f, 300.0f });
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(album.name.data());
		ImGui::Text("Track count: %zu", album.tracks.size());
		ImGui::SameLine();
		ImGui::Text("length");
		ImGui::EndGroup();

	}

	uint16_t AlbumModule::OnRender() {
		ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
		Window();
		ImGui::End();
		if (upProps & BIT(0)) {
			ImGui::BeginDisabled();
			ImGui::Begin("floater", nullptr, defaultWindowFlags);
			Window();
			ImGui::SetWindowPos({ ImGui::GetMousePos().x - GetSize().x / 2, ImGui::GetMousePos().y + 5 });
			ImGui::SetWindowSize(GetSize());
			ImGui::End();
			ImGui::EndDisabled();
		}
		return upProps;
	}
}
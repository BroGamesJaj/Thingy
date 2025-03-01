#include "AlbumModule.h"

namespace Thingy {

	void AlbumModule::SetupSubscriptions() {
		m_MessageManager->Subscribe("openAlbum", "albumModule", [this](const MessageData data) {
			
				if (data.type() == typeid(Album)) {
					Album recAlbum = std::any_cast<Album>(data);
					for (size_t i = 0; i < album.size(); i++) {
						if (album[i].name == recAlbum.name) {
							curr = i;
							T_INFO("returned");
							return;
						}
					}
					album.emplace_back(recAlbum);
					curr = album.size() - 1;
					std::cout << album[curr].toString() << std::endl;
				} else {
					T_ERROR("AlbumModule: Invalid data type for openAlbum");
				}
			});

		m_MessageManager->Subscribe("beforeSwitch" + GetModuleName(), GetModuleName(), [this](const MessageData data) {
			m_MessageManager->Publish("saveModuleState", std::make_pair<std::string, std::variant<int, std::string>>(GetModuleName(), album[curr].name));
			});
		
	}

	void AlbumModule::OnLoad(const std::variant<int, std::string> moduleState) {
		if (std::holds_alternative<std::string>(moduleState)) {
			std::string albumName = std::get<std::string>(moduleState);
			for (size_t i = 0; i < album.size(); i++) {
				if (album[i].name == albumName) {
					curr = i;
				}
			}
		}
		if (std::holds_alternative<int>(moduleState)) {
			T_ERROR("album got: {0}", std::get<int>(moduleState));
		}
		T_INFO("curr: {0}", curr);
		if (!textures[album[curr].id]) 
			textures[album[curr].id] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager->GetTexture(album[curr].imageURL));
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

		ImGui::Image((ImTextureID)(intptr_t)textures[album[curr].id].get(), {300.0f, 300.0f});
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::Text(album[curr].name.data());
		ImGui::Text("Track count: %zu", album[curr].tracks.size());
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
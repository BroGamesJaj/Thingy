#include "SearchModule.h"

namespace Thingy {
	void SearchModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openSearch", GetModuleName(), [this](MessageData data) {
			
			});
	}

	void SearchModule::OnLoad(const std::variant<int, std::string> moduleState) {
		std::unordered_map<uint32_t, std::future<Image>> images;	
		while (!images.empty()) {
			for (auto it = images.begin(); it != images.end(); ) {
				auto& image = it->second;
				if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
					textures[0101] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(image.get()));
					it = images.erase(it);
				} else {
					++it;
				}
			}
		}
	}

	void SearchModule::OnUpdate() {}

	void SearchModule::Window() {
	
	}

	uint16_t SearchModule::OnRender() {
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
#include "tpch.h"
#include "PlayerModule.h"
namespace Thingy {

	void PlayerModule::SetupSubscriptions() {
		m_MessageManager.Subscribe("openPlayer", GetModuleName(), [this](MessageData data) {
			open = true;
			});

		m_MessageManager.Subscribe("closePlayer", GetModuleName(), [this](MessageData data) {
			open = false;
			});

		m_MessageManager.Subscribe("changeQueueOpen", GetModuleName(), [this](MessageData data) {
			isQueueOpen = !isQueueOpen;
			});

	}

	void PlayerModule::OnLoad(const std::variant<int, std::string> moduleState) {
			
	}

	void PlayerModule::OnUpdate() {
		int currentTrackID = m_AudioManager.GetCurrentTrack().id; 
		if (isQueueOpen) {

			std::unordered_map<uint32_t, std::future<Image>> images;
			for (auto& track : queue) {
				if (!queueTextures[track.albumID]) {
					std::string& url = track.imageURL;
					images.emplace(track.albumID, std::async(std::launch::async, [this, &url]() { return m_ImageManager.GetImage(url); }));
				}

			}
			while (!images.empty()) {
				for (auto it = images.begin(); it != images.end(); ) {
					auto& image = it->second;
					if (image.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
						queueTextures[it->first] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTextureFromImage(image.get()));
						it = images.erase(it);
					} else {
						++it;
					}
				}
			}
		}

		if (currentTrackID != currentTrack.id && currentTrackID != -1) {
			open = true;
			currentTrack = m_AudioManager.GetCurrentTrack();
			if (!queueTextures[currentTrack.albumID]) {
				queueTextures[currentTrack.albumID] = std::unique_ptr<SDL_Texture, SDL_TDeleter>(m_ImageManager.GetTexture(currentTrack.imageURL));
			}
		}
	}

	void PlayerModule::Window() {
		ImVec2 bar_size = ImVec2(GetSize().x - 20, 30);
		ImGui::InvisibleButton("DragBar", bar_size);
		if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			upProps |= BIT(0);
		}
		if (ImGui::IsItemHovered()) {
			upProps |= BIT(2);
		} else {
			upProps &= ~BIT(2);
		}
		if (upProps & BIT(1)) upProps &= ~BIT(1);
		if (upProps & BIT(0) && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
			upProps |= BIT(1);
			upProps &= ~BIT(0);
		}
		ImGui::GetWindowDrawList()->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(255, 0, 0, 255), 0.0f, 0, 5.0f);

		if (isQueueOpen) {
			QueueView();
		} else {
			PlayerView();
		}
	}

	uint16_t PlayerModule::OnRender() {
		if (m_AudioManager.GetQueue().size() != 0 && open) {
			ImGui::Begin(GetModuleName().data(), nullptr, defaultWindowFlags);
			Window();
			ImGui::End();
			if (upProps & BIT(0)) {
				ImGui::BeginDisabled();
				ImGui::Begin("floater", nullptr, defaultWindowFlags);
				Window();
				ImGui::SetWindowPos({ ImGui::GetMousePos().x - (ImGui::FindWindowByName(GetModuleName().data())->Size.x / 2), ImGui::GetMousePos().y + 5 });
				ImGui::SetWindowSize(ImGui::FindWindowByName(GetModuleName().data())->Size);
				ImGui::End();
				ImGui::EndDisabled();
			}
		}
		return upProps;
	}

	void PlayerModule::QueueView() {
		if (ImGui::BeginTable("QueueTable", 2, ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY)) {
			ImGui::TableSetupColumn("Cover", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Info");

			int i = 0;
			for (auto& track : queue) {

				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Image(reinterpret_cast<ImTextureID>(queueTextures[track.albumID].get()), { 80.0f, 80.0f });

				ImGui::TableSetColumnIndex(1);
				std::string label = std::to_string(i);
				if (ImGui::Selectable(std::string("##" + label).data(), false, ImGuiSelectableFlags_AllowOverlap, ImVec2(0, 80.0f))) {
					
				}

				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Text(track.title.data());
				ImGui::Text(track.artistName.data());
				ImGui::EndGroup();
				i++;
			}

			ImGui::EndTable();
		}
	}

	void PlayerModule::PlayerView() {
		if (m_AudioManager.GetQueue().size() == 0) {
			ImGui::Button("image", { 300.0f, 300.0f });
		} else {
			ImGui::Image(reinterpret_cast<ImTextureID>(queueTextures[currentTrack.albumID].get()), {300.0f, 300.0f});
		}
		ImGui::Text(currentTrack.title.data());
		ImGui::Text(currentTrack.artistName.data());
		if (ImGui::Button("back", { 30.0f, 30.0f })) {
			m_AudioManager.PrevTrack();
		}
		ImGui::SameLine();
		if (m_AudioManager.IsMusicPaused()) {
			if (ImGui::Button("Play", { 30.0f, 30.0f })) {
				m_AudioManager.ResumeMusic();
			}
		} else {
			if (ImGui::Button("Pause", { 30.0f, 30.0f })) {
				m_AudioManager.PauseMusic();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Forward", { 30.0f, 30.0f })) {
			m_AudioManager.NextTrack();
		};
		ImGui::Text("Current");
		ImGui::SameLine();
		ImGui::SliderInt("time", &m_AudioManager.GetCurrentTrackPos(), 0, m_AudioManager.GetCurrentTrackDuration());
		if (ImGui::IsItemEdited()) {
			m_AudioManager.ChangeMusicPos();
		}
		ImGui::SliderInt("Volume", &m_AudioVolume, 0, MIX_MAX_VOLUME);
		if (ImGui::IsItemEdited()) {
			m_AudioManager.ChangeVolume();
		}
	}
}
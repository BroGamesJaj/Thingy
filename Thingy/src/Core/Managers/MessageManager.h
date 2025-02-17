#pragma once
#include "tpch.h"
#include "Core\Track.h"
#include "Core\Album.h"
#include "Core\Artist.h"

namespace Thingy {

	using MessageData = std::variant<int,Track, Album, Artist, std::string, std::vector<Track>, std::vector<Album>, std::vector<Artist>>;
	class MessageManager {
	public:
		using Callback = std::function<void(MessageData)>;

		MessageManager();
		~MessageManager();

		MessageManager(const MessageManager&) = delete;
		void operator=(const MessageManager&) = delete;

		void Subscribe(const std::string& event, const std::string& identifier, Callback callback) {
			listeners[event].push_back(std::make_pair(identifier, callback));
		}

		void Publish(const std::string& event, MessageData data) {
			if (listeners.find(event) != listeners.end()) {
				for (auto& pair : listeners[event]) {
					pair.second(data);
				}
			}
		}

		void UnSubscribe(const std::string& event, const std::string& identifier) {
			if (listeners.find(event) != listeners.end()) {
				auto& vec = listeners[event];
				vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const auto& pair) {
					return pair.first == identifier;
					}), vec.end());

				if (vec.empty()) {
					listeners.erase(event);
				}
			}
		}

		void UnSubscribeAll(const std::string& identifier) {
			for (auto& pair : listeners) {
				auto& vec = pair.second;
				vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const auto& pair) {
					return pair.first == identifier;
					}), vec.end());
			
				if (vec.empty()) {
					listeners.erase(pair.first);
				}
			}
		}
	private:
		std::unordered_map<std::string, std::vector<std::pair<std::string, Callback>>> listeners;
	};
}
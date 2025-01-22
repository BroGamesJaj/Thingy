#pragma once
#include "Core\Module.h"

namespace Thingy {
	class PlayerModule : Module {
	public:
		PlayerModule(std::vector<int>& queue) : m_Queue(queue) {
		}

		void OnUpdate() override;

		void OnRender() override;

		MODULE_CLASS_NAME("PlayerModule")
	private:
		std::vector<int>& m_Queue;
	};
}
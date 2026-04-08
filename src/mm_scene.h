#pragma once
#include "g_scenebase.h"

namespace MainMenu {

	struct Scene : Global::SceneBase {

		std::function<void()> delayCmd;
		float delayCmdExecTime{};

		template<typename Func>
		void SetDelayCmd(Func&& f, float waitTimespan_ = 0.35f) {
			delayCmd = std::forward<Func>(f);
			delayCmdExecTime = time + waitTimespan_;
		}

		void MakeUI() override;
		void Update() override;
	};

}

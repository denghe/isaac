#pragma once
#include "g_scenebase.h"

namespace MainMenu {

	struct Scene : Global::SceneBase<Scene> {
		void MakeUI() override;
		void Update() override;
	};

}

#pragma once
#include "g_scenebase.h"

namespace MainMenu {

	struct Scene : Global::SceneBase {
		void MakeUI() override;
		void Update() override;
	};

}

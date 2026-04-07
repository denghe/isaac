#pragma once
#include "g_scenebase.h"

namespace Test5 {

	struct Scene;
	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};
	};

}

#pragma once
#include "g_scenebase.h"

namespace Test2 {

	struct Scene;
	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};
	};

}

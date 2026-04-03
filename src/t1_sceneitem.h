#pragma once
#include "g_scenebase.h"

namespace Test1 {

	struct Scene;
	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};
	};

}

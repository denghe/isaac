#pragma once
#include "g_scenebase.h"

namespace Test3 {

	struct Scene;
	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};
	};

}

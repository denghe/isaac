#pragma once
#include "t4_scene.h"

namespace Test4 {

	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};

		void SceneItemInit(int32_t typeId_, Scene* scene_, XY pos_, float radius_);

		void Dispose() override;
	};

}

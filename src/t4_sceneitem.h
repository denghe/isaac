#pragma once
#include "t4_scene.h"

namespace Test4 {

	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};

		void SceneItemInit(int32_t typeId_, Scene* scene_, XY pos_, float radius_);

		void Dispose() override;

		// 修正位置值( 计算地图边缘回卷 )
		XY FixPosition(XY pos_);

		// 设置位置
		void SetPosition(XY pos_);
	};

}

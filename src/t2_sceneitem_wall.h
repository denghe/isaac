#pragma once
#include "t1_scene.h"

namespace Test2 {

	struct Wall : SceneItem {
		static constexpr int32_t cTypeId{ 1 };
		void Init(Scene* scene_, XY pos_);
		void Draw() override;
		void Dispose() override;
	};

}

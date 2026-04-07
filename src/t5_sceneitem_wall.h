#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct Wall : SceneItem {
		static constexpr int32_t cTypeId{ 1 };
		void Init(Scene* scene_, XY pos_);
		void Draw() override;
		void Dispose() override;
	};

}

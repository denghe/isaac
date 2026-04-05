#pragma once
#include "t3_scene.h"

namespace Test3 {

	struct Door : SceneItem {
		static constexpr int32_t cTypeId{ 2 };
		void Init(Scene* scene_, XY pos_, bool isCenter_);
		void Draw() override;
		void Dispose() override;
	};

}

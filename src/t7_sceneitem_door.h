#pragma once
#include "t7_scene.h"

namespace Test7 {

	struct Door : SceneItem {
		static constexpr int32_t cTypeId{ 2 };

		void Init(Scene* scene_, XY pos_, bool isCenter_);
		void Draw() override;
		void Dispose() override;
	};

}

#pragma once
#include "t3_scene.h"

namespace Test3 {

	struct Bucket : SceneItem {
		static constexpr int32_t cTypeId{ 4 };
		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;
	};

}

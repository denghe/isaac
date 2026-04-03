#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Bucket : SceneItem {
		static constexpr int32_t cTypeId{ __LINE__ };
		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;	// unsafe
		~Bucket() override;
	};

}

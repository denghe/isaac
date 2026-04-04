#pragma once
#include "t2_scene.h"

namespace Test2 {

	struct Bucket : SceneItem {
		static constexpr int32_t cTypeId{ 4 };

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;

		// 爆炸( 创建爆炸体并自杀 )
		void Explode();
	};

}

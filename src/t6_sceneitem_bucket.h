#pragma once
#include "t6_scene.h"

namespace Test6 {

	struct Bucket : SceneItem, DotContainer {
		static constexpr int32_t cTypeId{ 4 };

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;

		// 爆炸( 创建爆炸体并自杀 )
		void Explode();
	};

}

#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct Bucket;
	// 爆炸者( 由炸药桶爆炸时创建，负责爆炸效果和伤害逻辑 )
	struct Exploder : SceneItem {
		static constexpr int32_t cTypeId{ 5 };

		static constexpr float cExplodeRadius{ 128.f };
		static constexpr float cExplodeDuration{ 0.1f };
		static constexpr float cFadeoutDuration{ 0.3f };
		int32_t _1{}, numSteps{};
		float inc{}, alpha{ 1 };
		void Init(Bucket* bucket_);
		void Update() override;
		void Draw() override;
		void Dispose() override;
	};

}

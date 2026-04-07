#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct Player : SceneItem {
		static constexpr int32_t cTypeId{ 3 };
		static constexpr XY cSpeedMax{ 2000.f };
		static constexpr XY cAccel{ cSpeedMax * 0.1f };
		XY lastMoveDir{}, speed{};
		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;
	};

}

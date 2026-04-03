#pragma once
#include "t1_scene.h"

namespace Test1 {

	struct Player : SceneItem {
		static constexpr int32_t cTypeId{ __LINE__ };
		static constexpr XY cSpeedMax{ 2000.f };
		static constexpr XY cAccel{ cSpeedMax * 0.1f };
		XY lastMoveDir{}, speed{};
		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;	// unsafe
		~Player() override;
	};

}

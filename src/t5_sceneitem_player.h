#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct Player : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		static constexpr XY cSpeedMax{ 5000.f };
		static constexpr XY cAccel{ cSpeedMax * 0.1f };

		XY lastMoveDir{}, speed{};
		XY lastBulletCosSin{};		// 存储上次子弹发射方向，在无法计算方向时当作参考

		void HandleMove();
		void HandleShot();

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;
	};

}

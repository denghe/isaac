#pragma once
#include "t7_scene.h"

namespace Test7 {

	struct Player : SceneItem {
		static constexpr int32_t cTypeId{ 3 };

		static constexpr XY cSpeedMax{ 5000.f };
		static constexpr XY cAccel{ cSpeedMax * 0.1f };
		static constexpr float cAngleAccLimit{ xx::gPI * 2 / (gg.cFps * 0.2f) };

		// 为移动逻辑服务
		XY lastMoveDir{}
			// 当前移动速度
			, speed{};
		// 缓存 radians 对应的 cos sin 值
		// 角色通过转动角度的方式对准鼠标，有角速度限制
		// 子弹发射方向靠读取角色 radians, cos, sin 来算相对数据
		XY cosSin{};

		// todo: 完整的 properties 面板
		// 总伤害值
		int32_t damage{};

		void HandleDoors();
		void HandleMove();
		void HandleRotate();
		void HandleShot();

		void Init(Scene* scene_, XY pos_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}

#pragma once
#include "t4_sceneitem.h"

namespace Test4 {

	struct Monster : SceneItem {
		static constexpr int32_t cTypeId{ 1 };
		static constexpr float cLifespan{ 9999999.f };
		static constexpr float cShootInterval{ 1.f };
		int32_t _1{};
		float deathTime{}, nextShootTime{};
		xx::RGBA8 color{};
		void Init(Scene* scene_, XY pos_, float radius_, xx::RGBA8 color_ = xx::RGBA8_White);
		void Update() override;
		void Draw() override;
		void OnDispose() override;
		void SetPosition(XY pos_);
	};

}

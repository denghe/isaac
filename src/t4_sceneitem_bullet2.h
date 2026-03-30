#pragma once
#include "t4_sceneitem_bullet1.h"

namespace Test4 {

	struct Bullet2 : SceneItem {
		static constexpr int32_t cTypeId{ 2 };
		static constexpr float cLifespan{ 10.f };
		static constexpr float cMoveSpeed{ 300.f };
		static constexpr xx::FromTo<float> cMoveDuration{ 0.3f, 0.7f };
		static constexpr xx::FromTo<float> cMoveInterval{ 0.2f, 0.5f };
		xx::Weak<Monster> owner;
		int32_t _1{};
		float deathTime{};
		int32_t moveLeftStep{};
		XY moveInc{};
		void Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Bullet1> mother_);
		void Update() override;
		void Draw() override;
	};

}

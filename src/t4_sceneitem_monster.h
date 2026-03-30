#pragma once
#include "t4_sceneitem.h"

namespace Test4 {

	struct Monster : SceneItem {
		static constexpr int32_t cTypeId{ __LINE__ };
		static constexpr float cLifespan{ 100.f };
		static constexpr float cShootInterval{ 1.f };
		int32_t _1{};
		float deathTime{}, nextShootTime{};
		void Init(Scene* scene_, XY pos_, float radius_);
		void Update() override;
		void Draw() override;
	};

}

#pragma once
#include "t5_scene.h"

namespace Test5 {

	struct Player;
	struct PlayerBullet : SceneItem {
		static constexpr int32_t cTypeId{ 6 };

		static constexpr XY cSpeed{ 1000.f };
		static constexpr float cMaxLifetime{ 5.f };

		xx::Weak<Player> owner;
		XY inc{};
		float deathTime{};

		void Init(xx::Weak<Player> owner_, XY tarPos_);
		void Update() override;
		void Draw() override;
		void Dispose() override;
	};

}

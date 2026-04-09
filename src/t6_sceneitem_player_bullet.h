#pragma once
#include "t6_scene.h"

namespace Test6 {

	struct Player;
	struct PlayerBullet : SceneItem {
		static constexpr int32_t cTypeId{ 6 };

		static constexpr XY cSpeed{ 1000.f };
		static constexpr float cMaxLifetime{ 5.f };

		xx::Weak<Player> owner;
		XY inc{};
		float deathTime{};

		// 伤害值( 创建时从 player 身上复制 )
		int32_t damage{};

		void Init(xx::Weak<Player> owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}

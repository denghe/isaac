#pragma once
#include "t6_scene.h"

namespace Test6 {

	// 穿刺信息
	struct PierceInfo {
		// 目标
		xx::Weak<SceneItem> target;
		// 过期时间点
		float elapsedTime;
	};

	struct Player;
	struct PlayerBullet : SceneItem {
		static constexpr int32_t cTypeId{ 6 };

		// 移动速度
		static constexpr XY cSpeed{ 1000.f };
		// 最大存活时长
		static constexpr float cMaxLifetime{ 5.f };
		// 初始穿刺次数
		static constexpr int32_t cPierceCount{ 3 };
		// 穿刺间隔时长( 针对相同对象 )
		static constexpr float cPierceInterval{ 0.1f };

		// 指向子弹拥有者
		xx::Weak<Player> owner;
		// 每帧移动步进值
		XY inc{};
		// 死亡时间点 = 创建时时间 + 最大存活时长
		float deathTime{};

		// 伤害值( 创建时从 player 身上复制 )
		int32_t damage{};
		// 剩余穿刺次数( 命中 1 次，减 1 )
		int32_t leftPierceCount{};
		// 穿刺信息表( 黑名单, 避免子弹连续多帧判定同一个对象 )
		xx::List<PierceInfo> pierceInfos;

		void Init(xx::Weak<Player> owner_);
		void Update() override;
		void Draw() override;
		void DrawLight() override;
		void Dispose() override;
	};

}

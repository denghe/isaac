#pragma once
#include "t6_sceneitem.h"

namespace Test6 {

	struct SceneItem;
	struct Scene;
	struct Dot {

		//static constexpr int32_t cTypeId{/* 派生类需要带这行，值不重复 */};
		// 需要在派生类中 typeId = cTypeId;
		int32_t typeId{};

		// 存储 dot 过期/死亡时间点
		float deathTime{};

		// 指向主场景. 需要在派生类中 赋值
		Scene* scene{};

		// 返回 false 表示 自杀, 需要释放内存
		virtual bool Update(SceneItem* container_);

		virtual ~Dot() = default;
	};

	/**********************************************************************/
	/**********************************************************************/

	struct PlayerBullet;

	// 所有 dot 的容器. 和 SceneItem 一起 多重继承 以方便使用
	struct DotContainer {
		xx::List<xx::Shared<Dot>> dots;

		// 倒序执行所有 dots
		void UpdateDots(SceneItem* container_);

		// 下面是所有 dot 的创建函数. 合并/替换 啥的操作都在这些函数里做
		void MakePlayerDot1(PlayerBullet* caster_);
		// ...
	};

	/**********************************************************************/
	/**********************************************************************/

	// 此物由 玩家子弹 命中目标后 创建
	struct PlayerDot1 : Dot {
		static constexpr int32_t cTypeId{ 1 };
		// 每秒产生 2 次子弹伤害, 持续 5 秒
		static constexpr float cLifespan{ 5.f };
		static constexpr float cDamageInterval{ 0.5f };

		int32_t damage{};
		float nextDamageTime{};

		void Init(PlayerBullet* caster_);

		// 针对已存在 dot: 叠加伤害 并 延续时长
		void Refresh(PlayerBullet* caster_);

		bool Update(SceneItem* container_) override;
	};

	// ... more Dot Types here
}

#include "pch.h"
#include "t4_sceneitem_bullet2.h"

namespace Test4 {

	void Bullet2::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Bullet1> mother_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		owner = mother_->owner;
		deathTime = scene->time + cLifespan;
	}

	void Bullet2::Update() {
		// 随机移动. 每隔一段时间随机移动一次，持续一段时间，间隔一段时间

		// 生命周期处理
		if (scene->time >= deathTime || !owner) {
			// todo: 淡出
			Dispose();	// unsafe
			return;
		}

		// 碰撞检测
		auto cri = scene->itemsGrid.PosToCRIndex(pos);
		auto searchRange = radius * 2;
		bool crossed{};
		scene->itemsGrid.ForeachByRange(cri.y, cri.x, searchRange, gg.sgrdd, [&](decltype(scene->itemsGrid)::Node& node, float range)->void {
			// 避开自身
			if (node.value == this) return;
			// 避开非怪
			if (node.value->typeId != Monster::cTypeId) return;
			// 避开主人
			auto m = (Monster*)node.value;
			if (m == owner.GetPointer()) return;
			// 距离检测
			auto d = pos - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = radius + node.cache.radius;
			auto rr = r * r;
			// 未碰撞
			if (mag2 > rr) return;
			// 干掉目标
			crossed = true;
			node.value->Dispose();
			});
		// 如果碰到目标，自己也消失
		if (crossed) {
			TryDispose();
			return;
		}

		// 移动处理
		XX_BEGIN(_1);
		while (1) {
			// 设置移动方向和持续时间
			{
				// 计算要移动多少帧
				auto duration = gg.rnd.Next(cMoveDuration);
				moveLeftStep = std::ceilf(duration * gg.cFps);
				// 随机选一个移动方向
				auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
				// 计算移动增量
				moveInc = XY{ std::cosf(radians), std::sinf(radians) } * cMoveSpeed * gg.cDelta;
			}
			// 移动
			while (moveLeftStep > 0) {
				XX_YIELD(_1);
				--moveLeftStep;
				SetPosition(pos + moveInc);
			}
			// 设置要休息多久
			{
				// 计算要休息多少帧
				auto duration = gg.rnd.Next(cMoveInterval);
				moveLeftStep = std::ceilf(duration * gg.cFps);
			}
			// 休息
			while (moveLeftStep > 0) {
				XX_YIELD(_1);
				--moveLeftStep;
			}
		}
		XX_END(_1);
	}

	void Bullet2::Draw() {
		scene->DrawItem(gg.pics.c32_bullet, pos, radius * (1.f / 16.f), owner->color);
	}

}

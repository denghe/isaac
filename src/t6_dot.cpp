#include "pch.h"
#include "t6_.h"

namespace Test6 {

	bool Dot::Update(SceneItem* container_) {
		return false;
	}

	/**********************************************************************/
	/**********************************************************************/

	void DotContainer::UpdateDots(SceneItem* container_) {
		for (auto i = dots.len - 1; i >= 0; --i) {
			if (dots[i]->Update(container_)) {
				dots.SwapRemoveAt(i);
			}
		}
	}

	void DotContainer::MakePlayerDot1(PlayerBullet* caster_) {
		// 先检查是否已经有相同 dot 存在，如果存在则 叠加伤害 并 延续时长
		for (auto& dot : dots) {
			if (dot->typeId == PlayerDot1::cTypeId) {
				auto& o = dot.Cast<PlayerDot1>();
				o->damage += caster_->damage;
				o->deathTime += PlayerDot1::cLifespan;
				return;
			}
		}
		// 新建
		dots.Emplace().Emplace<PlayerDot1>()->Init(caster_);
	}

	/**********************************************************************/
	/**********************************************************************/

	void PlayerDot1::Init(PlayerBullet* caster_) {
		typeId = cTypeId;
		scene = caster_->scene;
		deathTime = scene->time + cLifespan;
		damage = caster_->damage;
		nextDamageTime = scene->time + cDamageInterval;
	}

	bool PlayerDot1::Update(SceneItem* container_) {
		// 超时: 自杀
		if (scene->time >= deathTime) {
			return true;
		}
		// 到达产生伤害的时间点: 产生伤害 并更新时间点
		if (scene->time >= nextDamageTime) {
			container_->properties->hp -= damage;	// todo: 改成调用函数 以方便 套公式
			nextDamageTime = scene->time + cDamageInterval;
		}

		return false;
	}

}

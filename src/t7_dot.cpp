#include "pch.h"
#include "t7_.h"

namespace Test7 {

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
		// 先检查是否已经有相同 dot 存在，如果存在则 Refresh
		for (auto& dot : dots) {
			if (dot->typeId == PlayerDot1::cTypeId) {
				dot.Cast<PlayerDot1>()->Refresh(caster_);
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
		Refresh(caster_);
	}

	void PlayerDot1::Refresh(PlayerBullet* caster_) {
		deathTime = scene->time + cLifespan;
		damage += caster_->damage;
		nextDamageTime = 0;	// update 时立即触发一次
	}

	bool PlayerDot1::Update(SceneItem* container_) {
		// 超时: 自杀
		if (scene->time >= deathTime) {
			return true;
		}
		// 到达产生伤害的时间点: 产生伤害 并更新时间点
		if (scene->time >= nextDamageTime) {
			container_->properties->hp -= damage;	// todo: 改成调用函数 以方便 套公式
			scene->effectTexts.Add(container_->pos, { 0,-1 }, xx::RGBA8_Red, 2 * scene->cam.scale, -damage, true);

			nextDamageTime = scene->time + cDamageInterval;
		}

		return false;
	}

}

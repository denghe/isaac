#include "pch.h"
#include "t4_sceneitem_bullet1.h"
#include "t4_sceneitem_bullet2.h"

namespace Test4 {

	void Bullet1::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Monster> owner_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		owner = std::move(owner_);
		deathTime = scene->time + cLifespan;
		nextShootTime = scene->time + cShootInterval;
	}

	void Bullet1::Update() {
		// todo
		// 每隔一段时间发射一颗 2 级子弹. 随机移动
		if (scene->time >= deathTime) {
			Dispose();	// unsafe
			return;
		}
		if (scene->time >= nextShootTime) {
			nextShootTime += cShootInterval;
			// 计算子弹的初始位置和属性
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			auto bulletPos = pos + XY{ cosf(radians), sinf(radians) } * radius;
			scene->items.Emplace().Emplace<Bullet2>()->Init(scene, bulletPos, 32.f, xx::WeakFromThis(this));
		}

		// 移动处理. 每隔一段时间随机移动一次，持续一段时间，间隔一段时间
		XX_BEGIN(_1);
		while (1) {
			{
				// 计算要移动多久
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
				pos += moveInc;
				// todo: sync pos to grid
			}
		}
		XX_END(_1);
	}

	void Bullet1::Draw() {
		scene->DrawItem(gg.pics.c64_bullet, pos, radius * (1.f / 32.f));
	}

}

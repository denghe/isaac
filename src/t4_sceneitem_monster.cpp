#include "pch.h"
#include "t4_sceneitem_monster.h"
#include "t4_sceneitem_bullet1.h"

namespace Test4 {

	void Monster::Init(Scene* scene_, XY pos_, float radius_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		deathTime = scene->time + cLifespan;
		nextShootTime = scene->time + cShootInterval;
	}

	void Monster::Update() {
		// 每隔一段时间发射一颗子弹. 静止不动. 可以活很久
		if (scene->time >= deathTime) {
			Dispose();	// unsafe
			return;
		}
		if (scene->time >= nextShootTime) {
			nextShootTime += cShootInterval;
			// 计算子弹的初始位置和属性
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			auto bulletPos = pos + XY{ cosf(radians), sinf(radians) } * radius;
			scene->items.Emplace().Emplace<Bullet1>()->Init(scene, bulletPos, 64.f, xx::WeakFromThis(this));
		}
	}

	void Monster::Draw() {
		scene->DrawItem(gg.pics.c128_monster, pos, radius * (1.f / 64.f));
	}

}

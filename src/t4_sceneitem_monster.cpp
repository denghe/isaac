#include "pch.h"
#include "t4_sceneitem_monster.h"
#include "t4_sceneitem_bullet1.h"

namespace Test4 {

	void Monster::Init(Scene* scene_, XY pos_, float radius_, xx::RGBA8 color_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		scene_->itemsGrid64.Add(indexAtGrid, this);
		color = color_;
		deathTime = scene->time + cLifespan;
		nextShootTime = scene->time + cShootInterval;
	}

	void Monster::Update() {
		// 每隔一段时间发射一颗子弹. 静止不动. 可以活很久

		// 生命周期处理
		if (scene->time >= deathTime) {
			Dispose();	// unsafe
			return;
		}

		// 发射子弹处理
		while (scene->time >= nextShootTime) {
			nextShootTime += cShootInterval;
			// 如果对象创建数量达到上限，直接跳过创建
			if (scene->items.len >= Scene::cNumMaxItems) {
				++scene->createIgnoreCount;
				continue;
			}
			// 计算子弹的初始位置和属性
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			auto bulletPos = pos + XY{ cosf(radians), sinf(radians) } * radius;
			scene->items.Emplace().Emplace<Bullet1>()->Init(scene, bulletPos, 64.f, xx::WeakFromThis(this));
		}
	}

	void Monster::Draw() {
		scene->DrawItem(gg.pics.c128_monster, pos, radius * (1.f / 64.f), color);
	}

	void Monster::OnDispose() {
		// 从 grid 中移除对象，避免被查询到
		if (indexAtGrid != -1) {
			assert(scene->itemsGrid64.NodeAt(indexAtGrid).value == this);
			scene->itemsGrid64.Remove(indexAtGrid, this);
		}
	}

	void Monster::SetPosition(XY pos_) {
		SceneItem::SetPosition(pos_);
		// 新的 pos 值同步到空间索引 grid
		scene->itemsGrid64.Update(indexAtGrid, this);
	}
}

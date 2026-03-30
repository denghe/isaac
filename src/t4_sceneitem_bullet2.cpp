#include "pch.h"
#include "t4_sceneitem_bullet2.h"

namespace Test4 {

	void Bullet2::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Bullet1> mother_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		owner = mother_->owner;
	}

	void Bullet2::Update() {
		// todo: 随机移动
	}

	void Bullet2::Draw() {
		scene->DrawItem(gg.pics.c32_bullet, pos, radius * (1.f / 16.f));
	}

}

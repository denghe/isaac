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
		// todo: 碰撞检测

		// 生命周期处理
		if (scene->time >= deathTime) {
			Dispose();	// unsafe
			return;
		}

		// 移动处理
		XX_BEGIN(_1);
		while (1) {
			// "协程"代码中不可夹杂局部变量，必须放在括号里，且不能跨越 yield
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

	void Bullet2::Draw() {
		scene->DrawItem(gg.pics.c32_bullet, pos, radius * (1.f / 16.f));
	}

}

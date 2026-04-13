#include "pch.h"
#include "t7_.h"

namespace Test7 {

	// todo: 场景内传送门，不可破坏地形 ( 阻挡 移动 + 子弹的，不阻挡子弹的 )

	void Player::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cPlayerRadius;
		scale = radius * 2.f / gg.pics.c128_player.uvRect.w;
		radians = {};

		scene->phys->Add(this);

		damage = 1;
	}

	void Player::HandleDoors() {
		// 判断是否走到了 门 的位置，触发房间切换
		if (scene->IsCrossDoor(pos, radius)) {
			// todo: 处理函数或许应该 挂在门上。针对不同的对象 穿越门 有着不同的处理方式
			xx::CoutN("visit door");
		}
	}

	void Player::HandleMove() {
		// 玩家输入逻辑，支持同时按下相反方向键时切换到另一个方向
		XY moveDir{};
		// x
		if ((gg.keyboard[GLFW_KEY_A])
			&& gg.keyboard[GLFW_KEY_D]) {
			if (lastMoveDir.x == -1) {
				moveDir.x = 1;
			}
			else if (lastMoveDir.x == 1) {
				moveDir.x = -1;
			}
			else {
				moveDir.x = 0;
			}
		}
		else if (gg.keyboard[GLFW_KEY_A]) {
			lastMoveDir.x = moveDir.x = -1;
		}
		else if (gg.keyboard[GLFW_KEY_D]) {
			lastMoveDir.x = moveDir.x = 1;
		}
		else {
			lastMoveDir.x = moveDir.x = 0;
		}
		// y
		if ((gg.keyboard[GLFW_KEY_W])
			&& gg.keyboard[GLFW_KEY_S]) {
			if (lastMoveDir.y == -1) {
				moveDir.y = 1;
			}
			else if (lastMoveDir.y == 1) {
				moveDir.y = -1;
			}
			else {
				moveDir.y = 0;
			}
		}
		else if (gg.keyboard[GLFW_KEY_W]) {
			lastMoveDir.y = moveDir.y = -1;
		}
		else if (gg.keyboard[GLFW_KEY_S]) {
			lastMoveDir.y = moveDir.y = 1;
		}
		else {
			lastMoveDir.y = moveDir.y = 0;
		}

		// 计算移动速度
		if (moveDir.x == 1) {		// right
			if (speed.x < 0) speed.x = 0;
			speed.x += cAccel.x;
			if (speed.x > cSpeedMax.x) {
				speed.x = cSpeedMax.x;
			}
		}
		else if (moveDir.x == -1) {	// left
			if (speed.x > 0) speed.x = 0;
			speed.x -= cAccel.x;
			if (speed.x < -cSpeedMax.x) {
				speed.x = -cSpeedMax.x;
			}
		}
		else {
			speed.x = 0;
		}

		if (moveDir.y == 1) {		// down
			if (speed.y < 0) speed.y = 0;
			speed.y += cAccel.y;
			if (speed.y > cSpeedMax.y) {
				speed.y = cSpeedMax.y;
			}
		}
		else if (moveDir.y == -1) {	// up
			if (speed.y > 0) speed.y = 0;
			speed.y -= cAccel.y;
			if (speed.y < -cSpeedMax.y) {
				speed.y = -cSpeedMax.y;
			}
		}
		else {
			speed.y = 0;
		}
		
		// 对角线方向加速修正
		XY accel;
		if (speed.x != 0 && speed.y != 0) {
			accel = speed * 0.7071f;
		}
		else {
			accel = speed;
		}
		// 物理附加加速度
		scene->phys->At(this).cache.accel += accel;

		// todo: 似乎速度值的表现偏慢，需要 fix
	}

	void Player::HandleRotate() {
		// 令角色始终面对鼠标
		auto mp = scene->cam.ToLogicPos(gg.mousePos);
		auto d = mp - pos;
		auto r = std::atan2f(d.y, d.x);
		xx::AngleStep(radians, r, cAngleAccLimit);
		cosSin.x = std::cosf(radians);
		cosSin.y = std::sinf(radians);
	}

	void Player::HandleShot() {
		// 处理子弹射击逻辑
		if (gg.mouse[GLFW_MOUSE_BUTTON_1](0.1f)) {
			scene->playerBullets.Emplace().Emplace()->Init(xx::WeakFromThis(this));
		}
	}

	void Player::Update() {
		HandleDoors();
		HandleRotate();
		HandleMove();
		HandleShot();
	}

	void Player::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_player, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Player::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (2048.f / 64.f) * scene->cam.scale);
	}

	void Player::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		scene->phys->Remove(this);

		// 从容器中移除对象( 释放内存 )
		scene->player.Reset();
	}

}

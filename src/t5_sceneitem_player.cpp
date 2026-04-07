#include "pch.h"
#include "t5_.h"

namespace Test5 {

	void Player::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cPlayerRadius;
		scale = radius * 2.f / gg.pics.cell_player.uvRect.w;
		radians = {};

		indexAtContainer = scene->players.len - 1;
		assert(scene->players[indexAtContainer].pointer == this);

		scene->phys->Add(this);
	}

	void Player::Update() {

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
	}

	void Player::Draw() {
		gg.Quad().DrawFrame(gg.pics.cell_player, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Player::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->players;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		scene->phys->Remove(this);

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}

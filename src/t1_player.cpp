#include "pch.h"
#include "t1_.h"

namespace Test1 {

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
		//scene->gridItems.Add(indexAtGrid, this);
		indexAtGrid = scene->phys->Add(this);
	}

	void Player::Update() {

		// input check
		XY moveDir{};
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

		// speed calc
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

		//// new pos calc
		//auto newPos = pos;
		//if (speed.x != 0 || speed.y != 0) {
		//	if (speed.x != 0 && speed.y != 0) {
		//		newPos += speed * 0.7071f;
		//	}
		//	else {
		//		newPos += speed;
		//	}

		//	// todo: some check?
		//}

		//// move
		//if (pos != newPos) {
		//	pos = newPos;
		//	y = pos.y;
		//	scene->gridItems.Update(indexAtGrid, this);
		//}

		XY acc;
		if (speed.x != 0 && speed.y != 0) {
			acc = speed * 0.7071f;
		}
		else {
			acc = speed;
		}


		scene->phys->At(indexAtGrid).acc += acc;
	}

	void Player::Draw() {
		gg.Quad().DrawFrame(gg.pics.cell_player, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Player::Dispose() {
		auto i = indexAtContainer;
		assert(scene->players[i].pointer == this);
		scene->players.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->players.SwapRemoveAt(i);
	}

	Player::~Player() {
		if (indexAtGrid > -1) {
			//scene->gridItems.Remove(indexAtGrid, this);
			scene->phys->Remove(indexAtGrid);
			indexAtGrid = -1;
		}
	}
}

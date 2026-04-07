#include "pch.h"
#include "t5_.h"
#include "mm_scene.h"

namespace Test5 {

	void Scene::GenWallHorizontal(int32_t xFrom_, int32_t xTo_, int32_t y_, bool leftOverflow_, bool rightOverflow_) {
		for (int32_t x = xFrom_; x <= xTo_; ++x) {
			walls.Emplace().Emplace()->Init(this, XY{ x, y_ } * cCellPixelSize + cCellPixelHalfSize);
		}
		for (int32_t x = xFrom_; x < xTo_; ++x) {
			walls.Emplace().Emplace()->Init(this, XY{ x, y_ } * cCellPixelSize + XY{ cCellPixelSize, cCellPixelHalfSize });
		}
		if (leftOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ xFrom_, y_ } * cCellPixelSize + XY{ 0, cCellPixelHalfSize });
		}
		if (rightOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ xTo_, y_ } * cCellPixelSize + XY{ cCellPixelSize, cCellPixelHalfSize });
		}
	}

	void Scene::GenWallVertical(int32_t x_, int32_t yFrom_, int32_t yTo_, bool topOverflow_, bool bottomOverflow_) {
		for (int32_t y = yFrom_; y <= yTo_; ++y) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, y } * cCellPixelSize + cCellPixelHalfSize);
		}
		for (int32_t y = yFrom_; y < yTo_; ++y) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, y } * cCellPixelSize + XY{ cCellPixelHalfSize, cCellPixelSize });
		}
		if (topOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, yFrom_ } * cCellPixelSize + XY{ cCellPixelHalfSize, 0 });
		}
		if (bottomOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, yTo_ } * cCellPixelSize + XY{ cCellPixelHalfSize, cCellPixelSize });
		}
	}

	void Scene::GenDoorHorizontal(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		doors.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 }, false);
		doors.Emplace().Emplace()->Init(this, p + XY{ -cCellPixelHalfSize, 0 }, false);
		doors.Emplace().Emplace()->Init(this, p, true);
	}

	void Scene::GenDoorVertical(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		doors.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize }, false);
		doors.Emplace().Emplace()->Init(this, p + XY{ 0, -cCellPixelHalfSize }, false);
		doors.Emplace().Emplace()->Init(this, p, true);
	}

	void Scene::GenPlayer(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		players.Emplace().Emplace()->Init(this, p);
	}

	void Scene::GenBucket(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		buckets.Emplace().Emplace()->Init(this, p);
	}

}

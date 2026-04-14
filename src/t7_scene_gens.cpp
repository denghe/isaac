#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::GenMap() {
		mapSize = cRoom1x1PixelSize;
		cam.Init(gg.scale, gg.designSize.y / cRoom1x1PixelSize.y, mapSize / 2);
		sortContainer.Resize<true>((int32_t)cRoomMaxPixelSize.y);
		gridBuildings.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize)
			, std::ceilf(mapSize.x / cCellPixelSize));
		phys.Emplace()->Init(this);

		floorMaskTex.Emplace()->Make(mapSize);

		//                     1 1 1 1 1
		// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4
		// [][][][][][][]  [][][][][][][]	0
		// []                          []	1
		// []                          []	2
		// []                          []	3
		//                               	4
		// []                          []	5
		// []                          []	6
		// []                          []	7
		// [][][][][][][]  [][][][][][][]	8

		GenWallHorizontal(0, 6, 0);
		GenDoorHorizontal(7, 0);
		GenWallHorizontal(8, 14, 0);

		GenWallHorizontal(0, 6, 8);
		GenDoorHorizontal(7, 8);
		GenWallHorizontal(8, 14, 8);

		GenWallVertical(0, 1, 3, true, false);
		GenDoorVertical(0, 4);
		GenWallVertical(0, 5, 7, false, true);
		GenWallVertical(14, 1, 3, true, false);
		GenDoorVertical(14, 4);
		GenWallVertical(14, 5, 7, false, true);

		// todo: 创建门的时候 顺便配置 传送方式 地点啥的

		// 桶总数计数器
		auto bucketCounter = xx::MakeShared<void*>();

		// 生成桶，除了玩家所在的这个位置，每个位置都生成一个桶
		for (int x = 1; x <= 13; ++x) {
			for (int y = 1; y <= 7; ++y) {
				if (x == 7 && y == 4) continue;
				auto& o = GenBucket(x, y);
				// 注册桶被销毁时的回调，如果所有桶都被销毁了，就开门
				o->disposeCallbacks.Add([counter = bucketCounter, this]() {
					// 最后一个桶被销毁了，计数器的共享计数应该是 1. 执行开门逻辑
					if (counter.GetSharedCount() == 1) {
						OpenDoors();
					}
				});
			}
		}
	}


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
		player.Emplace()->Init(this, p);
	}

	xx::Shared<Bucket>& Scene::GenBucket(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		buckets.Emplace().Emplace()->Init(this, p);
		return buckets.Back();
	}

}

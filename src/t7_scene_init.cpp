#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::Init() {
		effectTexts.Init(&cam, 10000);
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;

		mapSize = cRoom1x1PixelSize;
		cam.Init(gg.scale, gg.designSize.y / cRoom1x1PixelSize.y, mapSize / 2);
		sortContainer.Resize<true>((int32_t)cRoomMaxPixelSize.y);
		gridBuildings.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize)
			, std::ceilf(mapSize.x / cCellPixelSize));
		phys.Emplace()->Init(this);

		frameBuffer.Init();
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

		GenPlayer(7, 4);

		
		// 桶总数计数器
		auto bucketCounter = xx::MakeShared<void*>();

		// 生成桶，除了玩家所在的这个位置，每个位置都生成一个桶
		for (int x = 1; x <= 13; ++x) {
			for (int y = 1; y <= 7; ++y) {
				if (x == 7 && y == 4) continue;
				auto& o = GenBucket(x, y);
				// 注册桶被销毁时的回调，如果所有桶都被销毁了，就执行开门逻辑
				o->disposeCallbacks.Add([counter = bucketCounter, this]() {
					// 最后一个桶被销毁了，计数器的共享计数应该是 1
					// （当前回调持有的这个共享指针），在这里执行开门逻辑
					if(counter.GetSharedCount() == 1) {
						// todo
						doors.Clear();
					}
				});
			}
		}
	}

}

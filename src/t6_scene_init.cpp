#include "pch.h"
#include "t6_.h"
#include "mm_scene.h"

namespace Test6 {

	void Scene::Init() {
		effectTexts.Init(&cam, 10000);
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;

		mapSize = cRoom2x2PixelSize;
		cam.Init(gg.scale, gg.designSize.x / cRoom1x1PixelSize.x, mapSize / 2);
		sortContainer.Resize<true>((int32_t)cRoomMaxPixelSize.y);
		gridBuildings.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize)
			, std::ceilf(mapSize.x / cCellPixelSize));
		phys.Emplace()->Init(this);

		frameBuffer.Init();
		floorMaskTex.Emplace()->Make(mapSize);

		//                     1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2
		// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 
		// [][][][][][][]  [][][][][][][][][][][][]  [][][][][][][]	0
		// []                                                    []	1
		// []                                                    []	2
		// []                                                    []	3
		//                                                         	4
		// []                                                    []	5
		// []                                                    []	6
		// []                                                    []	7
		// []                                                    []	8
		// []                                                    []	9
		// []                                                    []	10
		//                                                         	11
		// []                                                    []	12
		// []                                                    []	13
		// []                                                    []	14
		// [][][][][][][]  [][][][][][][][][][][][]  [][][][][][][]	15

		// 顶边
		GenWallHorizontal(0, 6, 0);
		GenDoorHorizontal(7, 0);
		GenWallHorizontal(8, 19, 0);
		GenDoorHorizontal(20, 0);
		GenWallHorizontal(21, 27, 0);
		// 底边
		GenWallHorizontal(0, 6, 15);
		GenDoorHorizontal(7, 15);
		GenWallHorizontal(8, 19, 15);
		GenDoorHorizontal(20, 15);
		GenWallHorizontal(21, 27, 15);
		// 左边
		GenWallVertical(0, 1, 3, true, false);
		GenDoorVertical(0, 4);
		GenWallVertical(0, 5, 10, false, false);
		GenDoorVertical(0, 11);
		GenWallVertical(0, 12, 14, false, true);
		// 右边
		GenWallVertical(27, 1, 3, true, false);
		GenDoorVertical(27, 4);
		GenWallVertical(27, 5, 10, false, false);
		GenDoorVertical(27, 11);
		GenWallVertical(27, 12, 14, false, true);

		GenPlayer(13, 7);

		for (int x = 1; x <= cRoom2x2ContentSize.x; ++x) {
			for (int y = 1; y <= cRoom2x2ContentSize.y; ++y) {
				if (x == 13 && y == 7) continue;
				GenBucket(x, y);
			}
		}
	}

}

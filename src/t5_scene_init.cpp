#include "pch.h"
#include "t5_.h"
#include "mm_scene.h"

namespace Test5 {

	void Scene::Init() {
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
		// [][][][][][][]  [][][][][][][] 0
		// []                          [] 1
		// []                          [] 2
		// []                          [] 3
		//                                4
		// []                          [] 5
		// []                          [] 6
		// []                          [] 7
		// [][][][][][][]  [][][][][][][] 8

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

		for (int x = 1; x <= 13; ++x) {
			for (int y = 1; y <= 7; ++y) {
				if (x == 7 && y == 4) continue;
				GenBucket(x, y);
			}
		}
	}

}

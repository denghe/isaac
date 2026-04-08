#pragma once
#include "game.h"

// 模拟 isaac 这个游戏的一小部分（物理，道具，技能啥的）

namespace Test5 {
	// 这里放置一些常量

	static constexpr float cUIScale{ 0.5f };

	static constexpr float cCellPixelSize{ 128 };
	static constexpr float cCellPixelHalfSize{ cCellPixelSize / 2.f };
	static constexpr float cWallRadius{ cCellPixelSize / 2.f };
	static constexpr float cBucketRadius{ cWallRadius * 0.5f };
	static constexpr float cPlayerRadius{ cWallRadius * 0.5f };
	static constexpr float cItemMaxRadius{ cBucketRadius };

	static constexpr XY cRoomMaxContentSize{ 26, 14 };	// logic
	static constexpr XY cRoomMaxTotalSize{ cRoomMaxContentSize + 2 };
	static constexpr XY cRoomMaxPixelSize{ cRoomMaxTotalSize * cCellPixelSize };

	static constexpr XY cRoom1x1ContentSize{ 13, 7 };
	static constexpr XY cRoom1x1TotalSize{ cRoom1x1ContentSize + 2 };
	static constexpr XY cRoom1x1PixelSize{ cRoom1x1TotalSize * cCellPixelSize };

	static constexpr XY cRoom1x2ContentSize{ 13, 14 };
	static constexpr XY cRoom1x2TotalSize{ cRoom1x2ContentSize + 2 };
	static constexpr XY cRoom1x2PixelSize{ cRoom1x2TotalSize * cCellPixelSize };

	static constexpr XY cRoom2x1ContentSize{ 26, 7 };
	static constexpr XY cRoom2x1TotalSize{ cRoom2x1ContentSize + 2 };
	static constexpr XY cRoom2x1PixelSize{ cRoom2x1TotalSize * cCellPixelSize };

	static constexpr XY cRoom2x2ContentSize{ 26, 14 };
	static constexpr XY cRoom2x2TotalSize{ cRoom2x2ContentSize + 2 };
	static constexpr XY cRoom2x2PixelSize{ cRoom2x2TotalSize * cCellPixelSize };
	// ...
}

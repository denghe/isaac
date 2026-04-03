#pragma once
#include "game.h"

// simulate isaac game scene

// todo: big map camera logic

namespace Test1 {
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

	// ...
}

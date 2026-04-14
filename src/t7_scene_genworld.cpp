#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::GenWorld() {
		// todo: 创建门的时候 顺便配置 传送方式 地点啥的
		// 这需要完整的大地图系统
		// 门并非导致玩家会远距离传送 玩家只是物理通过
		// camera 也只是逻辑上被临时限制在 一个房间的区域

		// 图元：　墙门玩
		std::u32string_view mapText = UR"(
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
墙　　　　　　　墙　　　　　　　墙
墙　　　玩　　　门　　　　　　　墙
墙　　　　　　　墙　　　　　　　墙
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
)";

		// 移除首行空换行
		while (true) {
			assert(!mapText.empty());
			if (auto c = mapText[0]; c == U'\r' || c == U'\n') {
				mapText = mapText.substr(1, mapText.size() - 1);
				continue;
			}
			break;
		}

		// 移除尾行空换行
		while (true) {
			assert(!mapText.empty());
			if (auto c = mapText.back(); c == U'\r' || c == U'\n') {
				mapText = mapText.substr(0, mapText.size() - 1);
				continue;
			}
			break;
		}

		// 计算总宽度
		int32_t mapWidth{};
		for (int i = 0, e = (int)mapText.size(); i < e; ++i) {
			if (auto c = mapText[i]; c == U'\r' || c == U'\n')
				break;
			++mapWidth;
		}

		// 计算总高度
		int32_t mapHeight{ 1 };
		int32_t x{};
		for (int i = 0, e = (int)mapText.size(); i < e; ++i) {
			switch (auto c = mapText[i]) {
			case U'\r':
				continue;
			case U'\n':
				assert(x == mapWidth);	// 确保每一行长度一致
				x = 0;
				++mapHeight;
				continue;
			}
			++x;
		}


		mapSize.x = cCellPixelSize * mapWidth;
		mapSize.y = cCellPixelSize * mapHeight;
		cam.Init(gg.scale, gg.designSize.y / cRoom1x1PixelSize.y, mapSize / 2);
		sortContainer.Resize<true>((int32_t)mapSize.y);
		gridBuildings.Init(cCellPixelSize, mapHeight, mapWidth);
		phys.Emplace()->Init(this);
		floorMaskTex.Emplace()->Make(mapSize);

		// 逐行扫内容并生成
		x = 0;
		int32_t y{};
		for (int i = 0, e = (int)mapText.size(); i < e; ++i) {
			switch (auto c = mapText[i]) {
			case U'\r':
				continue;
			case U'\n':
				x = 0;
				++y;
				continue;
			case U'墙':
			{
				// 计算出左上角坐标
				auto p = XY{ x, y } * cCellPixelSize;
				// 创建墙壁主体
				walls.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize);
				// 判断左边和上边如果也有墙，在中缝补一个墙增加密度
				if (x > 0 && mapText[i - 1] == U'墙') {
					walls.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize });
				}
				if (y > 0 && mapText[i - mapWidth - 1] == U'墙') {
					walls.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 });
				}
				// 判断左边和上边如果也有门，在中缝补一个门增加密度
				if (x > 0 && mapText[i - 1] == U'门') {
					doors.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize }, false);
				}
				if (y > 0 && mapText[i - mapWidth - 1] == U'门') {
					doors.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 }, false);
				}
				break;
			}
			case U'门':
			{
				// 计算出左上角坐标
				auto p = XY{ x, y } * cCellPixelSize;
				// 创建墙壁主体
				doors.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize, true);
				// 判断左边和上边如果也有墙，在中缝补一个门增加密度
				if (x > 0 && mapText[i - 1] == U'墙') {
					doors.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize }, false);
				}
				if (y > 0 && mapText[i - mapWidth - 1] == U'墙') {
					doors.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 }, false);
				}
				break;
			}
			case U'玩':
			{
				GenPlayer(x, y);
				break;
			}
			}
			++x;
		}

	}

}

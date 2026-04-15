#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::GenWorld() {
		// 多房间世界
		// 
		// todo: 
		// 使用带分类 bits 的 typeId( 方便用位操作直接检测对象类型 )
		// 根据 mapData 生成小地图底图, 每帧只需要叠加 怪 玩家 道具 等图例
		// 每帧检查玩家坐标, 判断玩家所在房间尺寸并限制 camera 范围
		// 玩家穿过门时 camera 动画转场 切换房间( 焦点转移 )
		// 
		// 怪 和 门的开关有绑定关系. 理论上讲 1 道门会关联 2 波怪
		// 如果 2 波怪( 门的两边 ) 均存在，门不会打开。
		// 只要消失掉一边，门就会打开

		// 图元：　墙门玩
		std::u32string_view mapText = UR"(
墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙墙
墙　　　　　　　墙　　　　　　　墙
墙　　　玩　　　门　　　　　　　墙
墙　　　　　　　墙　　　　　　　墙
墙墙墙墙墙墙墙墙墙墙墙墙门墙墙墙墙
墙　　　　　　　墙　　　　　　　墙
墙　　　　　　　门　　　　　　　墙
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

		// 扫文本 先翻译成对象类型编号二维数组
		mapData.Resize<true, 0>(mapWidth * mapHeight);	// 默认填 0 代表空格
		mapRoomMappings.Resize<true, -1>(mapWidth * mapHeight);

		// 准备一个和 mapData 同样大小的 flag 数组，用来记录已经探索过或无法探索的区域( 标 1 )
		xx::List<char> mapFlags;
		mapFlags.Resize<true, 0>(mapData.len);	// 默认填 0 表示未探索过 可以探索

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
				auto i = y * mapWidth + x;
				mapData[i] = Wall::cTypeId;
				mapFlags[i] = 1;
				break;
			}
			case U'门':
			{
				auto i = y * mapWidth + x;
				mapData[i] = Door::cTypeId;
				mapFlags[i] = 1;
				break;
			}
			case U'玩':
			{
				mapData[y * mapWidth + x] = Player::cTypeId;
				break;
			}
			}
			++x;
		}


		// 先扫出第一个 非空 非墙壁位置
		XYi firstPos{ -1 };
		for (int32_t y = 0; y < mapHeight; y++) {
			for (x = 0; x < mapWidth; x++) {
				auto i = y * mapWidth + x;
				if (mapData[i] != Wall::cTypeId) {
					firstPos = { x, y };
					goto LabEnd;
				}
			}
		}
		// 如果能执行到这里，说明地图数据有问题：未找出任何一个 非空 非墙壁位置
		assert(false);
	LabEnd:;

		// 从第一个非墙壁位置开始扫房间范围
		// 1. 逐个找出房间内所有格子下标( 递归逻辑 ), 顺便算出 AABB 包围盒范围
		// 2. 找出所有相邻的门
		// 3. 扫门的邻居，得到另一个房间的非墙壁位置开始扫房间范围( 递归逻辑 )

		// 用于存储有哪些起始点坐标( 扫门算出 )
		xx::List<XYi> firstPoss;

		// 添加原始起始点
		firstPoss.Emplace(firstPos);

		// 用于指向当前房间信息
		xx::FromTo<XYi>* aabb{};
		// 用于保存当前房间信息的下标值
		int32_t aabbIndex{};

		// 用于存储当前房间有哪些内容坐标
		xx::List<XYi> poss;

		// 尝试往 poss 添加一个内容坐标( 无效就忽略 ) 并同步当前房间信息
		auto PossTryAdd = [&](XYi p_) {
			if (p_.x < 0 || p_.x >= mapWidth || p_.y < 0 || p_.y >= mapHeight) return;
			auto i = p_.x + p_.y * mapWidth;
			if (mapFlags[i]) return;
			mapFlags[i] = 1;
			mapRoomMappings[i] = aabbIndex;
			poss.Add(p_);
			if (p_.x < aabb->from.x) aabb->from.x = p_.x;
			if (p_.y < aabb->from.y) aabb->from.y = p_.y;
			if (p_.x > aabb->to.x) aabb->to.x = p_.x;
			if (p_.y > aabb->to.y) aabb->to.y = p_.y;
		};

		// 尝试往 firstPoss 添加一个起始内容坐标( 门对面 )
		auto FirstPossTryAdd = [&](XYi currPos_, XYi doorOffsetPos_) {
			auto p_ = currPos_ + doorOffsetPos_;
			if (p_.x < 0 || p_.x >= mapWidth || p_.y < 0 || p_.y >= mapHeight) return;
			auto i = p_.x + p_.y * mapWidth;
			if (mapData[i] != Door::cTypeId) return;
			p_ += doorOffsetPos_;
			if (p_.x < 0 || p_.x >= mapWidth || p_.y < 0 || p_.y >= mapHeight) return;
			i = p_.x + p_.y * mapWidth;
			if (mapData[i] == Door::cTypeId || mapData[i] == Wall::cTypeId) return;
			if (mapFlags[i]) return;
			firstPoss.Add(p_);
		};

		// 递归遍历起始点, 创建房间信息
		for (int32_t j = 0; j < firstPoss.len; ++j) {
			poss.Clear();
			auto fp = firstPoss[j];
			aabbIndex = mapRooms.len;
			mapRooms.Emplace(xx::FromTo<XYi>{.from = fp, .to = fp });
			aabb = &mapRooms.Back();
			PossTryAdd(fp);
			// 递归遍历内容及其邻居
			for (int32_t idx = 0; idx < poss.len; ++idx) {
				auto curr = poss[idx];	// 复制, 避免扩容失效
				// 查找未被访问过的邻居( 上下左右 )，添加到 poss 尾( 接下来会被 for )
				PossTryAdd(curr + XYi{ 0, -1 });		// 上
				PossTryAdd(curr + XYi{ 0, 1 });			// 下
				PossTryAdd(curr + XYi{ -1, 0 });		// 左
				PossTryAdd(curr + XYi{ 1, 0 });			// 右
				// 查找门邻居, 通过门找到隔壁房间入口( 接下来会被 for )
				FirstPossTryAdd(curr, XYi{ 0, -1 });	// 上
				FirstPossTryAdd(curr, XYi{ 0, 1 });		// 下
				FirstPossTryAdd(curr, XYi{ -1, 0 });	// 左
				FirstPossTryAdd(curr, XYi{ 1, 0 });		// 右
			}
		}

		
		mapSize = { mapWidth, mapHeight };
		mapPixelSize = mapSize * cCellPixelSize;
		cam.Init(gg.scale, gg.designSize.y / cRoom1x1PixelSize.y, mapPixelSize / 2);
		sortContainer.Resize<true>((int32_t)mapPixelSize.y);
		gridBuildings.Init(cCellPixelSize, mapHeight, mapWidth);
		phys.Emplace()->Init(this);
		floorMaskTex.Emplace()->Make(mapPixelSize);

		// 逐行扫内容并生成
		for (int32_t y = 0; y < mapHeight; y++) {
			for (x = 0; x < mapWidth; x++) {
				auto i = y * mapWidth + x;
				switch (mapData[i]) {
				case Wall::cTypeId:
				{
					// 计算出左上角坐标
					auto p = XY{ x, y } * cCellPixelSize;
					// 创建墙壁主体
					walls.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize);
					// 判断左边和上边如果也有墙，在中缝补一个墙增加密度
					if (x > 0 && mapData[i - 1] == Wall::cTypeId) {
						walls.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize });
					}
					if (y > 0 && mapData[i - mapWidth] == Wall::cTypeId) {
						walls.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 });
					}
					// 判断左边和上边如果也有门，在中缝补一个门增加密度
					if (x > 0 && mapData[i - 1] == Door::cTypeId) {
						doors.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize }, false);
					}
					if (y > 0 && mapData[i - mapWidth] == Door::cTypeId) {
						doors.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 }, false);
					}
					break;
				}
				case Door::cTypeId:
				{
					// 计算出左上角坐标
					auto p = XY{ x, y } * cCellPixelSize;
					// 创建墙壁主体
					doors.Emplace().Emplace()->Init(this, p + cCellPixelHalfSize, true);
					// 判断左边和上边如果也有墙，在中缝补一个门增加密度
					if (x > 0 && mapData[i - 1] == Wall::cTypeId) {
						doors.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize }, false);
					}
					if (y > 0 && mapData[i - mapWidth] == Wall::cTypeId) {
						doors.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 }, false);
					}
					break;
				}
				case Player::cTypeId:
				{
					GenPlayer(x, y);
					break;
				}
				}
			}
		}

	}

}

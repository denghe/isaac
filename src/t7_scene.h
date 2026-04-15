#pragma once
#include "t7_dot.h"
#include <xx_grid2d_circle.h>

namespace Test7 {

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {
		// 地图逻辑格子尺寸
		XYi mapSize{};
		// 地图总像素尺寸
		XY mapPixelSize{};
		// 地图布局数据( 值为 类型::cTypeId )
		xx::List<int32_t> mapData;
		// 记录房间内容范围 AABB
		xx::List<xx::FromTo<XYi>> mapRooms;
		// 用于查询地图内容属于哪个房间( 下标 )
		xx::List<int32_t> mapRoomMappings;
		// 迷你地图背景贴图
		xx::Shared<xx::GLTexture> mapMiniBG;
		// todo: 记录玩家出生点坐标


		xx::Grid2dCircle<SceneItem*, GridCache> gridBuildings;	// for walls, doors
		xx::Shared<PhysSystem> phys;	// for players, buckets
		xx::List<xx::Shared<Wall>> walls;
		xx::List<xx::Shared<Door>> doors;
		xx::List<xx::Shared<Bucket>> buckets;
		xx::Shared<Player> player;
		xx::List<xx::Shared<Exploder>> exploders;
		xx::List<xx::Shared<PlayerBullet>> playerBullets;

		xx::FrameBuffer frameBuffer;
		xx::Shared<xx::GLTexture> floorMaskTex;
		xx::List<FloorMask> floorMasks;

		xx::EffectTextManager effectTexts;

		void GenWorld();
		void GenMap();
		void GenWallHorizontal(int32_t xFrom_, int32_t xTo_, int32_t y_, bool leftOverflow_ = false, bool rightOverflow_ = false);
		void GenWallVertical(int32_t x_, int32_t yFrom_, int32_t yTo_, bool topOverflow_ = false, bool bottomOverflow_ = false);
		void GenDoorHorizontal(int32_t x_, int32_t y_);
		void GenDoorVertical(int32_t x_, int32_t y_);
		void GenPlayer(int32_t x_, int32_t y_);
		xx::Shared<Bucket>& GenBucket(int32_t x_, int32_t y_);

		// 打开所有门
		void OpenDoors();

		//关闭所有门
		void CloseDoors();

		// 处理建筑重叠( 圆形判断，会挪移 pos 令重叠消失 )
		void HandleBuildingsCross(XY& pos_, float radius_);

		// 尝试获取 pos 附近相交( 圆心足够近 )的 door. 未找到或不够近返回 空
		Door* TryGetCrossDoor(XY const& pos_, float radius_);

		// 基础初始化 + 不转场 + 创建玩家
		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void OnResize(bool modeChanged_) override;

		// 基础初始化
		void Init_();

		// 基础初始化 + 转场，会有一个 camera 移动动画，player 从 last 场景 move
		void Init(Scene* last_);
	};

}

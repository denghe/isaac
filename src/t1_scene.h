#pragma once
#include "t1_sceneitem.h"
#include <xx_grid2d_circle.h>

namespace Test1 {

	struct Wall;
	struct Door;
	struct Bucket;
	struct Player;
	struct PhysSystem;

	struct GridCache {
		XY pos{};
		float radius{};
		void operator=(SceneItem* p) {
			pos = p->pos;
			radius = p->radius;
		}
	};

	using SceneBase = Global::SceneBase;
	struct Scene : SceneBase {
		XY mapSize{};
		xx::Grid2dCircle<SceneItem*, GridCache> gridBuildings;	// for walls, doors
		xx::Shared<PhysSystem> phys;	// for players, buckets
		xx::List<xx::Shared<Wall>> walls;
		xx::List<xx::Shared<Door>> doors;
		xx::List<xx::Shared<Bucket>> buckets;
		xx::List<xx::Shared<Player>> players;

		void GenWallHorizontal(int32_t xFrom_, int32_t xTo_, int32_t y_, bool leftOverflow_ = false, bool rightOverflow_ = false);
		void GenWallVertical(int32_t x_, int32_t yFrom_, int32_t yTo_, bool topOverflow_ = false, bool bottomOverflow_ = false);
		void GenDoorHorizontal(int32_t x_, int32_t y_);
		void GenDoorVertical(int32_t x_, int32_t y_);
		void GenPlayer(int32_t x_, int32_t y_);
		void GenBucket(int32_t x_, int32_t y_);

		void Init();
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void OnResize(bool modeChanged_) override;
	};

}

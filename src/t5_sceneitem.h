#pragma once
#include "g_scenebase.h"

namespace Test5 {

	struct Scene;
	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};
	};

	struct GridCache {
		XY pos{};
		float radius{};
		void operator=(SceneItem* p) {
			pos = p->pos;
			radius = p->radius;
		}
	};

	struct FloorMask {
		xx::Frame frame;
		XY pos{};
		float scale{ 1.f }, radians{}, colorplus{ 1.f };
		xx::RGBA8 color{ 0,0,0,127 };
	};

	// 一系列预声明就放在这以方便使用
	struct Wall;
	struct Door;
	struct Bucket;
	struct Exploder;
	struct Player;
	struct PlayerBullet;
	struct PhysSystem;
	// ...

}

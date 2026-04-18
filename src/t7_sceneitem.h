#pragma once
#include "g_scenebase.h"

namespace Test7 {

	struct Properties {
		int32_t hp{}, hpMax{};
	};

	struct Scene;
	struct Dot;
	struct SceneItem : Global::SceneItemBase {

		// 为方便使用，指向所在场景指针( 生命周期通常长于 item )
		Scene* scene{};

		// 如果 item 是具有 属性面板 的对象，则可以创建并填充这个成员
		xx::Shared<Properties> properties;	// todo: 挪到多重继承类

		// 用于注册回调函数
		xx::List<std::function<void()>> disposeCallbacks;

		// 用于 Dispose 时调用回调函数
		void CallDisposeCallbacks();

		// 绘制血条的功能函数
		virtual void DrawHPBar() {};
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

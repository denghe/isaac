#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Door::Init(Scene* scene_, XY pos_, bool isCenter_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cWallRadius;
		scale = radius * 2.f / gg.pics.c128_wall.uvRect.w;
		radians = {};
		isCenter = isCenter_;

		indexAtContainer = scene->doors.len - 1;
		assert(scene->doors[indexAtContainer].pointer == this);

		scene->gridBuildings.Add(indexAtGrid, this);
	}

	void Door::Draw() {
		// 在颜色上体现 open 状态
		auto color = xx::RGBA8_White;
		if (isOpened) {
			color = {55,55,55,55};
		}
		gg.Quad().DrawFrame(gg.pics.c128_door, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, 1, color);
	}

	void Door::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->doors;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		if (indexAtGrid > -1) {
			scene->gridBuildings.Remove(indexAtGrid, this);
		}

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

	void Door::Open() {
		assert(isOpened == false);
		isOpened = true;
	}

	void Door::Close() {
		assert(isOpened);
		isOpened = false;
	}

	void Door::Visit(SceneItem* tar_) {
		// 各对象进门事件处理
		switch (tar_->typeId) {
		case Player::cTypeId: {
			// todo: 改玩家坐标
			// 切换到新场景
			gg.MakeScene<Scene>()->Init(xx::SharedFromThis(scene));
			return;
		}
		// ...
		}
	}

}

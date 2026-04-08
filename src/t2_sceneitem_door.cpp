#include "pch.h"
#include "t2_.h"

namespace Test2 {

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
		gg.Quad().DrawFrame(gg.pics.c128_door, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
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

}

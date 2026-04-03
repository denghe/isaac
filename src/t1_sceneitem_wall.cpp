#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Wall::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cWallRadius;
		scale = radius * 2.f / gg.pics.cell_wall.uvRect.w;
		radians = {};

		indexAtContainer = scene_->walls.len - 1;
		assert(scene_->walls[indexAtContainer].pointer == this);

		scene_->gridBuildings.Add(indexAtGrid, this);
	}

	void Wall::Draw() {
		gg.Quad().DrawFrame(gg.pics.cell_wall, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Wall::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->walls;
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

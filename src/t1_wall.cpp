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
		auto scene = (Scene*)this->scene;
		auto i = indexAtContainer;
		assert(scene->walls[i].pointer == this);
		scene->walls.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->walls.SwapRemoveAt(i);
	}

	Wall::~Wall() {
		auto scene = (Scene*)this->scene;
		if (indexAtGrid > -1) {
			scene->gridBuildings.Remove(indexAtGrid, this);
			indexAtGrid = -1;
		}
	}

}

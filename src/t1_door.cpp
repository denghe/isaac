#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Door::Init(Scene* scene_, XY pos_, bool isCenter_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cWallRadius;
		scale = radius * 2.f / gg.pics.cell_wall.uvRect.w;
		radians = {};
		isCenter = isCenter_;
		indexAtContainer = scene->doors.len - 1;
		assert(scene->doors[indexAtContainer].pointer == this);
		scene->gridBuildings.Add(indexAtGrid, this);
	}

	void Door::Draw() {
		gg.Quad().DrawFrame(gg.pics.cell_door, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Door::Dispose() {
		auto i = indexAtContainer;
		assert(scene->doors[i].pointer == this);
		scene->doors.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->doors.SwapRemoveAt(i);
	}

	Door::~Door() {
		if (indexAtGrid > -1) {
			scene->gridBuildings.Remove(indexAtGrid, this);
		}
	}

}

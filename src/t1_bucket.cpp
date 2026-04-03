#include "pch.h"
#include "t1_.h"

namespace Test1 {

	void Bucket::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cBucketRadius;
		scale = radius * 2.f / gg.pics.cell_bucket.uvRect.w;
		radians = {};
		indexAtContainer = scene->buckets.len - 1;
		assert(scene->buckets[indexAtContainer].pointer == this);
		//scene->gridItems.Add(indexAtGrid, this);
		indexAtGrid = scene->phys->Add(this);
	}

	void Bucket::Update() {
		// todo: logic
	}

	void Bucket::Draw() {
		gg.Quad().DrawFrame(gg.pics.cell_bucket, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Bucket::Dispose() {
		auto i = indexAtContainer;
		assert(scene->buckets[i].pointer == this);
		scene->buckets.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->buckets.SwapRemoveAt(i);
	}

	Bucket::~Bucket() {
		if (indexAtGrid > -1) {
			//scene->gridItems.Remove(indexAtGrid, this);
			scene->phys->Remove(indexAtGrid);
			indexAtGrid = -1;
		}
	}

}

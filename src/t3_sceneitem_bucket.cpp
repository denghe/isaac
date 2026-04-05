#include "pch.h"
#include "t3_.h"

namespace Test3 {

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

		scene->phys->Add(this);
	}

	void Bucket::Update() {
		// todo: 延迟爆炸逻辑( 如果受伤 )
	}

	void Bucket::Draw() {
		gg.Quad().DrawFrame(gg.pics.cell_bucket, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Bucket::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->buckets;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源
		scene->phys->Remove(this);

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

	void Bucket::Explode() {
		scene->exploders.Emplace().Emplace()->Init(this);
		Dispose();
	}
}

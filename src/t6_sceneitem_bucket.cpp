#include "pch.h"
#include "t6_.h"

namespace Test6 {

	void Bucket::Init(Scene* scene_, XY pos_) {
		typeId = cTypeId;
		scene = scene_;
		pos = pos_;
		y = pos.y;
		radius = cBucketRadius;
		scale = radius * 2.f / gg.pics.c128_bucket.uvRect.w;
		radians = {};

		indexAtContainer = scene->buckets.len - 1;
		assert(scene->buckets[indexAtContainer].pointer == this);

		scene->phys->Add(this);

		// 创建数据面板并初始化
		properties.Emplace();
		properties->hp = 100;
		properties->hpMax = 100;
	}

	void Bucket::Update() {
		// 驱动 dot, 减血到 <=0 时爆炸
		UpdateDots(this);
		if (properties->hp <= 0) {
			Explode();
		}
	}

	void Bucket::Draw() {
		gg.Quad().DrawFrame(gg.pics.c128_bucket, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void Bucket::DrawHPBar() {
		// 显示触发条件：带属性, 非满血
		if (properties->hp == properties->hpMax) return;
		auto percent = (float)properties->hp / properties->hpMax;
#if 0
		char buf[32];
		auto sv = xx::ToStringView(properties->hp, buf);
		auto p = pos + XY{ -55, -40 };
		gg.HPBar().Alloc()->Fill(scene->cam.ToGLPos(p), scene->cam.scale, xx::RGBA8_Red, percent, sv);
#else
		auto p = pos + XY{ -25, -40 };
		auto siz = XY{ 50, 9 } * scene->cam.scale;
		if (siz.x < 10) siz.x = 10;
		if (siz.y < 4) siz.y = 4;
		gg.HPBar().Alloc()->Fill(scene->cam.ToGLPos(p), siz, xx::RGBA8_Black, xx::RGBA8_White, xx::RGBA8_Red, percent);
#endif
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

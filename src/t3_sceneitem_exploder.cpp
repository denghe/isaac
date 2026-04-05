#include "pch.h"
#include "t3_.h"

namespace Test3 {

	void Exploder::Init(Bucket* bucket_) {
		typeId = cTypeId;
		scene = bucket_->scene;
		pos = bucket_->pos;
		y = bucket_->y;
		radius = bucket_->radius;
		scale = bucket_->scale;
		radians = bucket_->radians;

		indexAtContainer = scene->exploders.len - 1;
		assert(scene->exploders[indexAtContainer].pointer == this);
	}

	void Exploder::Update() {
		XX_BEGIN(_1);
		numSteps = (int32_t)std::ceilf(cExplodeDuration * gg.cFps);
		inc = (cExplodeRadius - radius) / numSteps;
		for (; numSteps > 0; --numSteps) {
			// 爆炸检测，级联引爆
			auto cri = scene->phys->PosToCRIndex(pos);
			scene->phys->ForeachByRange(cri.y, cri.x, radius + 64.f, gg.sgrdd, [&](PhysSystem::Node& o, float range)->void {
				if (o.value->typeId == Bucket::cTypeId) {
					auto d = o.cache.pos - pos;
					auto mag2 = d.x * d.x + d.y * d.y;
					auto r = o.cache.radius + radius;
					auto rr = r * r;
					if (mag2 < rr) {
						((Bucket*)o.value)->Explode();
					}
				}
			});
			XX_YIELD(_1);
			radius += inc;
			scale = radius * 2.f / gg.pics.cell_bucket.uvRect.w;
		}
		numSteps = (int32_t)std::ceilf(cFadeoutDuration * gg.cFps);
		inc = 1.f / numSteps;
		for (; numSteps > 0; --numSteps) {
			XX_YIELD(_1);
			alpha -= inc;
		}
		Dispose();
		XX_END(_1);
	}

	void Exploder::Draw() {
		auto c = (uint8_t)(255 * alpha);
		gg.Quad().DrawFrame(gg.pics.cell_bucket, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, 1, {c,c,c,c});
	}

	void Exploder::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->exploders;
		assert(container[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		container.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		container.SwapRemoveAt(i);
	}

}

#include "pch.h"
#include "t2_sceneitem_bucket.h"
#include "t2_sceneitem_exploder.h"

namespace Test2 {

	void Exploder::Init(Bucket* bucket_) {
		typeId = cTypeId;
		scene = bucket_->scene;
		pos = bucket_->pos;
		y = bucket_->y;
		radius = bucket_->radius;
		scale = bucket_->scale;
		radians = bucket_->radians;

		indexAtContainer = scene->exploders.len;
	}

	void Exploder::Update() {
		XX_BEGIN(_1);
		numSteps = (int32_t)std::ceilf(cExplodeDuration * gg.cFps);
		{
			auto targetScale = cExplodeRadius * 2.f / gg.pics.cell_bucket.uvRect.w;
			assert(targetScale > scale);
			inc = (targetScale - scale) / numSteps;
		}
		for (; numSteps > 0; --numSteps) {
			// todo: 爆炸检测
			XX_YIELD(_1);
			scale += inc;
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
		gg.Quad().DrawFrame(gg.pics.cell_bucket, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians, 1, {255,255,255, (uint8_t)(255*alpha)});
	}

	void Exploder::Dispose() {
		assert(scene);
		// 从容器中移除对象( 释放内存 )
		scene->exploders.SwapRemoveAt(indexAtContainer);
	}

}

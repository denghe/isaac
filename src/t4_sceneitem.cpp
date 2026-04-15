#include "pch.h"
#include "t4_sceneitem.h"

namespace Test4 {

	void SceneItem::SceneItemInit(int32_t typeId_, Scene* scene_, XY pos_, float radius_) {
		scene = scene_;
		typeId = typeId_;
		indexAtContainer = scene->items.len - 1;
		assert(scene_->items[indexAtContainer].pointer == this);
		pos = FixPosition(pos_);
		radius = radius_;
	}

	void SceneItem::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		assert(scene->items[indexAtContainer].pointer == this);

		// 设置标记
		disposing = true;

		// 进一步释放资源，事件逻辑
		OnDispose();

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		scene->items.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->items.SwapRemoveAt(i);	// unsafe: release memory
	}

	XY SceneItem::FixPosition(XY pos_) {
		// 地图边缘检测 回卷
		auto ms = scene->mapPixelSize;
		assert(pos_.x > -ms.x && pos_.x < ms.x * 2);
		if (pos_.x < 0) pos_.x += ms.x;
		else if (pos_.x >= ms.x) pos_.x -= ms.x;
		assert(pos_.y > -ms.y && pos_.y < ms.y * 2);
		if (pos_.y < 0) pos_.y += ms.y;
		else if (pos_.y >= ms.y) pos_.y -= ms.y;
		if (pos_.x == ms.x) pos_.x = 0.f;
		if (pos_.y == ms.y) pos_.y = 0.f;
		return pos_;
	}

	void SceneItem::SetPosition(XY pos_) {
		if (pos_ == pos) return;
		pos = FixPosition(pos_);
	}

}

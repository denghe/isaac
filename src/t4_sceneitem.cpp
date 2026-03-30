#include "pch.h"
#include "t4_sceneitem.h"

namespace Test4 {

	void SceneItem::SceneItemInit(int32_t typeId_, Scene* scene_, XY pos_, float radius_) {
		scene = scene_;
		typeId = typeId_;
		indexAtContainer = scene->items.len - 1;
		assert(scene_->items[indexAtContainer].pointer == this);
		pos = pos_;
		radius = radius_;
		scene_->itemsGrid.Add(indexAtGrid, this);
	}

	void SceneItem::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		assert(scene->items[indexAtContainer].pointer == this);
		assert(scene->itemsGrid.ValueAt(indexAtGrid) == this);

		// 设置标记
		disposing = true;

		// 从 grid 中移除对象，避免被查询到
		if (indexAtGrid != -1) {
			scene->itemsGrid.Remove(indexAtGrid, this);
		}

		// 进一步释放资源，事件逻辑
		OnDispose();

		// 从容器中移除对象( 释放内存 )
		auto i = indexAtContainer;
		scene->items.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->items.SwapRemoveAt(i);	// unsafe: release memory
	}

}

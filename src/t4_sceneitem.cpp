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

	XY SceneItem::FixPosition(XY pos_) {
		// 地图边缘检测 回卷
		auto ms = scene->mapSize;
		assert(pos_.x > -ms.x && pos_.x < ms.x * 2);
		if (pos_.x < 0) pos_.x += ms.x;
		else if (pos_.x >= ms.x) pos_.x -= ms.x;
		assert(pos_.y > -ms.y && pos_.y < ms.y * 2);
		if (pos_.y < 0) pos_.y += ms.y;
		else if (pos_.y >= ms.y) pos_.y -= ms.y;
		return pos_;
	}

	void SceneItem::SetPosition(XY pos_) {
		if (pos_ == pos) return;
		pos = FixPosition(pos_);
		// 新的 pos 值同步到空间索引 grid
		scene->itemsGrid.Update(indexAtGrid, this);
	}

	//// 碰撞检测示例
	//void SceneItem::RangeSearch() {
	//	auto cri = scene->itemsGrid.PosToCRIndex(pos);
	//	static constexpr float cCollectRange{ cFactoryRadius * 2 };
	//	scene->itemsGrid.ForeachByRange(cri.y, cri.x, cCollectRange + 64, gg.sgrdd, [&](decltype(scene->gridMaterials)::Node& node, float range)
	//		->void {
	//			auto d = pos - node.cache.pos;
	//			auto mag2 = d.x * d.x + d.y * d.y;
	//			static constexpr float rr = cCollectRange * cCollectRange;
	//			if (mag2 >= rr || mag2 <= 0.0001f) return;	// not cross?
	//			flyingWoods.Emplace().Emplace()->Init(this, node.value);
	//			node.value->Dispose();
	//		});
	//}
}

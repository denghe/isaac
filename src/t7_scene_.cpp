#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::OpenDoors() {
		for(auto& o : doors) o->Open();
	}

	void Scene::CloseDoors() {
		for(auto& o : doors) o->Close();
	}

	void Scene::HandleBuildingsCross(XY& pos_, float radius_) {
		// 遍历 item 邻居建筑处理碰撞( 直接修改 pos_ 位置将其移到建筑范围外 )
		using G = decltype(gridBuildings);
		auto& g = gridBuildings;
		auto cri = g.PosToCRIndex(pos_);
		g.ForeachBy9(cri.y, cri.x, [&](G::Node& node, float range)->void {
			// 忽略打开的门
			if (node.value->typeId == Door::cTypeId && node.value->isOpened) return;
			auto d = pos_ - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = node.cache.radius + radius_;
			auto rr = r * r;
			// 相交 但没有完全重叠
			if (mag2 < rr && mag2 > 0.0001f) {
				auto mag = std::sqrtf(mag2);
				auto norm = d / mag;
				pos_ = node.cache.pos + norm * r;
			}
		});
	}

	Door* Scene::TryGetCrossDoor(XY const& pos_, float radius_) {
		// 遍历 主门 判断 pos_ 是否离中心点足够近
		using G = decltype(gridBuildings);
		auto& g = gridBuildings;
		auto cri = g.PosToCRIndex(pos_);
		Door* rtv{};
		g.ForeachBy9Break(cri.y, cri.x, [&](G::Node& node, float range)->bool {
			// 忽略非门 非打开 非主
			if (node.value->typeId != Door::cTypeId 
				|| !node.value->isOpened
				|| !node.value->isCenter
				) return false;
			auto d = pos_ - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = /*node.cache.radius + */radius_;
			auto rr = r * r;
			// 如果 pos_ 足够接近圆心
			if (mag2 < rr) {
				rtv = (Door*)node.value;
				return true;
			}
			return false;
		});
		return rtv;
	}
}

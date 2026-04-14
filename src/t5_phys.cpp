#include "pch.h"
#include "t5_.h"

namespace Test5 {

	void PhysCache::operator=(SceneItem* p) {
		lastPos = pos = p->pos;
		accel = {};
		radius = p->radius;
	}


	void PhysSystem::Init(Scene* scene_, int32_t capacity_) {
		scene = scene_;
		pixelSize = scene->gridBuildings.pixelSize;
		numRows = std::ceilf(pixelSize.y / cCellPixelSize);
		numCols = std::ceilf(pixelSize.x / cCellPixelSize);
		Base::Init(cCellPixelSize, numRows, numCols, capacity_);
	}

	void PhysSystem::Add(SceneItem* item_) {
		return Base::Add(item_->indexAtGrid, item_);
	}

	void PhysSystem::Remove(SceneItem* item_) {
		return Base::Remove(item_->indexAtGrid, item_);
	}

	void PhysSystem::TryRemove(SceneItem* item_) {
		if (item_->indexAtGrid >= 0) {
			Remove(item_);
		}
	}

	PhysSystem::Node& PhysSystem::At(SceneItem* item_) const {
		return NodeAt(item_->indexAtGrid);
	}

	void PhysSystem::Step() {
		Calc();
		Writeback();
	}

	void PhysSystem::Calc() {
		for (int32_t bi = 0; bi < numCells; ++bi) {
			auto& b1 = buckets[bi];
			if (b1 < 0) continue;
			// 9格检测，当前格子和周围8格，越界的格子会被丢弃( 不参与碰撞检测 )
			if (auto bi2 = bi - 1; bi2 >= 0) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto b2 = buckets[bi]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi + 1; bi2 < numCells) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi + numCols - 1; bi2 < numCells) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi + numCols; bi2 < numCells) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi + numCols + 1; bi2 < numCells) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi - numCols - 1; bi2 >= 0) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi - numCols; bi2 >= 0) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
			if (auto bi2 = bi - numCols + 1; bi2 >= 0) if (auto b2 = buckets[bi2]; b2 >= 0) CalcBB(b1, b2);
		}
	}

	void PhysSystem::CalcBB(int32_t b1_, int32_t b2_) {
		assert(b1_ >= 0 && b1_ < count);
		assert(b2_ >= 0 && b2_ < count);
		// 两个桶内所有节点两两检测
		// 检测次数限制变量
		static constexpr int32_t cMaxChecksPerBucket{ 3 };
		int32_t n1{}, n2{};
		do {
			n2 = 0;
			auto b2 = b2_;
			do {
				CalcNN(nodes[b1_].cache, nodes[b2].cache);

				if (++n2 > cMaxChecksPerBucket) break;
				b2 = nodes[b2].next;
			} while (b2 >= 0);

			if (++n1 > cMaxChecksPerBucket) break;
			b1_ = nodes[b1_].next;
		} while (b1_ >= 0);
	}

	void PhysSystem::CalcNN(PhysCache& d1_, PhysCache& d2_) {
		// 距离计算
		auto d = d1_.pos - d2_.pos;
		auto mag2 = d.x * d.x + d.y * d.y;
		auto r = d1_.radius + d2_.radius;
		auto rr = r * r;
		// 没有相交
		if (mag2 >= rr) return;
		// 相交了, 计算移动量
		XY v;
		// 如果两个圆心几乎重叠，随机一个方向弹开( 使用最大速度 )
		if (mag2 <= 0.0001f) {
			if (&d1_ == &d2_) return;	// 相同节点, 不处理
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			XY cossin{ std::cosf(radians), std::sinf(radians) };
			v = cossin * cMaxSpeed;
		}
		else {
			auto mag = std::sqrtf(mag2);
			auto a = cResponseCoef * (r - mag);
			v = d / mag * a;
			// 移动量最大速度限制，避免物体弹得过快地图越界
			if (v.x > cMaxSpeed) v.x = cMaxSpeed;
			else if (v.x < -cMaxSpeed) v.x = -cMaxSpeed;
			if (v.y > cMaxSpeed) v.y = cMaxSpeed;
			else if (v.y < -cMaxSpeed) v.y = -cMaxSpeed;
		}
		// 将移动量加到两个物体上，一加一减保证作用力和反作用力相等
		d1_.pos += v;
		d2_.pos -= v;
	}

	void PhysSystem::Writeback() {
		for (int32_t ni = 0; ni < count; ++ni) {
			auto& o = nodes[ni];
			if (o.bucketsIndex < 0) continue;

			// 附加重力加速度
			o.cache.accel += cGravity;

			// 通过两个坐标来算移动增量
			auto inc = o.cache.pos - o.cache.lastPos;
			// 计算阻尼，阻尼系数越大，速度衰减越快。玩家的阻尼系数更大，当停止方向控制时，玩家会更快停下来
			auto vd = cVelocityDamping;
			if (o.value->typeId == Player::cTypeId) {
				vd = cVelocityDamping * 3.f;
			}
			// 将 加速度, 阻尼 按 1 帧的运行时长 应用到移动增量上
			inc = inc + (o.cache.accel - inc * vd) * (gg.cDelta * gg.cDelta);

			// 简单限制最大速度，避免物体弹得过快地图越界
#if 0
			if (inc.x * inc.x + inc.y * inc.y > cMaxSpeed * cMaxSpeed) {
				auto mag = std::sqrtf(inc.x * inc.x + inc.y * inc.y);
				inc = inc / mag * cMaxSpeed;
			}
#else
			// 这个写法不严谨, 但实际执行效果看上去正确，性能更好
			if (inc.x > cMaxSpeed) inc.x = cMaxSpeed;
			else if (inc.x < -cMaxSpeed) inc.x = -cMaxSpeed;
			if (inc.y > cMaxSpeed) inc.y = cMaxSpeed;
			else if (inc.y < -cMaxSpeed) inc.y = -cMaxSpeed;
#endif
			// 更新位置，重置加速度
			o.cache.lastPos = o.cache.pos;
			o.cache.pos = o.cache.pos + inc;
			o.cache.accel = {};

			// 遍历 item 邻居建筑处理碰撞( 直接修改 item 位置将其移到建筑范围外 )
			using G = decltype(scene->gridBuildings);
			auto& g = scene->gridBuildings;
			auto cri = g.PosToCRIndex(o.cache.pos);
			g.ForeachBy9(cri.y, cri.x, [&](G::Node& node, float range)->void {
				auto d = o.cache.pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = node.cache.radius + o.cache.radius;
				auto rr = r * r;
				// 相交 但没有完全重叠
				if (mag2 < rr && mag2 > 0.0001f) {
					auto mag = std::sqrtf(mag2);
					auto norm = d / mag;
					o.cache.pos = node.cache.pos + norm * r;
				}
				});

			// 更新数据
			cri = PosToCRIndex(o.cache.pos);
			Base::Base::Update(ni, cri.y, cri.x);
			if (o.value->pos != o.cache.pos) {
				o.value->pos = o.cache.pos;
				o.value->y = o.cache.pos.y;
			}
		}
	}

}

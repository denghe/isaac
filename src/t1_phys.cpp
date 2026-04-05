#include "pch.h"
#include "t1_.h"

namespace Test1 {

	PhysSystem::Node& PhysSystem::At(SceneItem* item_) const {
		return (Node&)nodes[item_->indexAtGrid];
	}

	void PhysSystem::Init(Scene* scene_, int32_t capacity_) {
		assert(nodes.Empty() && !buckets && capacity_ >= 0);
		scene = scene_;
		pixelSize = scene->gridBuildings.pixelSize;
		numRows = std::ceilf(pixelSize.y / cCellPixelSize);
		numCols = std::ceilf(pixelSize.x / cCellPixelSize);
		bucketsLen = numRows * numCols;
		buckets = std::make_unique_for_overwrite<Bucket[]>(bucketsLen);
		nodes.Reserve(capacity_);
	}

	void PhysSystem::Add(SceneItem* item_) {
		assert(buckets);
		assert(item_->indexAtGrid < 0);
		assert(item_->pos.x >= 0.f && item_->pos.x < pixelSize.x);
		assert(item_->pos.y >= 0.f && item_->pos.y < pixelSize.y);
		assert(item_->radius > 0.f);

		item_->indexAtGrid = nodes.len;
		nodes.Emplace(Node{
			.pos = item_->pos,
			.lastPos = item_->pos,
			.accel = 0,
			.radius = item_->radius,
			.item = item_
		});
	}

	void PhysSystem::Remove(SceneItem* item_) {
		assert(buckets);
		assert(nodes[item_->indexAtGrid].item = item_);

		auto i = item_->indexAtGrid;
		nodes.Back().item->indexAtGrid = i;
		item_->indexAtGrid = -1;
		nodes.SwapRemoveAt(i);
	}

	void PhysSystem::TryRemove(SceneItem* item_) {
		if (item_->indexAtGrid >= 0) {
			Remove(item_);
		}
	}

	void PhysSystem::Step() {
		FillBuckets();
		Calc();
		Writeback();
	}

	void PhysSystem::FillBuckets() {
		assert(buckets);
		// 清空桶
		for (int32_t i = 0; i < bucketsLen; ++i) {
			buckets[i].len = 0;
		}
		// 把节点下标放入桶: 根据节点位置计算桶索引
		// 只能放有限数量的节点，超过会被丢弃( 不参与碰撞检测 )
		for (int32_t len = nodes.len, i = 0; i < len; ++i) {
			auto p = (nodes[i].pos * _1_cellSize).As<int32_t>();
			assert(p.x >= 0 && p.x < numCols && p.y >= 0 && p.y < numRows);
			auto& b = buckets[p.x * numRows + p.y];
			if (b.len < b.indexAtNodess.size()) {
				b.indexAtNodess[b.len++] = i;
			}
		}
	}

	void PhysSystem::Calc() {
		for (int32_t bi = 0; bi < bucketsLen; ++bi) {
			auto& b = buckets[bi];
			if (!b.len) continue;
			// 9格检测，当前格子和周围8格，越界的格子会被丢弃( 不参与碰撞检测 )
			CalcBB(b, buckets[bi - 1]);
			CalcBB(b, buckets[bi]);
			CalcBB(b, buckets[bi + 1]);
			CalcBB(b, buckets[bi + numRows - 1]);
			CalcBB(b, buckets[bi + numRows]);
			CalcBB(b, buckets[bi + numRows + 1]);
			CalcBB(b, buckets[bi - numRows - 1]);
			CalcBB(b, buckets[bi - numRows]);
			CalcBB(b, buckets[bi - numRows + 1]);
		}
	}

	void PhysSystem::CalcBB(Bucket& b1_, Bucket& b2_) {
		// 桶内所有节点两两检测
		for (int32_t di1 = 0; di1 < b1_.len; ++di1) {
			for (int32_t di2 = 0; di2 < b2_.len; ++di2) {
				CalcNN(nodes[b1_.indexAtNodess[di1]], nodes[b2_.indexAtNodess[di2]]);
			}
		}
	}

	void PhysSystem::CalcNN(Node& d1_, Node& d2_) {
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
		for (int32_t i = 0, len = nodes.len; i < len; ++i) {
			auto& o = nodes[i];

			// 附加重力加速度
			o.accel += cGravity;

			// 通过两个坐标来算移动增量
			auto inc = o.pos - o.lastPos;
			// 计算阻尼，阻尼系数越大，速度衰减越快。玩家的阻尼系数更大，当停止方向控制时，玩家会更快停下来
			auto vd = cVelocityDamping;
			if (o.item->typeId == Player::cTypeId) {
				vd = cVelocityDamping * 3.f;
			}
			// 将 加速度, 阻尼 按 1 帧的运行时长 应用到移动增量上
			inc = inc + (o.accel - inc * vd) * (gg.cDelta * gg.cDelta);

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
			o.lastPos = o.pos;
			o.pos = o.pos + inc;
			o.accel = {};

			// 遍历 item 邻居建筑处理碰撞( 直接修改 item 位置将其移到建筑范围外 )
			using G = decltype(scene->gridBuildings);
			auto& g = scene->gridBuildings;
			auto cri = g.PosToCRIndex(o.pos);
			g.ForeachBy9(cri.y, cri.x, [&](G::Node& node, float range)->void {
				auto d = o.pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = node.cache.radius + o.radius;
				auto rr = r * r;
				// 相交 但没有完全重叠
				if (mag2 < rr && mag2 > 0.0001f) {
					auto mag = std::sqrtf(mag2);
					auto norm = d / mag;
					o.pos = node.cache.pos + norm * r;
				}
			});

			// 回写数据
			if (o.item->pos != o.pos) {
				o.item->pos = o.pos;
				o.item->y = o.pos.y;
			}
		}
	}

}

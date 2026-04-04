#include "pch.h"
#include "t1_.h"

namespace Test1 {

	PhysSystem::Node& PhysSystem::At(int32_t indexAtNodes_) const {
		assert(indexAtNodes_ >= 0 && indexAtNodes_ < count);
		return (Node&)nodes[indexAtNodes_];
	}

	void PhysSystem::Init(Scene* scene_, int32_t capacity_) {
		assert(!nodes && !buckets && capacity_ >= 0);
		scene = scene_;
		pixelSize = scene->gridBuildings.pixelSize;

		numRows = std::ceilf(pixelSize.y / cCellPixelSize);
		numCols = std::ceilf(pixelSize.x / cCellPixelSize);

		bucketsLen = numRows * numCols;
		nodes.Reserve(capacity_);
		buckets = std::make_unique_for_overwrite<Bucket[]>(bucketsLen);
	}

	void PhysSystem::Add(SceneItem* item_) {
		assert(buckets);
		assert(item_->indexAtGrid < 0);
		assert(item_->pos.x >= 0.f && item_->pos.x < pixelSize.x);
		assert(item_->pos.y >= 0.f && item_->pos.y < pixelSize.y);
		assert(item_->radius > 0.f);

		item_->indexAtGrid = nodes.len;
		auto& node = nodes.Emplace(Node{
			.pos = item_->pos,
			.lastPos = item_->pos,
			.acc = 0,
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

	void PhysSystem::Fill() {
		assert(buckets);
		for (int32_t i = 0; i < bucketsLen; ++i) {
			buckets[i].len = 0;
		}
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
		for (int32_t di1 = 0; di1 < b1_.len; ++di1) {
			for (int32_t di2 = 0; di2 < b2_.len; ++di2) {
				CalcNN(nodes[b1_.indexAtNodess[di1]], nodes[b2_.indexAtNodess[di2]]);
			}
		}
	}

	void PhysSystem::CalcNN(Node& d1_, Node& d2_) {
		auto d = d1_.pos - d2_.pos;
		auto mag2 = d.x * d.x + d.y * d.y;
		auto r = d1_.radius + d2_.radius;
		auto rr = r * r;
		if (mag2 >= rr) return;
		XY spd;
		if (mag2 <= eps) {
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			XY cossin{ std::cosf(radians), std::sinf(radians) };
			spd = cossin * cMaxSpeed;
		}
		else {
			auto mag = std::sqrtf(mag2);
			auto a = cResponseCoef * (r - mag);
			spd = d / mag * a;
			if (spd.x > cMaxSpeed) spd.x = cMaxSpeed;
			else if (spd.x < -cMaxSpeed) spd.x = -cMaxSpeed;
			if (spd.y > cMaxSpeed) spd.y = cMaxSpeed;
			else if (spd.y < -cMaxSpeed) spd.y = -cMaxSpeed;
		}
		d1_.pos += spd;
		d2_.pos -= spd;
	}

	void PhysSystem::Writeback() {
		for (int32_t i = 0, len = nodes.len; i < len; ++i) {
			auto& o = nodes[i];
			o.acc += cGravity;

#if 0
			// edge protection
			if (o.pos.x >= pixelSize.x - cMargin) {
				o.pos.x = pixelSize.x - cMargin;
			}
			else if (o.pos.x < cMargin) {
				o.pos.x = cMargin;
			}
			if (o.pos.y >= pixelSize.y - cMargin) {
				o.pos.y = pixelSize.y - cMargin;
			}
			else if (o.pos.y < cMargin) {
				o.pos.y = cMargin;
			}
#endif

			auto spd = o.pos - o.lastPos;
			auto vd = cVelocityDamping;
			if (o.item->typeId == Player::cTypeId) {
				vd = cVelocityDamping * 3.f;
			}

			spd = spd + (o.acc - spd * vd) * (gg.cDelta * gg.cDelta);
			// max speed limit
#if 0
			if (spd.x * spd.x + spd.y * spd.y > cMaxSpeed * cMaxSpeed) {
				auto mag = std::sqrtf(spd.x * spd.x + spd.y * spd.y);
				spd = spd / mag * cMaxSpeed;
			}
#else
			if (spd.x > cMaxSpeed) spd.x = cMaxSpeed;
			else if (spd.x < -cMaxSpeed) spd.x = -cMaxSpeed;
			if (spd.y > cMaxSpeed) spd.y = cMaxSpeed;
			else if (spd.y < -cMaxSpeed) spd.y = -cMaxSpeed;
#endif

			o.lastPos = o.pos;
			o.pos = o.pos + spd;
			o.acc = {};

			// wall collision response
			using G = decltype(scene->gridBuildings);
			auto& g = scene->gridBuildings;
			auto cri = g.PosToCRIndex(o.pos);
			g.ForeachBy9(cri.y, cri.x, [&](G::Node& node, float range)->void {
				auto d = o.pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = node.cache.radius + o.radius;
				auto rr = r * r;
				if (mag2 < rr) {	// cross?
					if (mag2 > 0) {
						auto mag = std::sqrtf(mag2);
						auto norm = d / mag;
						o.pos = node.cache.pos + norm * r;	// force move away
					}
				}
				});

			// write back
			if (o.item->pos != o.pos) {
				o.item->pos = o.pos;
				o.item->y = o.pos.y;
			}
		}
	}

	void PhysSystem::Step() {
		Fill();
		Calc();
		Writeback();
	}

}

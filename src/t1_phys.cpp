#include "pch.h"
#include "t1_.h"

namespace Test1 {

	PhysSystem::Data& PhysSystem::At(int32_t indexAtNodes_) {
		assert(indexAtNodes_ >= 0 && indexAtNodes_ < count);
		return datas[nodes[indexAtNodes_].indexAtDatas];
	}

	void PhysSystem::Init(Scene* scene_, int32_t capacity_) {
		assert(!nodes && !buckets && capacity_ >= 0);
		scene = scene_;
		pixelSize = scene->gridBuildings.pixelSize;

		numRows = std::ceilf(pixelSize.y / cCellPixelSize);
		numCols = std::ceilf(pixelSize.x / cCellPixelSize);

		bucketsLen = numRows * numCols;
		capacity = capacity_;
		freeHead = -1;
		freeCount = count = 0;
		if (capacity_) {
			nodes = std::make_unique_for_overwrite<Node[]>(capacity_);
			datas = std::make_unique_for_overwrite<Data[]>(capacity_);
		}
		buckets = std::make_unique_for_overwrite<Bucket[]>(bucketsLen);
		ClearBuckets();
	}

	void PhysSystem::Reserve(int32_t capacity_) {
		assert(buckets && capacity_ > 0);
		if (capacity_ <= capacity) return;
		capacity = capacity_;

		auto newNodes = std::make_unique_for_overwrite<Node[]>(capacity_);
		::memcpy((void*)newNodes.get(), (void*)nodes.get(), count * sizeof(Node));
		nodes = std::move(newNodes);

		auto newDatas = std::make_unique_for_overwrite<Data[]>(capacity_);
		::memcpy((void*)newDatas.get(), (void*)datas.get(), datasLen * sizeof(Data));
		datas = std::move(newDatas);
	}

	int32_t PhysSystem::Add(SceneItem* ud_) {
		assert(buckets);
		auto pos_ = ud_->pos;
		assert(pos_.y >= 0.f && pos_.y < pixelSize.y);
		assert(pos_.x >= 0.f && pos_.x < pixelSize.x);
		auto radius_ = ud_->radius;
		assert(radius_ > 0.f);

		int32_t ni;
		if (freeCount > 0) {
			ni = freeHead;
			freeHead = nodes[ni].next;
			freeCount--;
		}
		else {
			if (count == capacity) {
				Reserve(count ? count * 2 : 16);
			}
			ni = count;
			count++;
		}

		auto& n = nodes[ni];
		n.next = -1;
		n.indexAtDatas = datasLen;
		n.ud = ud_;

		auto& d = datas[datasLen++];
		d.indexAtNodes = ni;
		d.radius = radius_;
		d.pos = pos_;
		d.lpos = pos_;
		d.acc = 0;

		return ni;
	}

	void PhysSystem::Remove(int32_t indexAtNodes_) {
		assert(buckets);
		assert(indexAtNodes_ >= 0 && indexAtNodes_ < count);
		assert(nodes[indexAtNodes_].ud);

		auto& n = nodes[indexAtNodes_];
		auto& ld = datas[datasLen - 1];

		nodes[ld.indexAtNodes].indexAtDatas = n.indexAtDatas;	// redirect
		::memcpy((void*)&datas[n.indexAtDatas], (void*)&ld, sizeof(Data));
		datasLen--;

		n.next = freeHead;
		n.indexAtDatas = -1;
		n.ud = {};
		freeHead = indexAtNodes_;
		freeCount++;
	}

	void PhysSystem::ClearBuckets() {
		assert(buckets);
		for (int32_t i = 0; i < bucketsLen; ++i) {
			buckets[i].len = 0;
		}
	}

	void PhysSystem::FillBuckets() {
		for (int32_t di = 0; di < datasLen; ++di) {
			auto p = (datas[di].pos * _1_cellSize).As<int32_t>();
			assert(p.x >= 0 && p.x < numCols && p.y >= 0 && p.y < numRows);
			auto& b = buckets[p.x * numRows + p.y];
			if (b.len < b.indexAtDatass.size()) {	// ignore
				b.indexAtDatass[b.len++] = di;
			}
		}
	}

	void PhysSystem::CalcDatas() {
		for (int32_t bi = 0; bi < bucketsLen; ++bi) {
			auto& b = buckets[bi];
			if (!b.len) continue;
			Calc(b, buckets[bi - 1]);
			Calc(b, buckets[bi]);
			Calc(b, buckets[bi + 1]);
			Calc(b, buckets[bi + numRows - 1]);
			Calc(b, buckets[bi + numRows]);
			Calc(b, buckets[bi + numRows + 1]);
			Calc(b, buckets[bi - numRows - 1]);
			Calc(b, buckets[bi - numRows]);
			Calc(b, buckets[bi - numRows + 1]);
		}
	}

	void PhysSystem::Calc(Bucket& b1_, Bucket& b2_) {
		for (int32_t di1 = 0; di1 < b1_.len; ++di1) {
			for (int32_t di2 = 0; di2 < b2_.len; ++di2) {
				Calc(datas[b1_.indexAtDatass[di1]], datas[b2_.indexAtDatass[di2]]);
			}
		}
	}

	void PhysSystem::Calc(Data& d1_, Data& d2_) {
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

	void PhysSystem::UpdateDatas() {
		for (int32_t i = 0; i < datasLen; ++i) {
			auto& o = datas[i];
			auto ud = nodes[o.indexAtNodes].ud;
			//o.acc += XY{ 0, cGravity };

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

			auto spd = o.pos - o.lpos;
			auto vd = cVelocityDamping;
			if (ud->typeId == Player::cTypeId) {
				vd = cVelocityDamping * 3.f;
			}

			spd = spd + (o.acc - spd * vd) * (gg.cDelta * gg.cDelta);
			// max speed limit
			//if (spd.x * spd.x + spd.y * spd.y > cMaxSpeed * cMaxSpeed) {
			//	auto mag = std::sqrtf(spd.x * spd.x + spd.y * spd.y);
			//	spd = spd / mag * cMaxSpeed;
			//}
			if (spd.x > cMaxSpeed) spd.x = cMaxSpeed;
			else if (spd.x < -cMaxSpeed) spd.x = -cMaxSpeed;
			if (spd.y > cMaxSpeed) spd.y = cMaxSpeed;
			else if (spd.y < -cMaxSpeed) spd.y = -cMaxSpeed;

			o.lpos = o.pos;
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
			if (ud->pos != o.pos) {
				ud->pos = o.pos;
				ud->y = o.pos.y;
			}
		}
	}

	void PhysSystem::Update() {
		ClearBuckets();
		FillBuckets();
		CalcDatas();
		UpdateDatas();
	}

}

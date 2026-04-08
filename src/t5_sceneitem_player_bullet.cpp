#include "pch.h"
#include "t5_.h"

namespace Test5 {

	void PlayerBullet::Init(xx::Weak<Player> owner_, XY tarPos_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = std::move(owner_);

		// 根据 tarPos 算出发射角度的 cos sin
		auto p = owner->pos;
		auto d = tarPos_ - p;
		auto mag2 = d.x * d.x + d.y * d.y;
		XY cosSin;
		if (mag2 > 0.0001f) {
			auto mag = std::sqrtf(mag2);
			cosSin = d / mag;
			owner->lastBulletCosSin = cosSin;
		}
		else {
			cosSin = owner->lastBulletCosSin;
		}

		// 起始坐标：从玩家中心点出发，前进玩家半径的距离
		pos = owner->pos + cosSin * owner->radius;
		y = pos.y;
		radius = 16.f;
		scale = radius * 2.f / gg.pics.c32_bullet.uvRect.w;
		radians = {};

		indexAtContainer = scene->playerBullets.len - 1;
		assert(scene->playerBullets[indexAtContainer].pointer == this);

		// 算出每帧的步进
		inc = cosSin * cSpeed * gg.cDelta;
		deathTime = scene->time + cMaxLifetime;
	}

	void PlayerBullet::Update() {
		if (scene->time >= deathTime) {
			Dispose();
			return;
		}

		pos += inc;
		// 查找子弹位置的 bucket，爆炸它, 并自杀
		bool hited{};
		auto cri = scene->phys->PosToCRIndex(pos);
		scene->phys->ForeachBy9(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->void {
			if (o.value->typeId == Bucket::cTypeId) {
				auto d = o.cache.pos - pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = o.cache.radius + radius;
				auto rr = r * r;
				if (mag2 < rr) {
					hited = true;
					((Bucket*)o.value)->Explode();
				}
			}
		});
		if (!hited) {
			// 查找子弹位置的建筑. 如果有相交，子弹自杀
			using G = decltype(scene->gridBuildings);
			auto& g = scene->gridBuildings;
			cri = g.PosToCRIndex(pos);
			hited = g.ForeachBy9Break(cri.y, cri.x, [&](G::Node& node, float range)->bool {
				auto d = pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = node.cache.radius + radius;
				auto rr = r * r;
				return mag2 < rr;
				});
		}

		if (hited) {
			Dispose();
			return;
		}
	}

	void PlayerBullet::Draw() {
		gg.Quad().DrawFrame(gg.pics.c32_bullet, scene->cam.ToGLPos(pos)
			, scale * scene->cam.scale, radians);
	}

	void PlayerBullet::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		auto& container = scene->playerBullets;
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

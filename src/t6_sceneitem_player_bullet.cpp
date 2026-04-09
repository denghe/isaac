#include "pch.h"
#include "t6_.h"

namespace Test6 {

	void PlayerBullet::Init(xx::Weak<Player> owner_) {
		typeId = cTypeId;
		scene = owner_->scene;
		owner = std::move(owner_);

		auto pp = owner->pos;
		auto cosSin = owner->cosSin;
		auto pr = owner->radius;
		// 起始坐标：从玩家中心点出发，前进玩家半径的距离
		pos = pp + cosSin * pr;
		y = pos.y;
		radius = 16.f;
		scale = radius * 2.f / gg.pics.c32_bullet.uvRect.w;
		radians = {};

		indexAtContainer = scene->playerBullets.len - 1;
		assert(scene->playerBullets[indexAtContainer].pointer == this);

		damage = owner->damage;

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
		y = pos.y;

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
					// 在目标 bucket 身上挂 dot
					((Bucket*)o.value)->MakePlayerDot1(this);
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

	void PlayerBullet::DrawLight() {
		gg.Quad().DrawFrame(gg.pics.c64_light, scene->cam.ToGLPos(pos)
			, (256.f / 64.f) * scene->cam.scale);
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

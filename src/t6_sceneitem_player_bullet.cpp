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

		// 算出每帧的步进
		inc = cosSin * cSpeed * gg.cDelta;
		deathTime = scene->time + cMaxLifetime;

		damage = owner->damage;
		leftPierceCount = cPierceCount;
	}

	void PlayerBullet::Update() {
		if (scene->time >= deathTime) {
			Dispose();
			return;
		}

		pos += inc;
		y = pos.y;

		assert(leftPierceCount > 0);
		// 移除名单里面已经过期 或 对象已失效 的那部分
		auto currTime = scene->time;
		for (auto i = pierceInfos.len - 1; i >= 0; --i) {
			if (auto& o = pierceInfos[i]; !o.target || currTime >= o.elapsedTime) {
				pierceInfos.SwapRemoveAt(i);
			}
		}

		// 查找子弹位置的 bucket，爆炸它, 并自杀
		auto cri = scene->phys->PosToCRIndex(pos);
		scene->phys->ForeachBy9Break(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->bool {
			// 目标类型过滤( phys 里拥有众多会产生物理交互的类型, 过滤掉子弹无法攻击的那部分 )
			if (o.value->typeId == Bucket::cTypeId) {
				// 开始碰撞判定
				auto d = o.cache.pos - pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = o.cache.radius + radius;
				auto rr = r * r;
				// 距离小于圆心和: 相交
				if (mag2 < rr) {
					// 开始穿刺处理
					// 如果目标在名单里则忽略碰撞
					if (pierceInfos.Exists([&](PierceInfo& pi)->bool { 
						// 这里不用 Try版 是因为 Foreach 过程中不会发生 target 删除行为
						return pi.target.GetPointer() == o.value;
					})) {
						// 忽略碰撞 继续下次查询
						return false;
					}

					// 在目标 bucket 身上挂 dot
					((Bucket*)o.value)->MakePlayerDot1(this);
					// 记录到名单
					pierceInfos.Emplace(PierceInfo{
						.target = xx::WeakFromThis(o.value), 
						.elapsedTime = currTime + cPierceInterval
					});
					// 穿刺次数 -1
					--leftPierceCount;
					// 没有次数就终止整个查询
					if (leftPierceCount <= 0) return true;
				}
			}
			return false;
		});

		bool needDispose{};
		if (leftPierceCount > 0) {
			// 查找子弹位置的建筑. 如果有相交，子弹自杀
			using G = decltype(scene->gridBuildings);
			auto& g = scene->gridBuildings;
			cri = g.PosToCRIndex(pos);
			needDispose = g.ForeachBy9Break(cri.y, cri.x, [&](G::Node& node, float range)->bool {
				auto d = pos - node.cache.pos;
				auto mag2 = d.x * d.x + d.y * d.y;
				auto r = node.cache.radius + radius;
				auto rr = r * r;
				return mag2 < rr;
			});
		}
		else {
			needDispose = true;
		}
		if (needDispose) {
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

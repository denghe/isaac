#include "pch.h"
#include "t4_sceneitem_bullet1.h"
#include "t4_sceneitem_bullet2.h"

namespace Test4 {

	void Bullet1::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Monster> owner_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		scene_->itemsGrid32.Add(indexAtGrid, this);
		owner = std::move(owner_);
		deathTime = scene->time + cLifespan;
		nextShootTime = scene->time + cShootInterval;
	}

	void Bullet1::Update() {
		// 每隔一段时间发射一颗 2 级子弹. 随机移动. 每隔一段时间随机移动一次，持续一段时间，间隔一段时间

		// 生命周期处理
		if (scene->time >= deathTime || !owner) {
			// todo: 淡出? 为避免干扰查询，可以销毁对象并在当前坐标创建一个尸体特效
			Dispose();	// unsafe
			return;
		}

		// 碰撞检测
		bool crossed{}, disposed{};
		auto cri = scene->itemsGrid64.PosToCRIndex(pos);
		//auto searchRange = radius + 64.f;
		//scene->itemsGrid64.ForeachByRange(cri.y, cri.x, searchRange, gg.sgrdd, [&](decltype(scene->itemsGrid64)::Node& node, float range)->void {
		scene->itemsGrid64.ForeachBy9(cri.y, cri.x, [&](decltype(scene->itemsGrid64)::Node& node, float range)->void {
			// 统计查询次数
			++scene->searchCount;
			//// 避开自身
			//if (node.value == this) return;
			//// 避开非怪
			//if (node.value->typeId != Monster::cTypeId) return;
			assert(node.value->typeId == Monster::cTypeId);
			// 避开主人
			auto m = (Monster*)node.value;
			if (m == owner.GetPointer()) return;
			// 距离检测
			auto d = pos - node.cache.pos;
			auto mag2 = d.x * d.x + d.y * d.y;
			auto r = radius + node.cache.radius;
			auto rr = r * r;
			// 未碰撞
			if (mag2 > rr) return;
			// 碰撞了
			crossed = true;
			// 持有当前类的弱引用，方便判断有效性
			auto self = xx::WeakFromThis(this);
			// 干掉目标. 有可能目标死亡时也递归杀死当前对象
			node.value->Dispose();
			// 如果当前对象被释放了, 标记一下
			if (!self) disposed = true;
		});
		// 如果当前对象被释放了, 直接返回( 不能再访问所有成员 )
		if (disposed) return;
		// 如果碰到目标，自己也消失
		if (crossed) {
			TryDispose();
			return;
		}

		// 发射子弹处理
		while (scene->time >= nextShootTime) {
			nextShootTime += cShootInterval;
			// 如果对象创建数量达到上限，直接跳过创建
			if (scene->items.len >= Scene::cNumMaxItems) {
				++scene->createIgnoreCount;
				continue;
			}
			// 计算子弹的初始位置和属性
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			auto bulletPos = pos + XY{ cosf(radians), sinf(radians) } * radius;
			bulletPos = FixPosition(bulletPos);
			scene->items.Emplace().Emplace<Bullet2>()->Init(scene, bulletPos, 32.f, xx::WeakFromThis(this));
		}

		// 移动处理
		XX_BEGIN(_1);
		while (1) {
			// "协程"代码中不可夹杂局部变量，必须放在括号里，且不能跨越 yield
			{
				// 计算要移动多久
				auto duration = gg.rnd.Next(cMoveDuration);
				moveLeftStep = std::ceilf(duration * gg.cFps);
				// 随机选一个移动方向
				auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
				// 计算移动增量
				moveInc = XY{ std::cosf(radians), std::sinf(radians) } * cMoveSpeed * gg.cDelta;
			}
			// 移动
			while (moveLeftStep > 0) {
				XX_YIELD(_1);
				--moveLeftStep;
				SetPosition(pos + moveInc);
			}
		}
		XX_END(_1);
	}

	void Bullet1::Draw() {
		scene->DrawItem(gg.pics.c64_bullet, pos, radius * (1.f / 32.f), owner->color);
	}

	void Bullet1::OnDispose() {
		// 从 grid 中移除对象，避免被查询到
		if (indexAtGrid != -1) {
			assert(scene->itemsGrid32.ValueAt(indexAtGrid) == this);
			scene->itemsGrid32.Remove(indexAtGrid, this);
		}
	}

	void Bullet1::SetPosition(XY pos_) {
		SceneItem::SetPosition(pos_);
		// 新的 pos 值同步到空间索引 grid
		scene->itemsGrid32.Update(indexAtGrid, this);
	}
}

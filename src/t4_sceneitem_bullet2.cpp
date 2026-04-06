#include "pch.h"
#include "t4_sceneitem_bullet2.h"

namespace Test4 {

	void Bullet2::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Bullet1> mother_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		scene_->itemsGrid16.Add(indexAtGrid, this);
		owner = mother_->owner;
		deathTime = scene->time + cLifespan;
	}

	void Bullet2::Update() {
		// 随机移动. 每隔一段时间随机移动一次，持续一段时间，间隔一段时间

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

		// 移动处理
		XX_BEGIN(_1);
		while (1) {
			// 设置移动方向和持续时间
			{
				// 计算要移动多少帧
				moveLeftStep = std::ceilf(gg.rnd.Next(cMoveDuration) * gg.cFps);
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
			// 计算要休息多少帧
			moveLeftStep = std::ceilf(gg.rnd.Next(cMoveInterval) * gg.cFps);
			// 休息
			while (moveLeftStep > 0) {
				XX_YIELD(_1);
				--moveLeftStep;
			}
		}
		XX_END(_1);
	}

	void Bullet2::Draw() {
		scene->DrawItem(gg.pics.c32_bullet, pos, radius * (1.f / 16.f), owner->color);
	}

	void Bullet2::OnDispose() {
		// 从 grid 中移除对象，避免被查询到
		if (indexAtGrid != -1) {
			assert(scene->itemsGrid16.NodeAt(indexAtGrid).value == this);
			scene->itemsGrid16.Remove(indexAtGrid, this);
		}
	}

	void Bullet2::SetPosition(XY pos_) {
		SceneItem::SetPosition(pos_);
		// 新的 pos 值同步到空间索引 grid
		scene->itemsGrid16.Update(indexAtGrid, this);
	}
}

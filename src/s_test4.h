#pragma once
#include "game.h"
#include "mm_scene.h"
#include "g_scenebase.h"
#include <xx_grid2d_circle.h>

// 实现一套高效的 item 可随便增删的数组遍历逻辑
// todo:
// 需求是  空间中 有两只怪， 不断向周围发射  随机移动  的子弹。
// 子弹有寿命，到期自杀。 
// 子弹自杀时，会创建属于己方的二级子弹
// 子弹碰到  对方时，可杀死对方，对方的子弹 同时消失


namespace Test4 {

	using SceneBase = Global::SceneBase;
	using SceneItemBase = Global::SceneItemBase;
	struct Scene : SceneBase {
		using SceneBase::SceneBase;

		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void DrawItem(xx::Frame& f_, XY pos_, float scale_);

		XY mapSize{}, mapCenterPos{};
		static constexpr float cCellPixelSize{ 64.f };
		static constexpr int32_t cNumMaxItems{ 100000 };
		xx::List<xx::Shared<SceneItemBase>> items;
		xx::Grid2dCircle<SceneItemBase*> itemsGrid;	// todo: cache
	};

	/**********************************************************************************************************/
	/**********************************************************************************************************/

	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};

		void SceneItemInit(int32_t typeId_, Scene* scene_, XY pos_, float radius_) {
			scene = scene_;
			typeId = typeId_;
			indexAtContainer = scene->items.len - 1;
			assert(scene_->items[indexAtContainer].pointer == this);
			pos = pos_;
			radius = radius_;
			scene_->itemsGrid.Add(indexAtGrid, this);
		}

		void Dispose() override {
			assert(scene);
			assert(!disposing);
			assert(indexAtContainer != -1);
			assert(scene->items[indexAtContainer].pointer == this);
			assert(scene->itemsGrid.ValueAt(indexAtGrid) == this);

			// 设置标记
			disposing = true;

			// 从 grid 中移除对象，避免被查询到
			if (indexAtGrid != -1) {
				scene->itemsGrid.Remove(indexAtGrid, this);
			}

			// 进一步释放资源，事件逻辑
			OnDispose();

			// 从容器中移除对象( 释放内存 )
			auto i = indexAtContainer;
			scene->items.Back()->indexAtContainer = i;
			indexAtContainer = -1;
			scene->items.SwapRemoveAt(i);	// unsafe: release memory
		}
	};

	/**********************************************************************************************************/
	/**********************************************************************************************************/

	struct Monster : SceneItem {
		static constexpr int32_t cTypeId{ __LINE__ };
		static constexpr float cLifespan{ 100.f };
		static constexpr float cShootInterval{ 1.f };
		int32_t _1{};
		float deathTime{}, nextShootTime{};
		void Init(Scene* scene_, XY pos_, float radius_);
		void Update() override;
		void Draw() override;
	};

	struct Bullet1 : SceneItem {
		static constexpr int32_t cTypeId{ __LINE__ };
		static constexpr float cLifespan{ 10.f };
		static constexpr float cShootInterval{ 0.5f };
		static constexpr float cMoveSpeed{ 300.f };
		static constexpr xx::FromTo<float> cMoveDuration{ 0.3f, 0.7f };
		static constexpr xx::FromTo<float> cMoveInterval{ 0.2f };
		xx::Weak<Monster> owner;
		int32_t _1{};
		float deathTime{}, nextShootTime{};
		int32_t moveLeftStep{};
		XY moveInc{};
		void Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Monster> owner_);
		void Update() override;
		void Draw() override;
	};

	struct Bullet2 : SceneItem {
		static constexpr int32_t cTypeId{ __LINE__ };
		static constexpr float cLifespan{ 10.f };
		static constexpr float cShootInterval{ 0.5f };
		xx::Weak<Monster> owner;
		int32_t _1{};
		float deathTime{}, nextShootTime{};
		int32_t moveLeftStep{};
		XY moveInc{};
		void Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Bullet1> mother_);
		void Update() override;
		void Draw() override;
	};

	/**********************************************************************************************************/
	/**********************************************************************************************************/

	inline void Monster::Init(Scene* scene_, XY pos_, float radius_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		deathTime = scene->time + cLifespan;
		nextShootTime = scene->time + cShootInterval;
	}

	inline void Monster::Update() {
		// 每隔一段时间发射一颗子弹. 静止不动. 可以活很久
		if (scene->time >= deathTime) {
			Dispose();	// unsafe
			return;
		}
		if (scene->time >= nextShootTime) {
			nextShootTime += cShootInterval;
			// 计算子弹的初始位置和属性
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			auto bulletPos = pos + XY{ cosf(radians), sinf(radians) } * radius;
			scene->items.Emplace().Emplace<Bullet1>()->Init(scene, bulletPos, 64.f, xx::WeakFromThis(this));
		}
	}

	inline void Monster::Draw() {
		scene->DrawItem(gg.pics.c128_monster, pos, radius * ( 1.f / 64.f));
	}

	/**********************************************************************************************************/
	/**********************************************************************************************************/

	inline void Bullet1::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Monster> owner_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		owner = std::move(owner_);
		deathTime = scene->time + cLifespan;
		nextShootTime = scene->time + cShootInterval;
	}

	inline void Bullet1::Update() {
		// todo
		// 每隔一段时间发射一颗 2 级子弹. 随机移动
		if (scene->time >= deathTime) {
			Dispose();	// unsafe
			return;
		}
		if (scene->time >= nextShootTime) {
			nextShootTime += cShootInterval;
			// 计算子弹的初始位置和属性
			auto radians = gg.rnd.Next<float>(-M_PI, M_PI);
			auto bulletPos = pos + XY{ cosf(radians), sinf(radians) } * radius;
			scene->items.Emplace().Emplace<Bullet2>()->Init(scene, bulletPos, 32.f, xx::WeakFromThis(this));
		}

		// 移动处理. 每隔一段时间随机移动一次，持续一段时间，间隔一段时间
		XX_BEGIN(_1);
		while (1) {
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
				pos += moveInc;
				// todo: sync pos to grid
			}
		}
		XX_END(_1);
	}

	inline void Bullet1::Draw() {
		scene->DrawItem(gg.pics.c64_bullet, pos, radius * (1.f / 32.f));
	}

	/**********************************************************************************************************/
	/**********************************************************************************************************/

	inline void Bullet2::Init(Scene* scene_, XY pos_, float radius_, xx::Weak<Bullet1> mother_) {
		SceneItemInit(cTypeId, scene_, pos_, radius_);
		owner = mother_->owner;
	}

	inline void Bullet2::Update() {
		// todo: 随机移动
	}

	inline void Bullet2::Draw() {
		scene->DrawItem(gg.pics.c32_bullet, pos, radius * (1.f / 16.f));
	}

	/**********************************************************************************************************/
	/**********************************************************************************************************/

	inline void Scene::Init() {
		SceneBase::Init();

		mapSize = { 1920 * 5, 1080 * 5 };
		mapCenterPos = mapSize / 2;
		cam.Init(gg.scale, gg.designSize.y / mapSize.y, mapCenterPos);

		// 重要：需要大到足以避免 Resize 导致的指针失效. 所有创建行为都要检测个数上限
		items.Reserve(cNumMaxItems);
		itemsGrid.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize), std::ceilf(mapSize.x / cCellPixelSize));


		// 创建两只怪, 直接指定坐标
		items.Emplace().Emplace<Monster>()->Init(this, mapCenterPos + XY{ -1000, 0 }, 128.f);
		items.Emplace().Emplace<Monster>()->Init(this, mapCenterPos + XY{ 1000, 0 }, 128.f);
	}

	inline void Scene::Update() {
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

		SceneBase::Update();
	}

	inline void Scene::FixedUpdate() {
		for (int i = items.len - 1; i >= 0;) {
			items[i]->Update();	// 可能会删除当前对象，也可能会删除其他对象
			if (i >= items.len) {
				i = items.len - 1;
			}
			else {
				--i;
			}
		}
	}

	inline void Scene::Draw() {
		for (auto& o : items) {
			o->Draw();
		}

		SceneBase::Draw();
	}

	XX_INLINE void Scene::DrawItem(xx::Frame& f_, XY pos_, float scale_) {
		auto s = scale_ * cam.scale;
		auto p = cam.ToGLPos(pos_);
		gg.Quad().DrawFrame(f_, p, s);
	}

}

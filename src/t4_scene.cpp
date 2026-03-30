#include "pch.h"
#include "t4_scene.h"
#include "mm_scene.h"
#include "t4_sceneitem_monster.h"

// 实现一套高效的 item 可随便增删的数组遍历逻辑
// todo:
// 需求是  空间中 有两只怪， 不断向周围发射  随机移动  的子弹。
// 子弹有寿命，到期自杀。 
// 子弹自杀时，会创建属于己方的二级子弹
// 子弹碰到  对方时，可杀死对方，对方的子弹 同时消失

namespace Test4 {

	void Scene::Init() {
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

	void Scene::Update() {
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

		SceneBase::Update();
	}

	void Scene::FixedUpdate() {
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

	void Scene::Draw() {
		for (auto& o : items) {
			o->Draw();
		}

		SceneBase::Draw();
	}

	void Scene::DrawItem(xx::Frame& f_, XY pos_, float scale_) {
		auto s = scale_ * cam.scale;
		auto p = cam.ToGLPos(pos_);
		gg.Quad().DrawFrame(f_, p, s);
	}

}

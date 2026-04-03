#include "pch.h"
#include "t4_scene.h"
#include "mm_scene.h"
#include "t4_sceneitem_monster.h"

// 实现一套高效的 item 可随便增删的数组遍历逻辑
// 需求是 空间中 有两只 怪，不断向周围发射随机移动的 子弹1
// 子弹1 有寿命，到期自杀。期间随机移动，不断向周围发射  随机移动  的子弹2
// 子弹1 或 2 碰到 对方怪时，可杀死对方怪，对方的子弹 同时消失

namespace Test4 {

	void Scene::Init() {
		SceneBase::Init();

		mapSize = { 1920 * 4, 1080 * 4 };
		mapCenterPos = mapSize / 2;
		cam.Init(gg.scale, gg.designSize.y / mapSize.y, mapCenterPos);

		// 重要：需要大到足以避免 Resize 导致的指针失效. 所有创建行为都要检测个数上限
		items.Reserve(cNumMaxItems);
		
		// 初始化空间索引容器
		itemsGrid16.Init(16.f, std::ceilf(mapSize.y / 16.f), std::ceilf(mapSize.x / 16.f));
		itemsGrid32.Init(32.f, std::ceilf(mapSize.y / 32.f), std::ceilf(mapSize.x / 32.f));
		itemsGrid64.Init(64.f, std::ceilf(mapSize.y / 64.f), std::ceilf(mapSize.x / 64.f));

		// 创建两只怪, 直接指定坐标
		items.Emplace().Emplace<Monster>()->Init(this, mapCenterPos + XY{ -1200, 0 }, 128.f, xx::RGBA8_Green);
		items.Emplace().Emplace<Monster>()->Init(this, mapCenterPos + XY{ 1200, 0 }, 128.f, xx::RGBA8_Blue);
	}

	void Scene::MakeUI() {
		SceneBase::MakeUI();
		static constexpr float cLineHeight{ 100 };
		static constexpr float cItemHeight{ 80 };
		static constexpr float cMargin{ 20 };
		auto fontSize = cItemHeight - gg.embed.cfg_s9bN->paddings.TopBottom();

		auto C = ui->Make<xx::Node>();
		xx::Layouter L;
		auto ds = gg.designSize;
		auto ds_2 = ds / 2;
		L.InitBegin(C, 2, gg.p5 + XY{ 0, ds_2.y }, { 0.5f, 1.f }, ds.x)
			.HAlign(xx::HAligns::Center)
			.LeftMargin(cMargin)
			.DefaultLineHeight(cLineHeight);

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)("game speed: 1x")).SetLabelBorder().onClicked = [this] {
			timeScale = 1.f;
			};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)("10x")).SetLabelBorder().onClicked = [this] {
			timeScale = 10.f;
			};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)("100x")).SetLabelBorder().onClicked = [this] {
			timeScale = 100.f;
			};

		L.InitEnd();
	}

	void Scene::Update() {
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

		// 驱动 FixedUpdate
		SceneBase::Update();
	}

	void Scene::FixedUpdate() {
		// 每帧开始时重置计数器
		searchCount = 0;
		createIgnoreCount = 0;

		for (int i = items.len - 1; i >= 0;) {

			// 帧逻辑. 可能会自杀，可能会删除其他对象
			items[i]->Update();

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

		gg.uiText->SetText(xx::ToString("range search count: ", searchCount, " create ignore count: ", createIgnoreCount, " items.len = ", items.len));
		SceneBase::Draw();
	}

	void Scene::DrawItem(xx::Frame& f_, XY pos_, float scale_, xx::RGBA8 color_) {
		auto s = scale_ * cam.scale;
		auto p = cam.ToGLPos(pos_);
		gg.Quad().DrawFrame(f_, p, s, 0, 1, color_);
	}

}

#include "pch.h"
#include "s_mainmenu.h"
#include "s_test1.h"

namespace MainMenu {

	void Scene::MakeUI() {
		SceneBase::MakeUI();
		static constexpr float cLineHeight{ 100 };
		static constexpr float cItemHeight{ 80 };
		static constexpr float cMargin{ 20 };
		auto fontSize = cItemHeight - gg.embed.cfg_s9bN->paddings.TopBottom();

		// todo: resize bug fix

		auto C = ui->Make<xx::Node>();
		xx::Layouter L;
		auto ds = gg.designSize;
		auto ds_2 = ds / 2;
		L.InitBegin(C, 2, gg.p5 + XY{ 0, ds_2.y }, { 0.5f, 1.f }, ds.x)
			.HAlign(xx::HAligns::Center)
			.LeftMargin(cMargin)
			.DefaultLineHeight(cLineHeight);

		L.Append(C->Make<xx::Label>()->Init(2, 0, 0, fontSize * 10)("isaac")).SetBorder();
		L.EndLine();

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)("play")).SetLabelBorder().onClicked = [this] {
			gg.MakeScene<Test1::Scene>()->Init();
		};

		L.InitEnd();
	}

	void Scene::Update() {
		// handle inputs
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			if (auto node = ui->FindTopESCHandler(); node) {
				node->HandleESC();
			}
			else {
				gg.running = false;
			}
			return;
		}
		SceneBase::Update();
	}

}

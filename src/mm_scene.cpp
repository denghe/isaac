#include "pch.h"
#include "mm_scene.h"
//#include "t1_.h"
//#include "t2_.h"
//#include "t3_.h"
#include "t4_scene.h"
#include "t5_.h"
#include "t6_.h"

namespace MainMenu {

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

		L.Append(C->Make<xx::Label>()->Init(2, 0, 0, fontSize * 3)(U"以撒模拟")).SetBorder();
		L.EndLine();

		//L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"密集投放炸弹桶v1")).SetLabelBorder().onClicked = [this] {
		//	SetDelayCmd([this] {
		//		gg.MakeScene<Test1::Scene>()->Init();
		//	});
		//};

		//L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"密集投放炸弹桶v2")).SetLabelBorder().onClicked = [this] {
		//	SetDelayCmd([this] {
		//		gg.MakeScene<Test2::Scene>()->Init();
		//	});
		//};

		//L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"连环引爆炸弹桶")).SetLabelBorder().onClicked = [this] {
		//	SetDelayCmd([this] {
		//		gg.MakeScene<Test3::Scene>()->Init();
		//	});
		//};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"连锁事件测试")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test4::Scene>()->Init();
			});
		};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"物理移动射击爆炸光效")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test5::Scene>()->Init();
			});
		};

		L.Append(C->Make<xx::LabelButton>()->Init(2, 0, 0, fontSize)(U"测试DOT效果")).SetLabelBorder().onClicked = [this] {
			SetDelayCmd([this] {
				gg.MakeScene<Test6::Scene>()->Init();
			});
		};

		L.InitEnd();
	}

	void Scene::Update() {
		// 按 ESC 可以关闭 顶层 popup UI 或直接退出程序
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

		// 延迟执行指令( 给一些按键释放留点时间 避免误触 )
		if (delayCmd) {
			if (time >= delayCmdExecTime) {
				delayCmd();
			}
		}
	}

}

#include "pch.h"
#include "g_scenebase.h"

namespace Global {

	void SceneBase_::Init() {
	}

	void SceneBase_::MakeUI() {
		ui.Emplace()->InitRoot(gg.scale);
	}

	void SceneBase_::Update() {
		//// handle inputs
		//if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
		//	gg.MakeScene<MainMenu::Scene>()->Init();
		//	return;
		//}

		// fixed update
		auto d = float(std::min((float)gg.delta, gg.cMaxDelta) * timeScale);
		timePool += d;
		while (timePool >= gg.cDelta) {
			time += gg.cDelta;
			timePool -= gg.cDelta;
			FixedUpdate();
		}
	}

	void SceneBase_::FixedUpdate() {
	}

	void SceneBase_::Draw() {
		// draw ui
		gg.DrawNode(ui);
	}

	void SceneBase_::DrawCursor() {
		cursor->Draw();
	}

	void SceneBase_::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale);
		cam.SetBaseScale(gg.scale);
	}

	void SceneBase_::OnFocus(bool focused_) {
		if (focused_) {
			gg.sound.SetGlobalVolume(1);
		}
		else {
			gg.sound.SetGlobalVolume(0);
		}
	}

	SceneBase_::~SceneBase_() {
	}
}

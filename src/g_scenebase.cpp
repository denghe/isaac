#include "pch.h"
#include "g_scenebase.h"

namespace Global {

	void SceneBase::Init() {
		cam.Init(gg.scale, 1.f, gg.designSize / 2);
		sortContainer.Resize<true>((int32_t)gg.designSize.y);
		cursor.Emplace()->Init();
		MakeUI();
	}

	void SceneBase::MakeUI() {
		ui.Emplace()->InitRoot(gg.scale);
	}

	void SceneBase::Update() {
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

	void SceneBase::FixedUpdate() {
	}

	void SceneBase::Draw() {
		// draw ui
		gg.DrawNode(ui);
	}

	void SceneBase::DrawCursor() {
		cursor->Draw();
	}

	void SceneBase::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale);
		cam.SetBaseScale(gg.scale);
	}

	void SceneBase::OnFocus(bool focused_) {
		if (focused_) {
			gg.sound.SetGlobalVolume(1);
		}
		else {
			gg.sound.SetGlobalVolume(0);
		}
	}


	void SceneBase::SortContainerAdd(SceneItem* o_) {
		auto& slot = sortContainer[(int32_t)o_->y];
		o_->next = slot;
		slot = o_;
	}

	void SceneBase::SortContainerDraw() {
		for (auto o : sortContainer) {
			while (o) {
				o->Draw();
				o = o->next;
			}
		}
		memset(sortContainer.buf, 0, sortContainer.len * sizeof(void*));
	}

}

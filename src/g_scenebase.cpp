#include "pch.h"
#include "g_scenebase.h"

namespace Global {

	void SceneItemBase::TryDispose() {
		if (disposing) return;
		Dispose();
	}

	// 默认初始化
	void SceneBase::Init() {
		cam.Init(gg.scale, 1.f, gg.designSize / 2);
		sortContainer.Resize<true>((int32_t)gg.designSize.y);
		cursor.Emplace()->Init();
		MakeUI();
	}

	// UI 初始化，默认创建一个根节点
	void SceneBase::MakeUI() {
		ui.Emplace()->InitRoot(gg.scale);
	}

	// 帧逻辑
	void SceneBase::Update() {
		/* for copy code
		// handle inputs
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}
		*/

		// fixed update
		auto d = float(std::min((float)gg.delta, gg.cMaxDelta) * timeScale);
		timePool += d;
		while (timePool >= gg.cDelta) {
			time += gg.cDelta;
			timePool -= gg.cDelta;
			FixedUpdate();
		}
	}

	// 固定时间帧逻辑( 每秒调用 gg.cFps 次 )
	void FixedUpdate() {}

	// 绘制
	void SceneBase::Draw() {
		// draw ui
		gg.DrawNode(ui);
	}

	// 绘制鼠标指针
	void SceneBase::DrawCursor() {
		cursor->Draw();
	}

	// 处理窗口大小变化，默认调整 UI 和摄像机
	void SceneBase::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale);
		cam.SetBaseScale(gg.scale);
	}

	// 处理窗口焦点变化，默认调整全局音量
	void SceneBase::OnFocus(bool focused_) {
		if (focused_) {
			gg.sound.SetGlobalVolume(1);
		}
		else {
			gg.sound.SetGlobalVolume(0);
		}
	}

	// 往 y排序容器 添加一个对象
	void SceneBase::SortContainerAdd(SceneItemBase* o_) {
		auto& slot = sortContainer[(int32_t)o_->y];
		o_->next = slot;
		slot = o_;
	}

	// 绘制 y排序容器 中的对象，绘制完成后清空容器
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

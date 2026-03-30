#include "pch.h"
#include "game.h"
#include "mm_scene.h"
#include "g_scenebase.h"

// global game instance
Global::Game gg;

// entry
int32_t main() {
	return gg.Run();
}

namespace Global {

	void Game::Init() {
		title = "isaac";
		windowSize = designSize = cDesignSize;
		DisableIME();
	}

	void Game::GLInit() {
		// init global ui
		ui.Emplace()->InitRoot(scale);
		(uiFPS = ui->Make<xx::Label>())->Init(2, p7, a7, 32.f);
		(uiText = ui->Make<xx::Label>())->Init(2, p8, a8, 32.f);
		(uiTips = ui->Make<xx::Label>())->Init(2, p9, a9, 32.f)(U"hotkeys: ESC, [, ]");

		// begin load res
		pics.Load("res/_pics.png");// ->SetParm(GL_LINEAR);
		// ...

		// hide hardware mouse( scene can draw custom mouse cursor )
		SetMousePointerVisible(false);

		// create startup scene
		scene.Emplace<MainMenu::Scene>()->Init();
	}

	// game loop
	void Game::Update() {
		// handle inputs
		if (keyboard[GLFW_KEY_LEFT_BRACKET](0.2f)) {
			isShowInfo = !isShowInfo;
		}
		if (keyboard[GLFW_KEY_RIGHT_BRACKET](0.2f)) {
			isLimitFPS = !isLimitFPS;
		}
		if (keyboard[GLFW_KEY_BACKSLASH](0.2f)) {
			isShowDebugPanel = !isShowDebugPanel;			// todo
		}

		scene->Update();
		if (!minimized) {
			scene->Draw();

			// draw ui
			uiFPS->SetText(fpsVal);
			DrawNode(ui);

			// draw cursor
			scene->DrawCursor();
		}
		if (oldScene) {
			uiText->Clear();
			oldScene.Reset();
		}
	}

	void Game::Delay() {
		if (isLimitFPS) {
			SleepSecs(cDelta - (glfwGetTime() - time));	// for power saving
		}
	}

	void Game::OnResize(bool modeChanged_) {
		uiFPS->position = p7;
		uiText->position = p8;
		uiTips->position = p9;
		ui->Resize(scale);
		if (scene) {
			scene->OnResize(modeChanged_);
		}
	}

	void Game::OnFocus(bool focused_) {
		if (scene) {
			scene->OnFocus(focused_);
		}
	}

	void Game::Stat() {
		fpsVal = xx::ToString(drawFPS);	// for uiFPS
		if (isShowInfo) {
			xx::CoutN("FPS = ", drawFPS, " DrawCall = ", drawCall, " NumVerts = ", drawVerts);
		}
	}

}

#pragma once
#include "pch.h"
#include "_pics.h"

namespace Global {
	struct SceneBase;

	struct Game : xx::GameBase {
		static constexpr float cFps{ 120 };
		static constexpr float cDelta{ 1.f / cFps };
		static constexpr float cMaxDelta{ 0.1f };

		static constexpr XY cDesignSize{ 1920, 1080 };
		static constexpr float cCursorWidth{ 64.f };


		bool isShowInfo{}, isLimitFPS{ true }, uiColorFlag{}, isShowDebugPanel{};
		std::string fpsVal;
		xx::Shared<xx::Node> ui;
		xx::Weak<xx::Label> uiFPS, uiText, uiTips;

		xx::Shared<SceneBase> scene, oldScene;
		template<typename T>
		xx::Shared<T>& MakeScene() {
			oldScene = std::move(scene);
			scene = xx::MakeShared<T>();
			return (xx::Shared<T>&)scene;
		}

		xx::Rnd rnd;
		_pics pics;
		// ...

		void Init() override;
		void GLInit() override;
		void Update() override;
		void Delay() override;
		void Stat() override;
		void OnResize(bool modeChanged_) override;
		void OnFocus(bool focused_) override;
	};

}

extern Global::Game gg;

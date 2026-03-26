#pragma once
#include "game.h"
#include "g_cursor.h"
#include "g_sceneitem.h"

namespace Global {

	struct SceneBase : xx::SceneBase {
		xx::Shared<xx::Node> ui;
		xx::Camera cam;
		float time{}, timePool{}, timeScale{ 1 }, timer{};
		xx::Shared<CursorBase> cursor;

		xx::List<SceneItem*> sortContainer;				// for draw order by Y
		void SortContainerAdd(SceneItem* o);
		void SortContainerDraw();

		virtual void MakeUI();
		void Init();
		void Update() override;
		virtual void FixedUpdate();
		void Draw() override;
		virtual void DrawCursor();
		void OnResize(bool modeChanged_) override;
		void OnFocus(bool focused_) override;
	};

}

#pragma once
#include "game.h"

namespace Global {

	struct SceneBase;
	struct SceneItem {
		SceneBase* scene{};
		SceneItem* next{};	// for draw order sort by y
		int32_t indexAtList{ -1 };
		float y{};
		virtual bool Update() { return false; }	// return true mean release
		virtual void Draw() {}
		virtual void DrawShadow() {}
		~SceneItem() {}
	};

}

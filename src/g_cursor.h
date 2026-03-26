#pragma once
#include "game.h"

namespace Global {

	struct CursorBase {
		xx::Frame frame;
		float radius{};

		void Init();
		virtual void Update();
		virtual void Draw();
	};

}

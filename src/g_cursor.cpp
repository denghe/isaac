#include "pch.h"
#include "g_cursor.h"

namespace Global {

	void CursorBase::Init() {
		frame = gg.pics.cursor_default;
		radius = 24.f;
	}

	void CursorBase::Update() {
	}

	void CursorBase::Draw() {
		auto frameRadius = frame.uvRect.w * 0.5f;
		auto scale = radius / frameRadius * gg.scale;
		auto mp = gg.mousePos;

		auto&& shader = gg.Quad();
		//shader.Commit();
		//frame.tex->SetParm(GL_NEAREST);
		shader.DrawFrame(frame, mp, scale);
	}

}

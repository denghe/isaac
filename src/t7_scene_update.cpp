#include "pch.h"
#include "t7_.h"
#include "mm_scene.h"

namespace Test7 {

	void Scene::Update() {
		// handle inputs
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

		// todo: 鼠标滚轮缩放 camera

		// fixed update
		auto d = float(std::min((float)gg.delta, gg.cMaxDelta) * timeScale);
		timePool += d;
		while (timePool >= gg.cDelta) {
			time += gg.cDelta;
			timePool -= gg.cDelta;
			FixedUpdate();
		}
	}

	void Scene::FixedUpdate() {
		player->Update();
		UpdateItems(buckets);
		UpdateItems(playerBullets);
		UpdateItems(exploders);

		phys->Step();
		effectTexts.Update(time);

		if (/*gg.mouse[GLFW_MOUSE_BUTTON_1]
			|| */gg.mouse[GLFW_MOUSE_BUTTON_2]
			|| gg.mouse[GLFW_MOUSE_BUTTON_3]
			|| gg.mouse[GLFW_MOUSE_BUTTON_4]
			) {
			auto p = cam.ToLogicPos(gg.mousePos);
			if (p.x > cCellPixelSize && p.x < mapPixelSize.x - cCellPixelSize
				&& p.y > cCellPixelSize && p.y < mapPixelSize.y - cCellPixelSize) {

				size_t count = 0;
				if (gg.mouse[GLFW_MOUSE_BUTTON_2]) {
					count = 1;
				}
				else if (gg.mouse[GLFW_MOUSE_BUTTON_3]) {
					count = 10;
				}
				else if (gg.mouse[GLFW_MOUSE_BUTTON_4]) {
					count = 100;
				}
				for (size_t i = 0; i < count; i++) {
					auto pos = p + xx::GetRndPosDoughnut(gg.rnd, cItemMaxRadius);
					buckets.Emplace().Emplace()->Init(this, p);
				}
			}
		}
	}

}

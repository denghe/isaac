#pragma once
#include "g_scenebase.h"
#include <xx_grid2d_circle.h>

namespace Test4 {

	using SceneBase = Global::SceneBase;
	using SceneItemBase = Global::SceneItemBase;

	struct GridCache {
		XY pos{};
		float radius{};
		void operator=(SceneItemBase* p) {
			pos = p->pos;
			radius = p->radius;
		}
	};

	struct Scene : SceneBase {
		using SceneBase::SceneBase;

		void Init() override;
		void MakeUI() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void DrawItem(xx::Frame& f_, XY pos_, float scale_, xx::RGBA8 color_ = xx::RGBA8_White);

		int32_t searchCount{}, createIgnoreCount{};
		XY mapPixelSize{}, mapCenterPos{};
		static constexpr float cCellPixelSize{ 32.f };
		static constexpr int32_t cNumMaxItems{ 100000 };
		xx::List<xx::Shared<SceneItemBase>> items;
		xx::Grid2dCircle<SceneItemBase*, GridCache> itemsGrid16, itemsGrid32, itemsGrid64;
	};

}

#pragma once
#include "g_scenebase.h"
#include <xx_grid2d_circle.h>

namespace Test4 {

	using SceneBase = Global::SceneBase;
	using SceneItemBase = Global::SceneItemBase;
	struct Scene : SceneBase {
		using SceneBase::SceneBase;

		void Init() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void DrawItem(xx::Frame& f_, XY pos_, float scale_);

		XY mapSize{}, mapCenterPos{};
		static constexpr float cCellPixelSize{ 64.f };
		static constexpr int32_t cNumMaxItems{ 100000 };
		xx::List<xx::Shared<SceneItemBase>> items;
		xx::Grid2dCircle<SceneItemBase*> itemsGrid;	// todo: cache
	};

}

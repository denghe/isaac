#pragma once
#include "t7_scene.h"

namespace Test7 {

	struct Door : SceneItem {
		static constexpr int32_t cTypeId{ 2 };
		// isOpened, isCenter 这这些变量在基类里( 充分利用空间 )

		void Init(Scene* scene_, XY pos_, bool isCenter_);
		void Draw() override;
		void Dispose() override;

		void Open();
		void Close();

		// tar 进门
		virtual void Visit(SceneItem* tar_);
	};

}

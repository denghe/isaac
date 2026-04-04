#pragma once
#include "t1.h"

namespace Test1 {

	struct Scene;
	struct SceneItem;
	struct PhysSystem {
		static constexpr float eps{ 0.0001f };
		static constexpr float cVelocityDamping{ 300.f };
		static constexpr XY cGravity{ 0, 0/*100.f*/ };
		static constexpr float cResponseCoef{ 0.5f };
		static constexpr float cMaxSpeed{ 500.f / gg.cFps };

		static constexpr float cCellPixelSize{ cItemMaxRadius * 2.f };
		static constexpr float _1_cellSize{ 1.f / cCellPixelSize };
		static constexpr float cMargin{ cCellPixelSize };

		struct Node {
			XY pos, lastPos, acc;
			float radius{};
			SceneItem* item{};
		};

		struct Bucket {
			int32_t len;
			std::array<int32_t, 3> indexAtNodess;	// 3 maybe need larger
		};

		Scene* scene{};
		int32_t numRows{}, numCols{}, bucketsLen{};
		XY pixelSize{};

		xx::List<Node> nodes;
		std::unique_ptr<Bucket[]> buckets;

		Node& At(int32_t indexAtNodes_) const;

		void Init(Scene* scene_, int32_t capacity_ = 0);
		void Add(SceneItem* item_);
		void Remove(SceneItem* item_);
		void TryRemove(SceneItem* item_);

		void FillBuckets();
		void Calc();
		void CalcBB(Bucket& b1_, Bucket& b2_);
		void CalcNN(Node& n1_, Node& n2_);
		void Writeback();

		void Step();
	};

}

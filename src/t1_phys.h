#pragma once
#include "t1.h"

namespace Test1 {

	struct Scene;
	struct SceneItem;
	struct PhysSystem {
		static constexpr float eps{ 0.0001f };
		static constexpr float cVelocityDamping{ 300.f };
		//static constexpr float cGravity{ 100.f };
		static constexpr float cResponseCoef{ 0.5f };
		static constexpr float cMaxSpeed{ 500.f / gg.cFps };

		static constexpr float cCellPixelSize{ cItemMaxRadius * 2.f };
		static constexpr float _1_cellSize{ 1.f / cCellPixelSize };
		static constexpr float cMargin{ cCellPixelSize };

		struct Node {
			int32_t next;
			int32_t indexAtDatas;
			SceneItem* ud;
		};

		struct Data {
			int32_t indexAtNodes;
			float radius{};
			XY pos, lpos, acc;
		};

		struct Bucket {
			int32_t len;
			std::array<int32_t, 3> indexAtDatass;	// 3 maybe need larger
		};

		Scene* scene{};

		int32_t numRows{}, numCols{}, bucketsLen{};					// for buckets
		int32_t freeHead{ -1 }, freeCount{}, count{}, capacity{};	// for nodes
		int32_t datasLen{};											// for datas

		XY pixelSize{};

		std::unique_ptr<Node[]> nodes;
		std::unique_ptr<Data[]> datas;
		std::unique_ptr<Bucket[]> buckets;

		Data& At(int32_t indexAtNodes_);
		void Init(Scene* scene_, int32_t capacity_ = 0);
		void Reserve(int32_t capacity_);
		int32_t Add(SceneItem* ud_);	// return indexAtNodes
		void Remove(int32_t indexAtNodes_);
		void ClearBuckets();
		void FillBuckets();
		void CalcDatas();
		void Calc(Bucket& b1_, Bucket& b2_);
		void Calc(Data& d1_, Data& d2_);
		void UpdateDatas();
		void Update();
	};

}

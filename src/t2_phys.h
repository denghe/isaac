#pragma once
#include "t2.h"

namespace Test2 {

	// 物理系统，负责处理物体的运动和碰撞
	// 依赖 Scene 的 gridBuildings pixelSize
	// 依赖 SceneItem 的 pos radius indexAtGrid typeId

	// todo: 提供针对 buckets 的 范围查询 接口

	struct Scene;
	struct SceneItem;
	struct PhysSystem {
		// 速度阻尼
		static constexpr float cVelocityDamping{ 300.f };
		// 重力加速度( 每 Step 都会加在所有 item 的加速度上 )
		static constexpr XY cGravity{ 0, 0/*100.f*/ };
		// 碰撞响应系数( 数值越大，碰撞后弹得越远 )
		static constexpr float cResponseCoef{ 0.5f };
		// 物体最大速度( 超过这个速度会被强行限制 )
		static constexpr float cMaxSpeed{ 500.f / gg.cFps };
		// 格子像素尺寸
		static constexpr float cCellPixelSize{ cItemMaxRadius * 2.f };
		// 格子尺寸的倒数( 方便计算格子索引 )
		static constexpr float _1_cellSize{ 1.f / cCellPixelSize };

		// 对象池 / 缓存
		struct Node {
			// 当前位置，上一帧位置，加速度
			XY pos, lastPos, accel;
			// 碰撞半径
			float radius{};
			// 指向桶链表下一个节点, -1 代表链表末尾
			int32_t nextNodeIndex{};
			// 指向对象，方便回写数据
			SceneItem* item{};
		};

		// 指向场景，方便访问场景数据( 比如地图尺寸，物体列表 )
		Scene* scene{};
		// 网格行列数，桶数量
		int32_t numRows{}, numCols{}, bucketsLen{};
		// 像素尺寸
		XY pixelSize{};
		// 节点数组( 起到对象池 缓存的作用 )
		xx::List<Node> nodes;
		// 桶数组( 仅用于 Step 计算 )
		std::unique_ptr<int32_t[]> buckets;

		// 初始化，capacity 是 nodes 的预分配数量
		void Init(Scene* scene_, int32_t capacity_ = 1024);
		// 添加对象( 复制数据到 nodes，并填充 indexAtGrid )
		void Add(SceneItem* item_);
		// 移除对象( 交换删除并同步 indexAtGrid )
		void Remove(SceneItem* item_);
		// 尝试移除对象（如果对象不在系统中，则不执行任何操作）
		void TryRemove(SceneItem* item_);
		// 定位到节点（方便直接改数据）
		Node& At(SceneItem* item_) const;
		// 帧逻辑. 调用 FillBuckets、Calc、Writeback 完成物理计算( 遍历 nodes )
		void Step();
		// 下面是 Step 的具体步骤
		void FillBuckets();
		void Calc();
		void CalcBB(int32_t b1_, int32_t b2_);
		void CalcNN(Node& n1_, Node& n2_);
		void Writeback();
	};

}

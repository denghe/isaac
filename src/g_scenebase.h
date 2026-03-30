#pragma once
#include "game.h"
#include "g_cursor.h"

namespace Global {

	struct SceneItemBase {
		/* for copy code
		static constexpr int32_t cTypeId{ __LINE__ };
		*/

		// 用于 switch case 逻辑，避免 dynamic_cast 带来的性能问题
		// 在 Init 系列函数中填充: typeId = cTypeId
		int32_t typeId{};

		// 用于显示排序
		float y{};
		SceneItemBase* next{};

		// 所有查询行为都要检测这个标记，正在删除的对象不参与任何行为，包括被删除对象自己
		bool disposing{};

		// 再放一些常用 bool 成员在此，充分利用内存对齐
		bool flipX{}, isCenter{};
		// ...

		// 记录在容器中的位置, 方便高速随机删除
		int32_t indexAtContainer{ -1 }, indexAtGrid{ -1 };

		// 下面是是一些常用属性
		XY pos{};
		float scale{}, radians{}, radius{};
		// ...


		// 逻辑
		virtual void Update() {}

		// 绘制
		virtual void Draw() {};

		// 删除对象
		virtual void Dispose() {};

		// 试着删除对象
		void TryDispose();

		// Dispose 的中间步骤，清理资源，事件逻辑
		virtual void OnDispose() {};

		// 满足虚函数要求
		virtual ~SceneItemBase() = default;
	};

	struct SceneBase : xx::SceneBase {
		// 场景的根节点，所有 UI 都添加在这个节点下
		xx::Shared<xx::Node> ui;

		// 场景的摄像机
		xx::Camera cam;

		// 鼠标指针
		xx::Shared<CursorBase> cursor;

		// fixed update 相关
		float time{}, timePool{}, timeScale{ 1 }, timer{};

		// 默认初始化
		virtual void Init();

		// UI 初始化，默认创建一个根节点
		virtual void MakeUI();

		// 帧逻辑
		void Update() override;

		// 固定时间帧逻辑( 每秒调用 gg.cFps 次 )
		virtual void FixedUpdate() {}

		// 绘制
		void Draw() override;

		// 绘制鼠标指针
		virtual void DrawCursor();

		// 处理窗口大小变化，默认调整 UI 和摄像机
		void OnResize(bool modeChanged_) override;

		// 处理窗口焦点变化，默认调整全局音量
		void OnFocus(bool focused_) override;

		virtual ~SceneBase() = default;

		// y坐标排序容器
		xx::List<SceneItemBase*> sortContainer;

		// 往 y排序容器 添加一个对象
		void SortContainerAdd(SceneItemBase* o_);

		// 绘制 y排序容器 中的对象，绘制完成后清空容器
		void SortContainerDraw();

	};

}

/* for copy code

	struct Scene : Global::SceneBase {
		using Base = Global::SceneBase;
		using Base::Base;
		using SceneItemBase = Global::SceneItemBase;

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

	struct SceneItem : Global::SceneItemBase {
		Scene* scene{};

		void SceneItemInit(int32_t typeId_, Scene* scene_, XY pos_, float radius_) {
			scene = scene_;
			typeId = typeId_;
			indexAtContainer = scene->items.len - 1;
			assert(scene_->items[indexAtContainer].pointer == this);
			pos = pos_;
			radius = radius_;
			scene_->itemsGrid.Add(indexAtGrid, this);
		}

		void Dispose() override {
			assert(scene);
			assert(!disposing);
			assert(indexAtContainer != -1);
			assert(scene->items[indexAtContainer].pointer == this);
			assert(scene->itemsGrid.ValueAt(indexAtGrid) == this);

			// 设置标记
			disposing = true;

			// 从 grid 中移除对象，避免被查询到
			if (indexAtGrid != -1) {
				scene->itemsGrid.Remove(indexAtGrid, this);
			}

			// 进一步释放资源，事件逻辑
			OnDispose();

			// 从容器中移除对象( 释放内存 )
			auto i = indexAtContainer;
			scene->items.Back()->indexAtContainer = i;
			indexAtContainer = -1;
			scene->items.SwapRemoveAt(i);	// unsafe: release memory
		}
	};

*/
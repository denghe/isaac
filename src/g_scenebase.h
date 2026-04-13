#pragma once
#include "game.h"
#include "g_cursor.h"

namespace Global {

	struct SceneItemBase {
		/* for copy code
		static constexpr int32_t cTypeId{ ???? };	// 跨类唯一
		*/

		// 用于 switch case 逻辑，避免 dynamic_cast 带来的性能问题
		// 在 Init 系列函数中填充: typeId = cTypeId
		int32_t typeId{};

		// 用于显示排序
		float y{};
		SceneItemBase* next{};

		// 放一些常用 bool 成员在此，充分利用内存对齐
		// 所有查询行为都要检测这个标记，正在删除的对象不参与任何行为，包括被删除对象自己
		bool disposing{}
			// 显示时反转左右
			, flipX{}
			// 像门之类的对象，如果是位于中间那格，该标记将为 true
			, isCenter{}
			// 门的开关状态。如果为 true 则不参与物理碰撞
			, isOpened{};

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
		virtual void DrawShadow() {};
		virtual void DrawLight() {};

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

		// 倒序更新容器中的对象，允许对象在 Update 中自杀或删除其他对象
		template<typename Items>
		XX_INLINE static void UpdateItems(Items& items_) {
			for (int i = items_.len - 1; i >= 0;) {

				// 帧逻辑. 可能会自杀，可能会删除其他对象
				items_[i]->Update();

				if (i >= items_.len) {
					i = items_.len - 1;
				}
				else {
					--i;
				}
			}
		}

	};

}

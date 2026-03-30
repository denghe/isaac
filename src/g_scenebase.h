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

		// 删除对象( 首先会检查标记，避免重复删除 )
		void TryDispose() {
			if (disposing) return;
			Dispose();
		}

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
		virtual void Init() {
			cam.Init(gg.scale, 1.f, gg.designSize / 2);
			sortContainer.Resize<true>((int32_t)gg.designSize.y);
			cursor.Emplace()->Init();
			MakeUI();
		}

		// UI 初始化，默认创建一个根节点
		virtual void MakeUI() {
			ui.Emplace()->InitRoot(gg.scale);
		}

		// 帧逻辑
		void Update() override {
			/* for copy code
			// handle inputs
			if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
				gg.MakeScene<MainMenu::Scene>()->Init();
				return;
			}
			*/

			// fixed update
			auto d = float(std::min((float)gg.delta, gg.cMaxDelta) * timeScale);
			timePool += d;
			while (timePool >= gg.cDelta) {
				time += gg.cDelta;
				timePool -= gg.cDelta;
				FixedUpdate();
			}
		}

		// 固定时间帧逻辑( 每秒调用 gg.cFps 次 )
		virtual void FixedUpdate() {}

		// 绘制
		void Draw() override {
			// draw ui
			gg.DrawNode(ui);
		}

		// 绘制鼠标指针
		virtual void DrawCursor() {
			cursor->Draw();
		}

		// 处理窗口大小变化，默认调整 UI 和摄像机
		void OnResize(bool modeChanged_) override {
			ui->Resize(gg.scale);
			cam.SetBaseScale(gg.scale);
		}

		// 处理窗口焦点变化，默认调整全局音量
		void OnFocus(bool focused_) override {
			if (focused_) {
				gg.sound.SetGlobalVolume(1);
			}
			else {
				gg.sound.SetGlobalVolume(0);
			}
		}

		virtual ~SceneBase() = default;


		// 下面这些东西为 按 y 坐标排序服务
		xx::List<SceneItemBase*> sortContainer;

		void SortContainerAdd(SceneItemBase* o_) {
			auto& slot = sortContainer[(int32_t)o_->y];
			o_->next = slot;
			slot = o_;
		}

		void SortContainerDraw() {
			for (auto o : sortContainer) {
				while (o) {
					o->Draw();
					o = o->next;
				}
			}
			memset(sortContainer.buf, 0, sortContainer.len * sizeof(void*));
		}

	};

}

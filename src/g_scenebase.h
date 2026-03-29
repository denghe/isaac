#pragma once
#include "game.h"
#include "g_cursor.h"

namespace Global {

	/*
	* for copy code
	
	struct Scene : Global::SceneBase<Scene> {
		using Base = Global::SceneBase<Scene>;
		using Base::Base;
		using SceneItem = Global::SceneItem<Scene>;
		// todo: ui for show result

		xx::List<xx::Shared<SceneItem>> items;

		void Init() override;
		void MakeUI() override;
		void Update() override;
		void FixedUpdate() override;
		void Draw() override;
		void DrawCursor() override;
		void OnResize(bool modeChanged_) override;
		void OnFocus(bool focused_) override;
	};
	using SceneItem = Scene::SceneItem;
	
	*/

	struct SceneBase_ : xx::SceneBase {
		xx::Shared<xx::Node> ui;
		xx::Camera cam;
		float time{}, timePool{}, timeScale{ 1 }, timer{};
		xx::Shared<CursorBase> cursor;

		virtual void Init() {}

		virtual void MakeUI() {
			ui.Emplace()->InitRoot(gg.scale);
		}

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

		virtual void FixedUpdate() {}

		void Draw() override {
			// draw ui
			gg.DrawNode(ui);
		}

		virtual void DrawCursor() {
			cursor->Draw();
		}

		void OnResize(bool modeChanged_) override {
			ui->Resize(gg.scale);
			cam.SetBaseScale(gg.scale);
		}

		void OnFocus(bool focused_) override {
			if (focused_) {
				gg.sound.SetGlobalVolume(1);
			}
			else {
				gg.sound.SetGlobalVolume(0);
			}
		}

		virtual ~SceneBase_() = default;
	};

	template<typename DerivedSceneBaseEx>
	struct SceneItemBase {
		/* for copy code
		static constexpr int32_t cTypeId{ __LINE__ };
		void Init(Scene* scene_) { BaseInit<std::remove_pointer_t<decltype(this)>>(scene_); }
		*/
		static constexpr int32_t cTypeId{};

		// 用于 switch case 逻辑，避免 dynamic_cast 带来的性能问题
		// 在 Init 系列函数中填充: typeId = cTypeId
		int32_t typeId{};

		// 用于显示排序
		float y{};
		SceneItemBase* next{};

		// 方便访问场景
		DerivedSceneBaseEx* scene{};

		// 记录在容器中的位置, 方便高速随机删除
		int32_t indexAtContainer{ -1 }, indexAtGrid{ -1 };

		// 下面是是一些常用属性
		XY pos{};
		float scale{}, radians{}, radius{};
		bool flipX{}, isCenter{};

		// 所有查询行为都要检测这个标记，正在删除的对象不参与任何行为，包括被删除对象自己
		bool disposing{};

		// 逻辑
		virtual void Update() {}

		// 绘制
		virtual void Draw() {};

		// 删除对象
		virtual void Dispose() {
			/* for copy code
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
			*/
		};

		// 删除对象( 首先会检查标记，避免重复删除 )
		void TryDispose() {
			if (disposing) return;
			Dispose();
		}

		// Dispose 的中间步骤，清理资源，事件逻辑
		virtual void OnDispose() {};

		virtual ~SceneItemBase() = default;
	};

	template<typename Derived>
	struct SceneBase : SceneBase_ {
		using SI = SceneItemBase<Derived>;

		void Init() override {
			cam.Init(gg.scale, 1.f, gg.designSize / 2);
			sortContainer.Resize<true>((int32_t)gg.designSize.y);
			cursor.Emplace()->Init();
			MakeUI();
		}

		// for draw order by Y
		xx::List<SI*> sortContainer;

		void SortContainerAdd(SI* o_) {
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

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

		virtual void Init();
		virtual void MakeUI();
		void Update() override;
		virtual void FixedUpdate();
		void Draw() override;
		virtual void DrawCursor();
		void OnResize(bool modeChanged_) override;
		void OnFocus(bool focused_) override;
		virtual ~SceneBase_();
	};

	template<typename DerivedSceneBaseEx>
	struct SceneItem {
		static constexpr int32_t cTypeId{};
		DerivedSceneBaseEx* scene{};
		SceneItem* next{};
		int32_t typeId{};	// fill by Init: typeId = cTypeId
		XY pos{};
		float y{};
		float scale{}, radians{}, radius{};
		bool flipX{}, isCenter{}, disposing{};
		int32_t indexAtContainer{ -1 };
		int32_t indexAtGrid{ -1 };
		virtual bool Update() { return false; }
		virtual void Draw() {};
		virtual void Dispose() {};	// unsafe: container.swapRemove( this )
		virtual ~SceneItem() {};
	};

	template<typename Derived>
	struct SceneBase : SceneBase_ {
		using SI = SceneItem<Derived>;

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

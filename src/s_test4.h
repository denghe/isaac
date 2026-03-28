#pragma once
#include "game.h"
#include "s_mainmenu.h"
#include "g_scenebase.h"

// 实现一套高效的 ABC 随便乱删的数组遍历逻辑 ver 2
// todo: 复杂逻辑，update 模拟
// 两只怪发射子弹相互攻击。本体死亡，其发射的子弹也会消失

namespace Test4 {

	struct Scene;
	struct ABCBase {
		// for copy code
		// static constexpr int32_t cTypeId{ __LINE__ };
		// void Init(Scene* scene_) { InitBase<std::remove_pointer_t<decltype(this)>>(scene_); }

		int32_t typeId{};
		int32_t indexAtContainer{ -1 };
		Scene* scene{};

		bool disposing{};	// removing flag
		void TryDispose();
		virtual void Dispose();
		virtual void OnDispose() {};

		template<typename Derived> void InitBase(Scene* scene_);
		virtual ~ABCBase() = default;
		virtual void Update() {};
	};

	struct Scene : Global::SceneBase<Scene> {
		using Base = Global::SceneBase<Scene>;
		void Init() override;
		void Update() override {
			if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
				gg.MakeScene<MainMenu::Scene>()->Init();
				return;
			}
		}

		xx::List<xx::Shared<ABCBase>> items;	// ABC container
	};

	template<typename Derived>
	void ABCBase::InitBase(Scene* scene_) {
		scene = scene_;
		typeId = Derived::cTypeId;
		indexAtContainer = scene->items.len - 1;
	}

	inline void ABCBase::TryDispose() {
		if (disposing) return;
		Dispose();
	}

	inline void ABCBase::Dispose() {
		assert(scene);
		assert(!disposing);
		assert(indexAtContainer != -1);
		assert(scene->items[indexAtContainer].pointer == this);
		disposing = true;	// set removing flag
		// todo: more resource release here
		OnDispose();
		auto i = indexAtContainer;
		scene->items.Back()->indexAtContainer = i;
		indexAtContainer = -1;
		scene->items.SwapRemoveAt(i);	// unsafe: release memory
	}

	struct A : ABCBase {
		static constexpr int32_t cTypeId{ __LINE__ };
		void Init(Scene* scene_) { InitBase<std::remove_pointer_t<decltype(this)>>(scene_); }
		void OnDispose() override;
	};

	struct B : ABCBase {
		static constexpr int32_t cTypeId{ __LINE__ };
		void Init(Scene* scene_) { InitBase<std::remove_pointer_t<decltype(this)>>(scene_); }
		void OnDispose() override;
	};

	struct C : ABCBase {
		static constexpr int32_t cTypeId{ __LINE__ };
		void Init(Scene* scene_) { InitBase<std::remove_pointer_t<decltype(this)>>(scene_); }
		void OnDispose() override;
	};

	inline void A::OnDispose() {
	}

	inline void B::OnDispose() {
	}

	inline void C::OnDispose() {
	}

	inline void Scene::Init() {
		Base::Init();
		items.Reserve(100000);	// 重要：需要大到足以避免 Resize 导致的指针失效

		auto secs = xx::NowEpochSeconds();
		uint64_t counter{};

		for (size_t j = 0; j < 10; j++) {

			for (size_t k = 0; k < 10000; k++) {
				items.Emplace().Emplace<A>()->Init(this);
				items.Emplace().Emplace<B>()->Init(this);
				items.Emplace().Emplace<C>()->Init(this);
			}

			//xx::CoutN("items.len = ", items.len);
			counter += items.len;

			// 模拟遍历
			for (int i = items.len - 1; i >= 0;) {
				//xx::CoutN("i = ", i);
				//items[i]->TryDispose();	// logic
				if (items[i]->typeId == A::cTypeId) items[i]->TryDispose();	// logic

				// 注意：上面 TryDispose 可能会删除当前对象，也可能会删除其他对象（比如 A 会删掉 B 和 C），所以 i 的更新逻辑如下：
				if (i >= items.len) {
					i = items.len - 1;
				}
				else {
					--i;
				}
			}

			//xx::CoutN("items.len = ", items.len);
			counter += items.len;

			items.Clear();
		}
		secs = xx::NowEpochSeconds(secs);
		xx::CoutN("test2 counter = ", counter, ", secs = ", secs);

	}

}

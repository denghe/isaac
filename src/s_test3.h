#pragma once
#include "game.h"
#include "s_mainmenu.h"
#include "g_scenebase.h"

// 实现一套高效的 ABC 随便乱删的数组遍历逻辑（ 最后再删版 ）

namespace Test3 {

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

	struct Scene : Global::SceneBase {
		using Base = Global::SceneBase;
		void Init() override;
		void Update() override {
			if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
				gg.MakeScene<MainMenu::Scene>()->Init();
				return;
			}
		}

		xx::List<xx::Shared<ABCBase>> items;	// ABC container

		// 模拟某种针对 items 的随机删除逻辑
		template<typename T>
		void DisposeItemByType() {
			for (int i = 0; i < items.len; ++i) {
				auto o = items[i].pointer;
				if (o->typeId == T::cTypeId) {
					o->TryDispose();
				}
			}
		}
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
		//auto i = indexAtContainer;
		//scene->items.Back()->indexAtContainer = i;
		//indexAtContainer = -1;
		//scene->items.SwapRemoveAt(i);	// unsafe: release memory
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
		scene->DisposeItemByType<B>();
		//scene->DisposeItemByType<C>();
	}

	inline void B::OnDispose() {
		scene->DisposeItemByType<A>();
		//scene->DisposeItemByType<C>();
	}

	inline void C::OnDispose() {
		//scene->DisposeItemByType<A>();
		//scene->DisposeItemByType<B>();
	}

	inline void Scene::Init() {
		Base::Init();
		items.Reserve(100000);	// 重要：需要大到足以避免 Resize 导致的指针失效

		auto secs = xx::NowEpochSeconds();
		uint64_t counter{};

		for (size_t j = 0; j < 10; j++) {

			for (size_t k = 0; k < 100; k++) {
				items.Emplace().Emplace<A>()->Init(this);
				items.Emplace().Emplace<B>()->Init(this);
				items.Emplace().Emplace<C>()->Init(this);
			}

			//xx::CoutN("items.len = ", items.len);
			counter += items.len;

			// 模拟遍历
			for (int i = 0; i < items.len; ++i) {
				//xx::CoutN("i = ", i);
				//items[i]->TryDispose();	// logic
				if (items[i]->typeId == A::cTypeId) items[i]->TryDispose();	// logic
			}

			// 清理之前被 dispose 的对象
			int32_t i_dest = 0;
			for (int32_t i_src = 0; i_src != items.len; ++i_src) {
				ABCBase& item = *items[i_src].pointer;
				if (item.disposing) {
					item.indexAtContainer = -1;
				}
				else {
					item.indexAtContainer = i_dest;
					items[i_dest++] = std::move(items[i_src]);
				}
			}
			items.Resize(i_dest);


			//xx::CoutN("items.len = ", items.len);
			counter += items.len;

			items.Clear();
		}

		secs = xx::NowEpochSeconds(secs);
		xx::CoutN("test3 counter = ", counter, ", secs = ", secs);

	}

}

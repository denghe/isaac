#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void SceneItem::CallDisposeCallbacks() {
		for (auto i = disposeCallbacks.len - 1; i >= 0; --i) {
			disposeCallbacks[i]();
		}
	}

}

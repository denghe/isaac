#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::OpenDoors() {
		for (auto i = doors.len - 1; i >= 0; --i) {
			doors[i]->Dispose();
		}
	}

}

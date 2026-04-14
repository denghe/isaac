#include "pch.h"
#include "t7_.h"

namespace Test7 {

	void Scene::Init_() {
		frameBuffer.Init();
		effectTexts.Init(&cam, 10000);
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;
		//GenMap();
		GenWorld();
	}

	void Scene::Init() {
		Init_();
		//GenPlayer(7, 4);
	}

	void Scene::Init(Scene* last_) {
		Init_();
		// 从旧场将玩家挪过来
		last_->player->MoveTo(this);
		// todo: 转场
		// 修正玩家坐标
		// 需要算相对应的门的位置？
		// 转场期间，update 逻辑暂停执行，只播放转场 camera 动画。
		// 转场完毕之后 怪才"长出来"，一开始在地表有占位符
	}
}

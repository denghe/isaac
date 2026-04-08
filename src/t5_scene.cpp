#include "pch.h"
#include "t5_.h"
#include "mm_scene.h"

namespace Test5 {

	void Scene::Init() {
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();
		cursor->frame = gg.pics.cursor_cross;

#if 0
		mapSize = cRoom1x2PixelSize;
		cam.Init(gg.scale, gg.designSize.x / cRoom1x1PixelSize.x, mapSize / 2);
		sortContainer.Resize<true>((int32_t)cRoomMaxPixelSize.y);
		gridBuildings.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize)
			, std::ceilf(mapSize.x / cCellPixelSize));
		phys.Emplace()->Init(this);

		floorMaskFB.Init();
		floorMaskTex.Emplace()->Make(mapSize);

		//                     1 1 1 1 1
		// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4
		// [][][][][][][]  [][][][][][][]	0
		// []                          []	1
		// []                          []	2
		// []                          []	3
		//                               	4
		// []                          []	5
		// []                          []	6
		// []                          []	7
		// []                          []	8
		// []                          []	9
		// []                          []	10
		//                               	11
		// []                          []	12
		// []                          []	13
		// []                          []	14
		// [][][][][][][]  [][][][][][][]	15

		// 顶边
		GenWallHorizontal(0, 6, 0);
		GenDoorHorizontal(7, 0);
		GenWallHorizontal(8, 14, 0);
		// 底边
		GenWallHorizontal(0, 6, 15);
		GenDoorHorizontal(7, 15);
		GenWallHorizontal(8, 14, 15);
		// 左边
		GenWallVertical(0, 1, 3, true, false);
		GenDoorVertical(0, 4);
		GenWallVertical(0, 5, 10, false, false);
		GenDoorVertical(0, 11);
		GenWallVertical(0, 12, 14, false, true);
		// 右边
		GenWallVertical(14, 1, 3, true, false);
		GenDoorVertical(14, 4);
		GenWallVertical(14, 5, 10, false, false);
		GenDoorVertical(14, 11);
		GenWallVertical(14, 12, 14, false, true);

		GenPlayer(7, 7);

		for (int x = 1; x <= 13; ++x) {
			for (int y = 1; y <= 14; ++y) {
				if (x == 7 && y == 7) continue;
				GenBucket(x, y);
			}
		}
#else
		mapSize = cRoom2x2PixelSize;
		cam.Init(gg.scale, gg.designSize.x / cRoom1x1PixelSize.x, mapSize / 2);
		sortContainer.Resize<true>((int32_t)cRoomMaxPixelSize.y);
		gridBuildings.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize)
			, std::ceilf(mapSize.x / cCellPixelSize));
		phys.Emplace()->Init(this);

		floorMaskFB.Init();
		floorMaskTex.Emplace()->Make(mapSize);

		//                     1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2
		// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 
		// [][][][][][][]  [][][][][][][][][][][][]  [][][][][][][]	0
		// []                                                    []	1
		// []                                                    []	2
		// []                                                    []	3
		//                                                         	4
		// []                                                    []	5
		// []                                                    []	6
		// []                                                    []	7
		// []                                                    []	8
		// []                                                    []	9
		// []                                                    []	10
		//                                                         	11
		// []                                                    []	12
		// []                                                    []	13
		// []                                                    []	14
		// [][][][][][][]  [][][][][][][][][][][][]  [][][][][][][]	15

		// 顶边
		GenWallHorizontal(0, 6, 0);
		GenDoorHorizontal(7, 0);
		GenWallHorizontal(8, 19, 0);
		GenDoorHorizontal(20, 0);
		GenWallHorizontal(21, 27, 0);
		// 底边
		GenWallHorizontal(0, 6, 15);
		GenDoorHorizontal(7, 15);
		GenWallHorizontal(8, 19, 15);
		GenDoorHorizontal(20, 15);
		GenWallHorizontal(21, 27, 15);
		// 左边
		GenWallVertical(0, 1, 3, true, false);
		GenDoorVertical(0, 4);
		GenWallVertical(0, 5, 10, false, false);
		GenDoorVertical(0, 11);
		GenWallVertical(0, 12, 14, false, true);
		// 右边
		GenWallVertical(27, 1, 3, true, false);
		GenDoorVertical(27, 4);
		GenWallVertical(27, 5, 10, false, false);
		GenDoorVertical(27, 11);
		GenWallVertical(27, 12, 14, false, true);

		GenPlayer(13, 7);

		for (int x = 1; x <= cRoom2x2ContentSize.x; ++x) {
			for (int y = 1; y <= cRoom2x2ContentSize.y; ++y) {
				if (x == 13 && y == 7) continue;
				GenBucket(x, y);
			}
		}
#endif
	}

	void Scene::Update() {
		// handle inputs
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

		// todo: 鼠标滚轮缩放 camera

		// fixed update
		auto d = float(std::min((float)gg.delta, gg.cMaxDelta) * timeScale);
		timePool += d;
		while (timePool >= gg.cDelta) {
			time += gg.cDelta;
			timePool -= gg.cDelta;
			FixedUpdate();
		}
	}

	void Scene::FixedUpdate() {
		player->Update();
		UpdateItems(buckets);
		UpdateItems(playerBullets);
		UpdateItems(exploders);

		phys->Step();

		if (/*gg.mouse[GLFW_MOUSE_BUTTON_1]
			|| */gg.mouse[GLFW_MOUSE_BUTTON_2]
			|| gg.mouse[GLFW_MOUSE_BUTTON_3]
			|| gg.mouse[GLFW_MOUSE_BUTTON_4]
			) {
			auto p = cam.ToLogicPos(gg.mousePos);
			if (p.x > cCellPixelSize && p.x < mapSize.x - cCellPixelSize
				&& p.y > cCellPixelSize && p.y < mapSize.y - cCellPixelSize) {

				size_t count = 0;
				if (gg.mouse[GLFW_MOUSE_BUTTON_2]) {
					count = 1;
				}
				else if (gg.mouse[GLFW_MOUSE_BUTTON_3]) {
					count = 10;
				}
				else if (gg.mouse[GLFW_MOUSE_BUTTON_4]) {
					count = 100;
				}
				for (size_t i = 0; i < count; i++) {
					auto pos = p + xx::GetRndPosDoughnut(gg.rnd, cItemMaxRadius);
					buckets.Emplace().Emplace()->Init(this, p);
				}

				//if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
				//	// 查找鼠标位置的 bucket，爆炸它
				//	auto cri = phys->PosToCRIndex(p);
				//	phys->ForeachBy9(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->void {
				//		if (o.value->typeId == Bucket::cTypeId) {
				//			auto d = o.cache.pos - p;
				//			auto mag2 = d.x * d.x + d.y * d.y;
				//			auto r = o.cache.radius + 10.f;	// + 10.f 鼠标范围
				//			auto rr = r * r;
				//			if (mag2 < rr) {
				//				((Bucket*)o.value)->Explode();
				//			}
				//		}
				//	});
				//}
			}
		}
	}

	void Scene::Draw() {
		// bg color
		//gg.Quad().DrawTinyFrame(gg.embed.shape_dot, 0, 0.5f, gg.windowSize, 0, 1, { 0x81,0xbd,0x57,255 });

		// 大地图镜头控制: 如果已经接近地图边缘，则不继续修改 original 而是让角色往边缘移动
		xx::FromTo<XY> originalRange{};
		// 先算 camera 的 original 值范围. 如果地图比屏幕尺寸大，则范围为差值, 否则将地图居中
		auto ws = gg.windowSize / cam.scale;
		if (mapSize.x > ws.x) {
			originalRange.from.x = mapSize.x * 0.5f - (mapSize.x - ws.x) * 0.5f;
			originalRange.to.x = mapSize.x * 0.5f + (mapSize.x - ws.x) * 0.5f;
		}
		else {
			originalRange.from.x = originalRange.to.x = mapSize.x * 0.5f;
		}
		if (mapSize.y > ws.y) {
			originalRange.from.y = mapSize.y * 0.5f - (mapSize.y - ws.y) * 0.5f;
			originalRange.to.y = mapSize.y * 0.5f + (mapSize.y - ws.y) * 0.5f;
		}
		else {
			originalRange.from.y = originalRange.to.y = mapSize.y * 0.5f;
		}
		// 用值范围限制 original 值
		auto original = player->pos;
		if (original.x < originalRange.from.x) original.x = originalRange.from.x;
		else if (original.x > originalRange.to.x) original.x = originalRange.to.x;
		if (original.y < originalRange.from.y) original.y = originalRange.from.y;
		else if (original.y > originalRange.to.y) original.y = originalRange.to.y;
		cam.SetOriginal(original);

		// 绘制地板纹理
		for (int32_t i = 0; i < gridBuildings.numRows; ++i) {
			for (int32_t j = 0; j < gridBuildings.numCols; ++j) {
				XY p{ j * cCellPixelSize, i * cCellPixelSize };
				gg.Quad().DrawTinyFrame(gg.pics.c128_floor, cam.ToGLPos(p), {0,1}, cam.scale, 0);
			}
		}

		// 背景部分绘制
		for (auto& o : walls) o->Draw();
		for (auto& o : doors) o->Draw();

		// 地板 render texture ( 血迹, 爆炸痕迹等 )
		if (floorMasks.len) {
			// 将数据里的东西画到 render texture 上并清空
			floorMaskFB.DrawTo(floorMaskTex, {}, [this] {
				// 这里的绘制坐标不受 cam 影响, 直接映射到逻辑地图. 以贴图左上角为 0,0 开始绘制
				auto leftTopPos = mapSize * XY{ -0.5f, 0.5f };
				for (auto& o : floorMasks) {
					auto p = leftTopPos + o.pos.FlipY();
					gg.Quad().DrawFrame(o.frame, p, o.scale, o.radians, o.colorplus, o.color);
				}
			});
			floorMasks.Clear();
		}
		gg.Quad().Draw(*floorMaskTex, *floorMaskTex, cam.ToGLPos(mapSize * 0.5f), 0.5f, cam.scale);
		// todo: 换 quad ex 并按屏幕实际尺寸裁切uv 以节省填充率?

		// sort order by y
		SortContainerAdd(player.pointer);
		for (auto& o : buckets) SortContainerAdd(o.pointer);
		for (auto& o : playerBullets) SortContainerAdd(o.pointer);
		SortContainerDraw();

		// todo: 光照层 ?
		// 爆炸特效覆盖在最上层
		for (auto& o : exploders) o->Draw();

		gg.uiText->SetText(xx::ToString("num items = ", buckets.len));
		gg.DrawNode(ui);
	}

	void Scene::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale * cUIScale);
		cam.SetBaseScale(gg.scale);
	}

}

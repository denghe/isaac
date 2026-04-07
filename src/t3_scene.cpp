#include "pch.h"
#include "t3_.h"
#include "mm_scene.h"

namespace Test3 {

	void Scene::GenWallHorizontal(int32_t xFrom_, int32_t xTo_, int32_t y_, bool leftOverflow_, bool rightOverflow_) {
		for (int32_t x = xFrom_; x <= xTo_; ++x) {
			walls.Emplace().Emplace()->Init(this, XY{ x, y_ } * cCellPixelSize + cCellPixelHalfSize);
		}
		for (int32_t x = xFrom_; x < xTo_; ++x) {
			walls.Emplace().Emplace()->Init(this, XY{ x, y_ } * cCellPixelSize + XY{ cCellPixelSize, cCellPixelHalfSize });
		}
		if (leftOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ xFrom_, y_ } * cCellPixelSize + XY{ 0, cCellPixelHalfSize });
		}
		if (rightOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ xTo_, y_ } * cCellPixelSize + XY{ cCellPixelSize, cCellPixelHalfSize });
		}
	}

	void Scene::GenWallVertical(int32_t x_, int32_t yFrom_, int32_t yTo_, bool topOverflow_, bool bottomOverflow_) {
		for (int32_t y = yFrom_; y <= yTo_; ++y) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, y } * cCellPixelSize + cCellPixelHalfSize);
		}
		for (int32_t y = yFrom_; y < yTo_; ++y) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, y } * cCellPixelSize + XY{ cCellPixelHalfSize, cCellPixelSize });
		}
		if (topOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, yFrom_ } * cCellPixelSize + XY{ cCellPixelHalfSize, 0 });
		}
		if (bottomOverflow_) {
			walls.Emplace().Emplace()->Init(this, XY{ x_, yTo_ } * cCellPixelSize + XY{ cCellPixelHalfSize, cCellPixelSize });
		}
	}

	void Scene::GenDoorHorizontal(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		doors.Emplace().Emplace()->Init(this, p + XY{ cCellPixelHalfSize, 0 }, false);
		doors.Emplace().Emplace()->Init(this, p + XY{ -cCellPixelHalfSize, 0 }, false);
		doors.Emplace().Emplace()->Init(this, p, true);
	}

	void Scene::GenDoorVertical(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		doors.Emplace().Emplace()->Init(this, p + XY{ 0, cCellPixelHalfSize }, false);
		doors.Emplace().Emplace()->Init(this, p + XY{ 0, -cCellPixelHalfSize }, false);
		doors.Emplace().Emplace()->Init(this, p, true);
	}

	void Scene::GenPlayer(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		players.Emplace().Emplace()->Init(this, p);
	}

	void Scene::GenBucket(int32_t x_, int32_t y_) {
		auto p = XY{ x_, y_ } * cCellPixelSize + cCellPixelHalfSize;
		buckets.Emplace().Emplace()->Init(this, p);
	}

	void Scene::Init() {
		ui.Emplace()->InitRoot(gg.scale * cUIScale);
		cursor.Emplace()->Init();

		mapSize = cRoom1x1PixelSize;
		cam.Init(gg.scale, gg.designSize.y / mapSize.y, mapSize / 2);
		sortContainer.Resize<true>((int32_t)cRoomMaxPixelSize.y);
		gridBuildings.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize), std::ceilf(mapSize.x / cCellPixelSize));
		//gridItems.Init(cCellPixelSize, std::ceilf(mapSize.y / cCellPixelSize), std::ceilf(mapSize.x / cCellPixelSize));
		phys.Emplace()->Init(this);

		floorMaskFB.Init();
		floorMaskTex.Emplace()->Make(mapSize);


		// [][][][][][][]  [][][][][][][]
		// []                          []
		// []                          []
		// []                          []
		//                               
		// []                          []
		// []                          []
		// []                          []
		// [][][][][][][]  [][][][][][][]

		GenWallHorizontal(0, 6, 0);
		GenDoorHorizontal(7, 0);
		GenWallHorizontal(8, 14, 0);

		GenWallHorizontal(0, 6, 8);
		GenDoorHorizontal(7, 8);
		GenWallHorizontal(8, 14, 8);

		GenWallVertical(0, 1, 3, true, false);
		GenDoorVertical(0, 4);
		GenWallVertical(0, 5, 7, false, true);
		GenWallVertical(14, 1, 3, true, false);
		GenDoorVertical(14, 4);
		GenWallVertical(14, 5, 7, false, true);

		GenPlayer(7, 4);

		for (int x = 1; x <= 13; ++x) {
			for (int y = 1; y <= 7; ++y) {
				if (x == 7 && y == 4) continue;
				GenBucket(x, y);
			}
		}

		//GenBucket(6, 4);
		//GenBucket(8, 4);
		//GenBucket(7, 3);
		//GenBucket(7, 5);
	}

	void Scene::Update() {
		// handle inputs
		if (gg.keyboard[GLFW_KEY_ESCAPE](0.2f)) {
			gg.MakeScene<MainMenu::Scene>()->Init();
			return;
		}

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
		UpdateItems(players);
		UpdateItems(buckets);
		UpdateItems(exploders);

		phys->Step();

		if (gg.mouse[GLFW_MOUSE_BUTTON_1]
			|| gg.mouse[GLFW_MOUSE_BUTTON_2]
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

				if (gg.mouse[GLFW_MOUSE_BUTTON_1]) {
					// 查找鼠标位置的 bucket，爆炸它
					auto cri = phys->PosToCRIndex(p);
					phys->ForeachBy9(cri.y, cri.x, [&](PhysSystem::Node& o, float range)->void {
						if (o.value->typeId == Bucket::cTypeId) {
							auto d = o.cache.pos - p;
							auto mag2 = d.x * d.x + d.y * d.y;
							auto r = o.cache.radius + 10.f;	// + 10.f 鼠标范围
							auto rr = r * r;
							if (mag2 < rr) {
								((Bucket*)o.value)->Explode();
							}
						}
					});
				}
			}
		}
	}

	void Scene::Draw() {
		// bg color
		//gg.Quad().DrawTinyFrame(gg.embed.shape_dot, 0, 0.5f, gg.windowSize, 0, 1, { 0x81,0xbd,0x57,255 });

		// todo: 大地图镜头控制

		// 绘制地板纹理
		for (int32_t i = 0; i < gridBuildings.numRows; ++i) {
			for (int32_t j = 0; j < gridBuildings.numCols; ++j) {
				XY p{ j * cCellPixelSize, i * cCellPixelSize };
				gg.Quad().DrawTinyFrame(gg.pics.cell_floor, cam.ToGLPos(p), {0,1}, cam.scale, 0);
			}
		}

		// 背景部分绘制
		for (auto& o : walls) o->Draw();
		for (auto& o : doors) o->Draw();

		// 地板 render texture ( 血迹, 爆炸痕迹等 )
		if (floorMasks.len) {
			// 将数据里的东西画到 render texture 上并清空
			floorMaskFB.DrawTo(floorMaskTex, {}, [this] {
				for (auto& o : floorMasks) {
					gg.Quad().DrawFrame(o.frame, cam.ToGLPos(o.pos), cam.scale * o.scale, o.radians, o.colorplus, o.color);
				}
			});
			floorMasks.Clear();
		}
		gg.Quad().Draw(*floorMaskTex, *floorMaskTex, {});

		// sort order by y
		for (auto& o : players) SortContainerAdd(o.pointer);
		for (auto& o : buckets) SortContainerAdd(o.pointer);
		SortContainerDraw();

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

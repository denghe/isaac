#include "pch.h"
#include "t6_.h"
#include "mm_scene.h"

namespace Test6 {

	void Scene::Draw() {
		// todo: 根据 window 实际显示范围 判断裁切

		// bg color
		//gg.Quad().DrawTinyFrame(gg.embed.shape_dot, 0, 0.5f, gg.windowSize, 0, 1, { 0x81,0xbd,0x57,255 });

		// 大地图镜头控制: 如果已经接近地图边缘，则不继续修改 original 而是让角色往边缘移动
		xx::FromTo<XY> originalRange{};
		// 先算 camera 的 original 值范围. 如果地图比屏幕尺寸大，则范围为差值, 否则将地图居中
		auto ws = gg.windowSize / cam.scale;
		if (mapPixelSize.x > ws.x) {
			originalRange.from.x = mapPixelSize.x * 0.5f - (mapPixelSize.x - ws.x) * 0.5f;
			originalRange.to.x = mapPixelSize.x * 0.5f + (mapPixelSize.x - ws.x) * 0.5f;
		}
		else {
			originalRange.from.x = originalRange.to.x = mapPixelSize.x * 0.5f;
		}
		if (mapPixelSize.y > ws.y) {
			originalRange.from.y = mapPixelSize.y * 0.5f - (mapPixelSize.y - ws.y) * 0.5f;
			originalRange.to.y = mapPixelSize.y * 0.5f + (mapPixelSize.y - ws.y) * 0.5f;
		}
		else {
			originalRange.from.y = originalRange.to.y = mapPixelSize.y * 0.5f;
		}
		// 用值范围限制 original 值
		auto original = player->pos;
		if (original.x < originalRange.from.x) original.x = originalRange.from.x;
		else if (original.x > originalRange.to.x) original.x = originalRange.to.x;
		if (original.y < originalRange.from.y) original.y = originalRange.from.y;
		else if (original.y > originalRange.to.y) original.y = originalRange.to.y;
		cam.SetOriginal(original);

		// 设置内容绘制时不插值, 让图像清晰锐利
		gg.picsTex->SetParm(GL_NEAREST);

		// 准备地板污染痕迹贴图
		if (floorMasks.len) {
			// 将数据里的东西画到 render texture 上并清空
			frameBuffer.DrawTo(floorMaskTex, {}, [this] {
				// 这里的绘制坐标不受 cam 影响, 直接映射到逻辑地图. 以贴图左上角为 0,0 开始绘制
				auto leftTopPos = mapPixelSize * XY{ -0.5f, 0.5f };
				for (auto& o : floorMasks) {
					auto p = leftTopPos + o.pos.FlipY();
					gg.Quad().DrawFrame(o.frame, p, o.scale, o.radians, o.colorplus, o.color);
				}
				});
			floorMasks.Clear();
		}

		// 准备内容贴图( 需要被 light 照亮的部分 )
		auto tex = frameBuffer.Draw(gg.windowSize, true, xx::RGBA8{ 0,0,0,0 }, [&]() {
			// 绘制地板纹理
			for (int32_t i = 0; i < gridBuildings.numRows; ++i) {
				for (int32_t j = 0; j < gridBuildings.numCols; ++j) {
					XY p{ j * cCellPixelSize, i * cCellPixelSize };
					gg.Quad().DrawTinyFrame(gg.pics.c128_floor, cam.ToGLPos(p), { 0,1 }, cam.scale, 0);
				}
			}

			// 背景部分绘制
			for (auto& o : walls) o->Draw();
			for (auto& o : doors) o->Draw();

			// 地板污染痕迹绘制
			// todo: 换 quad ex 并按屏幕实际尺寸裁切uv 以节省填充率?
			gg.Quad().Draw(*floorMaskTex, *floorMaskTex, cam.ToGLPos(mapPixelSize * 0.5f), 0.5f, cam.scale, 0, 1.f, {222,222,222,222});

			// sort order by y
			SortContainerAdd(player.pointer);
			for (auto& o : buckets) SortContainerAdd(o.pointer);
			for (auto& o : playerBullets) SortContainerAdd(o.pointer);
			SortContainerDraw();

			// 爆炸特效覆盖在最上层
			for (auto& o : exploders) o->Draw();

			// todo: 影子
		});

		// 设置内容绘制时插值, 让光影过渡柔和
		gg.picsTex->SetParm(GL_LINEAR);

		// 准备光照贴图
		static constexpr float lightTexScale{ 0.25f };	// 用更小的绘制比例以节省填充率( 太小会画质恶劣 )
		cam.SetBaseScale(gg.scale * lightTexScale);
		auto bgColor = xx::RGBA8{ 10,10,10,255 };
		auto lightTex = frameBuffer.Draw(gg.windowSize * lightTexScale, true, bgColor, [&] {
			gg.GLBlendFunc({ GL_SRC_COLOR, GL_ONE, GL_FUNC_ADD });
			player->DrawLight();
			for (auto& o : playerBullets) o->DrawLight();
			for (auto& o : exploders) o->DrawLight();
			// ...
		});
		lightTex->SetParm(GL_LINEAR);
		cam.SetBaseScale(gg.scale);

		// 合并绘制: 内容 + 光照
		gg.QuadLight().Draw(tex, lightTex, xx::RGBA8_White, 2);
		// 立即提交以防止 tex, lightTex 出函数后失效
		gg.ShaderEnd();

		// 血条
		for (auto& o : buckets) o->DrawHPBar();

		// 伤害文字
		effectTexts.Draw();

		// 设置内容绘制时不插值, 让图像清晰锐利
		gg.picsTex->SetParm(GL_NEAREST);

		// 设置顶部信息文字显示内容
		gg.uiText->SetText(xx::ToString("buckets.len = ", buckets.len, "  playerBullets.len = ", playerBullets.len, "  exploders.len = ", exploders.len));
		gg.DrawNode(ui);
	}

	void Scene::OnResize(bool modeChanged_) {
		ui->Resize(gg.scale * cUIScale);
		cam.SetBaseScale(gg.scale);
	}

}

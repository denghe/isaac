#pragma once
#include "pch.h"
#include <xx_frame.h>

struct _pics {
	xx::Frame c32;
	xx::Frame c32_bullet;
	xx::Frame c64;
	xx::Frame c64_bullet;
	xx::Frame c128;
	xx::Frame c128_monster;
	xx::Frame cell_bucket;
	xx::Frame cell_door;
	xx::Frame cell_floor;
	xx::Frame cell_player;
	xx::Frame cell_wall;
	xx::Frame cursor_default;

	xx::Shared<xx::GLTexture> Load(std::string picFN_, bool generateMipmap_ = true);
};

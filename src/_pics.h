#pragma once
#include "pch.h"
#include <xx_frame.h>

struct _pics {
	xx::Frame cell_bucket;
	xx::Frame cell_door;
	xx::Frame cell_floor;
	xx::Frame cell_player;
	xx::Frame cell_wall;
	xx::Frame cursor_default;

	xx::Shared<xx::GLTexture> Load(std::string picFN_, bool generateMipmap_ = true);
};

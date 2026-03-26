#include "pch.h"
#include <xx_gamebase.h>
#include "_pics.h"

xx::Shared<xx::GLTexture> _pics::Load(std::string picFN_, bool generateMipmap_) {
	auto t = xx::GameBase::instance->LoadTexture(picFN_);
	if(generateMipmap_) t->TryGenerateMipmap();

	this->cell_bucket = { t, 9, 9, 128, 128, { 0.5, 0.5 } };
	this->cell_door = { t, 9, 147, 128, 128, { 0.5, 0.5 } };
	this->cell_floor = { t, 9, 285, 128, 128, { 0.5, 0.5 } };
	this->cell_player = { t, 147, 9, 128, 128, { 0.5, 0.5 } };
	this->cell_wall = { t, 285, 9, 128, 128, { 0.5, 0.5 } };
	this->cursor_default = { t, 9, 423, 12, 19, { 0, 1 } };
	return t;
};

#include "pch.h"
#include <xx_gamebase.h>
#include "_pics.h"

xx::Shared<xx::GLTexture> _pics::Load(std::string picFN_, bool generateMipmap_) {
	auto t = xx::GameBase::instance->LoadTexture(picFN_);
	if(generateMipmap_) t->TryGenerateMipmap();

	this->c32 = { t, 433, 9, 32, 32, { 0.5, 0.5 } };
	this->c32_bullet = { t, 433, 51, 32, 32, { 0.5, 0.5 } };
	this->c64 = { t, 9, 423, 64, 64, { 0.5, 0.5 } };
	this->c64_bullet = { t, 83, 423, 64, 64, { 0.5, 0.5 } };
	this->c128 = { t, 9, 9, 128, 128, { 0.5, 0.5 } };
	this->c128_monster = { t, 9, 147, 128, 128, { 0.5, 0.5 } };
	this->cell_bucket = { t, 9, 285, 128, 128, { 0.5, 0.5 } };
	this->cell_door = { t, 157, 9, 128, 128, { 0.5, 0.5 } };
	this->cell_floor = { t, 295, 9, 128, 128, { 0.5, 0.5 } };
	this->cell_player = { t, 147, 147, 128, 128, { 0.5, 0.5 } };
	this->cell_wall = { t, 147, 285, 128, 128, { 0.5, 0.5 } };
	this->cursor_default = { t, 475, 9, 12, 19, { 0, 1 } };
	return t;
};

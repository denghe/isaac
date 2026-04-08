#include "pch.h"
#include <xx_gamebase.h>
#include "_pics.h"

xx::Shared<xx::GLTexture> _pics::Load(std::string picFN_, bool generateMipmap_) {
	auto t = xx::GameBase::instance->LoadTexture(picFN_);
	if(generateMipmap_) t->TryGenerateMipmap();

	this->c32 = { t, 433, 83, 32, 32, { 0.5, 0.5 } };
	this->c32_bullet = { t, 433, 125, 32, 32, { 0.5, 0.5 } };
	this->c64 = { t, 9, 423, 64, 64, { 0.5, 0.5 } };
	this->c64_bullet = { t, 83, 423, 64, 64, { 0.5, 0.5 } };
	this->c64_light = { t, 433, 9, 64, 64, { 0.5, 0.5 } };
	this->c128 = { t, 9, 9, 128, 128, { 0.5, 0.5 } };
	this->c128_bucket = { t, 9, 147, 128, 128, { 0.5, 0.5 } };
	this->c128_door = { t, 9, 285, 128, 128, { 0.5, 0.5 } };
	this->c128_floor = { t, 157, 9, 128, 128, { 0.5, 0.5 } };
	this->c128_monster = { t, 295, 9, 128, 128, { 0.5, 0.5 } };
	this->c128_player = { t, 147, 167, 128, 128, { 0.5, 0.5 } };
	this->c128_wall = { t, 285, 147, 128, 128, { 0.5, 0.5 } };
	this->cursor_cross = { t, 475, 83, 19, 19, { 0.5, 0.5 } };
	this->cursor_default = { t, 475, 112, 12, 19, { 0, 1 } };
	return t;
};

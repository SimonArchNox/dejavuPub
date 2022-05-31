#pragma once
#ifndef __TIME_GRADIENT_H__
#define __TIME_GRADIENT_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <unordered_map>
using std::unordered_map;
using std::make_pair;

#include <ctime>

class TimeGradient : public LayerGradient {

private:
	const Color4B START_COLOR = Color4B(7, 95, 170, 255);
	const Color4B DEFAULT_END_COLOR = Color4B(135, 199, 250, 255);
	const Vec2 GRADIENT_ANGLE = Vec2(1.0f, -0.7f);

	TimeGradient();
	void setGradientColor();

public:
	static TimeGradient* create();
	~TimeGradient();

};

#endif //__TIME_GRADIENT_H__
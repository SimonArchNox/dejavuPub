#include "TimeGradient.h"

TimeGradient::TimeGradient()
{
	setGradientColor();
}

TimeGradient::~TimeGradient()
{
}

TimeGradient* TimeGradient::create() {

	TimeGradient* gradient = new TimeGradient();
	if (gradient) {
		gradient->autorelease();
		return gradient;
	}
	CC_SAFE_DELETE(gradient);
	return NULL;
}

void TimeGradient::setGradientColor() {

	time_t t = std::time(nullptr);
	std::tm* local = std::localtime(&t);
	int colorOffset = ((local->tm_hour % 4) * 33); //0-134
	Color4B endColor;
	Color4B startColor = START_COLOR;

	if (local->tm_hour >= 0 && local->tm_hour < 4) {
		//startColor = Color4B(4, 4, 138, 255);
		endColor = Color4B(138 - colorOffset, 4, 138, 255);
	}
	else if (local->tm_hour >= 4 && local->tm_hour < 8) {
		//startColor = Color4B(4, 138, 138, 255);
		endColor = Color4B(4, 4 + colorOffset, 138, 255);
	}
	else if (local->tm_hour >= 8 && local->tm_hour < 12) {
		//startColor = Color4B(4, 138, 4, 255);
		endColor = Color4B(4, 138, 138 - colorOffset, 255);
	}
	else if (local->tm_hour >= 12 && local->tm_hour < 16) {
		//startColor = Color4B(138, 138, 4, 255);
		endColor = Color4B(4 + colorOffset, 138, 4, 255);
	}
	else if (local->tm_hour >= 16 && local->tm_hour < 20) {
		//startColor = Color4B(138, 4, 4, 255);
		endColor = Color4B(138, 138 - colorOffset, 4, 255);
	}
	else if (local->tm_hour >= 20 && local->tm_hour < 24) {
		//startColor = Color4B(138, 4, 138, 255);
		endColor = Color4B(138, 4, 4 + colorOffset, 255);
	}
	else {
		startColor = START_COLOR;
		endColor = DEFAULT_END_COLOR;
	}

	this->initWithColor(startColor, endColor, GRADIENT_ANGLE); //(top, bottom)
}
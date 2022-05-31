#pragma once
#ifndef __BG_ICON_H__
#define __BG_ICON_H__

#include "cocos2d.h"
using namespace cocos2d;

class BGIcon : public Sprite {

private:
	enum e_iconActionState {START, RUN, PAUSE};

	BGIcon(const Point& startPoint, const Point& endPoint, const float& movementLength);
	Point _startPoint;
	Point _endPoint;
	float _movementLength;
	Sequence* _movementSeq;
	void movementSeqEnd();
	e_iconActionState _iconActionState;

public:
	static BGIcon* create(const std::string& spriteFrame = "blank.png", 
									const Point& startPoint = Point(0.0f,0.0f), const Point& endPoint = Point(0.0f, 0.0f),
									const float& movementLength = 0.0f);
	~BGIcon();
	void createMovementSequence(const float& length);
	void createMovementSequence(const float& length, const float& endpointX, const float& endpointY);

	void startAction();
	void stopAction();
};

#endif //__BACKGROUND_ICON_H__
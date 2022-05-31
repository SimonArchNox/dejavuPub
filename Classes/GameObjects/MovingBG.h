#pragma once
#ifndef __MOVING_BG_H__
#define __MOVING_BG_H__

#include "cocos2d.h"
using namespace cocos2d;

#include <vector>
using std::vector;

#include "BGIcon.h"

class MovingBG : public cocos2d::Node {

private:

	const unsigned int COLS = 5;
	const unsigned int ROWS = 6;

	Vector<BGIcon*> _bgSprites;
	vector<Point> _iconsPositions;
	vector<Point> _BGIconsEndPosition;
	vector<Point> _BGIconsStartPosition;
	unsigned int _totalBgSprites;

	const float C_BGI_ROTATE = 345.0f;
	const float C_BGI_SCALE = 1.5f;
	const float C_BGI_MOVEMENT_LENGTH = 40.0f; //full cycle, refers to row 1

	MovingBG();
	void initPositionMaps();
	void initBackgroundSprites();

public:
	static MovingBG* create();
	void setBackgroundSprites(const vector<std::string>& spriteList);
	void setBackgroundSprites(const vector<std::string>* spriteList);
	~MovingBG();
	void startAction();
	void stopAction();
};

#endif //__MOVING_BG__H__
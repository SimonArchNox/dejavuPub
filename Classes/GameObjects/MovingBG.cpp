#include "MovingBG.h"

MovingBG::MovingBG()
{
	initPositionMaps();	
}

MovingBG::~MovingBG()
{
	_bgSprites.clear();
}

//defaults: spriteFrame = "blank.png", startPoint = Point(0.0f,0.0f), endPoint = Point(0.0f, 0.0f)
MovingBG* MovingBG::create() {

	MovingBG * node = new MovingBG();
	if (node) {
		node->autorelease();
		node->setContentSize(Director::getInstance()->getWinSize());
		node->initBackgroundSprites();	//here becuase we first need to set content size
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

void MovingBG::initPositionMaps() {
	//background icons potsitions - left column to right column, lower row to higher row
	_iconsPositions = {
		Point(0.2126f, -0.2867f), Point(0.4925f, -0.2305f), Point(0.7725f, -0.1741f), Point(1.0525f, -0.1180f), Point(1.3327f, -0.0616f),
		Point(0.1345f, -0.0687f), Point(0.4145f, -0.0125f), Point(0.6945f, 0.0438f), Point(0.9746f, 0.1001f), Point(1.2547f, 0.1563f),
		Point(0.0568f, 0.1494f), Point(0.3368f, 0.2057f), Point(0.6168f, 0.2619f), Point(0.8969f, 0.3183f), Point(1.1771f, 0.3745f),
		Point(-0.0211f, 0.3674f), Point(0.2589f, 0.4237f), Point(0.5389f, 0.4800f), Point(0.8190f, 0.5363f), Point(1.0991f, 0.5926f),
		Point(-0.0991f, 0.5854f), Point(0.1810f, 0.6417f), Point(0.4609f, 0.6979f), Point(0.7409f, 0.7543f), Point(1.0212f, 0.8106f),
		Point(-0.1768f, 0.8035f), Point(0.1031f, 0.8598f), Point(0.3832f, 0.9161f), Point(0.6632f, 0.9724f), Point(0.9434f, 1.0287f)
	};

	_totalBgSprites = _iconsPositions.size();

	//left column to right column
	_BGIconsStartPosition = {
		Point(0.2126f, -0.2867f), Point(0.4926f, -0.2305f), Point(0.7725f, -0.1741f), Point(1.0525f, -0.1180f), Point(1.3327f, -0.0616f)
	};

	//left column to right column
	_BGIconsEndPosition = {
		Point(-0.2549f, 1.0216f), Point(0.0251f, 1.0778f), Point(0.3051f, 1.1341f), Point(0.5852f, 1.1903f), Point(0.8654f, 1.2467f)
	};

}

void MovingBG::initBackgroundSprites() {

	
	//crete movement length vector
	vector<float> movementLengthByRows;
	float length = C_BGI_MOVEMENT_LENGTH / ROWS;
	for (unsigned int i = 1; i <= ROWS; i++)
		movementLengthByRows.insert(movementLengthByRows.begin(), length * i);
	
	
	//create background sprite
	for (unsigned int i = 0; i < _totalBgSprites; i++) {
		int currColumn = i % COLS;
		auto bgIcon = BGIcon::create("blank.png",
			Point(this->getContentSize().width * _BGIconsStartPosition[currColumn].x, this->getContentSize().height * _BGIconsStartPosition[currColumn].y),
			Point(this->getContentSize().width * _BGIconsEndPosition[currColumn].x, this->getContentSize().height * _BGIconsEndPosition[currColumn].y),
			C_BGI_MOVEMENT_LENGTH);
		bgIcon->setPosition(this->getContentSize().width * _iconsPositions[i].x, this->getContentSize().height * _iconsPositions[i].y);
		bgIcon->setRotation(C_BGI_ROTATE);
		bgIcon->setScale(C_BGI_SCALE);
		//bgIcon->setVisible(true);
		_bgSprites.pushBack(bgIcon);
		this->addChild(bgIcon);

		//set length and and activate
		int currRow = i / COLS;
		bgIcon->createMovementSequence(movementLengthByRows[currRow]);
	}
}


void MovingBG::setBackgroundSprites(const vector<std::string>* spriteList) {
	setBackgroundSprites((*spriteList));
}


void MovingBG::setBackgroundSprites(const vector<std::string>& spriteList) {

	int spriteIndex = 0;
	for (int i = 0; i < _bgSprites.size(); i++) {

		spriteIndex = i % spriteList.size();
		auto icon = _bgSprites.at(i);
		icon->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteList[spriteIndex]));
	}
}

void MovingBG::startAction() {
	for (auto icon : _bgSprites)
		icon->startAction();
}

void MovingBG::stopAction() {
	for (auto icon : _bgSprites)
		icon->stopAction();
}

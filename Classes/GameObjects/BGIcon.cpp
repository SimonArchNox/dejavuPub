#include "BGIcon.h"

BGIcon::BGIcon(const Point& startPoint, const Point& endPoint, const float& movementLength)
	:_startPoint(startPoint), _endPoint(endPoint), _movementLength(movementLength), _iconActionState(e_iconActionState::START)
{}

BGIcon::~BGIcon() 
{}

//defaults: spriteFrame = "blank.png", startPoint = Point(0.0f,0.0f), endPoint = Point(0.0f, 0.0f)
BGIcon* BGIcon::create(const std::string& spriteFrame, const Point& startPoint, const Point& endPoint, const float& movementLength) {

	BGIcon * sprite = new BGIcon(startPoint, endPoint, movementLength);
	if (sprite && sprite->initWithSpriteFrameName(spriteFrame)) {
		sprite->autorelease();
		return sprite;
	}
	CC_SAFE_DELETE(sprite);
	return NULL;
}

void BGIcon::movementSeqEnd() {
	this->setPosition(_startPoint.x, _startPoint.y);
	createMovementSequence(_movementLength, _endPoint.x, _endPoint.y);
}

void BGIcon::createMovementSequence(const float& length) {

	this->createMovementSequence(length, _endPoint.x, _endPoint.y);
}

void BGIcon::createMovementSequence(const float& length, const float& endpointX, const float& endpointY) {

	_movementSeq = Sequence::create(
		MoveTo::create(length, Vec2(endpointX, endpointY)),
		CallFunc::create(std::bind(&BGIcon::movementSeqEnd, this)), nullptr);
	
	this->runAction(_movementSeq);
}


void BGIcon::startAction() {
	if (_iconActionState == e_iconActionState::START) {
		this->runAction(_movementSeq);
		_iconActionState = e_iconActionState::RUN;
	}
	else if (_iconActionState == e_iconActionState::PAUSE) {
		this->resume();
		_iconActionState = e_iconActionState::RUN;
	}
	
}
void BGIcon::stopAction() {
	if (_iconActionState != e_iconActionState::PAUSE) {
		this->pause();
		_iconActionState = e_iconActionState::PAUSE;
	}
}

#pragma once
#ifndef __PACKS_SELECTION_LAYER_H__
#define __PACKS_SELECTION_LAYER_H__

#include "cocos2d.h"
using namespace cocos2d;

class PacksSelectionLayer : public Layer {

private:


public:
	CREATE_FUNC(PacksSelectionLayer);

	PacksSelectionLayer();
	virtual ~PacksSelectionLayer();

	//touch events
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);

	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

};

#endif //__PACKS_SELECTION_LAYER_H__
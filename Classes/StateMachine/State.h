#pragma once
#ifndef __STATE_H__
#define __STATE_H__

#include "StatesMisc.h"
class DejavuGame;

class State {
protected:

public: 
	virtual void onEnter(DejavuGame* context) = 0;
	virtual void onExit(DejavuGame* context) = 0;

	virtual void update(DejavuGame* context, const float& dt) = 0;

	virtual bool onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown) = 0;
	virtual void onTouchMoved(DejavuGame* context, StatesMisc::Touch touch) = 0;
	virtual void onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection) = 0;

	virtual void onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent) = 0;
	virtual void onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender) = 0;

};

#endif //__STATE_H__
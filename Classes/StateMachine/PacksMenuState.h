#pragma once
#ifndef __PACKS_MENU_STATE_H__
#define __PACKS_MENU_STATE_H__

#include "State.h"
#include "DejavuGame.h"

class PacksMenuState : public State {

private:
	enum class PacksMenuInnerStates {
		PacksMenu
	};

	PacksMenuInnerStates _innerState;

public: 
	PacksMenuState();
	virtual ~PacksMenuState();
	
	virtual void onEnter(DejavuGame* context);
	virtual void onExit(DejavuGame* context);

	virtual void update(DejavuGame* context, const float& dt);

	virtual bool onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown);
	virtual void onTouchMoved(DejavuGame* context, StatesMisc::Touch touch);
	virtual void onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection);

	virtual void onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent);
	virtual void onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender);

	//virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	//virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

};

#endif //__PACKS_MENU_STATE_H__
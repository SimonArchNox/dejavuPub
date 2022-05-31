#pragma once
#ifndef __GAME_PLAY_STATE_H__
#define __GAME_PLAY_STATE_H__

#include "State.h"
#include "DejavuGame.h"

class GamePlayState : public State {

private:
	enum class GamePlayInnerStates {
		GamePlayNewRound,
		GamePlayMakeCard,
		GamePlayRevealCard,
		GamePlayTimerOn,
		GamePlayTimerExpires,

		GamePlayMakeLevelUpCard,
		GamePlayRevealLevelUpCard,
		GamePlayLevelUpCardTimerOn,

		GamePlayPlayerSwipe,
		GamePlayLevelUpCardSwipe,

		GamePlayZeroLives,	//game over (0 lives)
		GamePlayModeEnd	//this is when the "deck" of cards is exhausted
	};

	GamePlayInnerStates _innerState;

public: 
	GamePlayState();
	virtual ~GamePlayState();
	
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

#endif //__GAME_PLAY_STATE_H__
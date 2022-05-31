#include "InitGamePlayState.h"

InitGamePlayState::InitGamePlayState() {
	_innerState = InitGamePlayInnerStates::GamePlayInit;
}

InitGamePlayState::~InitGamePlayState() {

}

void InitGamePlayState::onEnter(DejavuGame* context) {
	context->initGame();
	context->showGameplayHUD();
}

void InitGamePlayState::onExit(DejavuGame* context) {

}

void InitGamePlayState::update(DejavuGame* context, const float& dt) {
	context->moveToGameplayState();
}

bool InitGamePlayState::onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown)
{
	return true;
}

void InitGamePlayState::onTouchMoved(DejavuGame* context, StatesMisc::Touch touch)
{

}

void InitGamePlayState::onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection)
{

}

void InitGamePlayState::onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent)
{
	/*
	switch (callbackEvent) {

	default:
		break;
	}
	*/
}

void InitGamePlayState::onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender) {

}
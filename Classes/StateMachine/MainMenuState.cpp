#include "MainMenuState.h"

MainMenuState::MainMenuState() {
	_innerState = MainMenuInnerStates::GameMainMenu;
}

MainMenuState::~MainMenuState() {

}

void MainMenuState::onEnter(DejavuGame* context) {
	//show GUI elements
	context->showMainMenuHUD();
	
}

void MainMenuState::onExit(DejavuGame* context) {

}

void MainMenuState::update(DejavuGame* context, const float& dt) {

}

bool MainMenuState::onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown)
{
	return true;
}

void MainMenuState::onTouchMoved(DejavuGame* context, StatesMisc::Touch touch)
{

}

void MainMenuState::onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection)
{

}

void MainMenuState::onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent)
{
	/*
	switch (callbackEvent) {

	default:
		break;
	}
	*/
}

void MainMenuState::onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender) {

	switch (sender) {
	case StatesMisc::ButtonTouchEventEndedSender::StartButton:
		context->moveToInitGameState();
		break;
	default:
	break;
	}

}


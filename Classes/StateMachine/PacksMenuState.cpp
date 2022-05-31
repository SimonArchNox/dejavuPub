#include "PacksMenuState.h"

PacksMenuState::PacksMenuState() {
	_innerState = PacksMenuInnerStates::PacksMenu;
}

PacksMenuState::~PacksMenuState() {

}

void PacksMenuState::onEnter(DejavuGame* context) {
	context->showPacksMenu();
}

void PacksMenuState::onExit(DejavuGame* context) {
	context->hidePacksMenu();
}

void PacksMenuState::update(DejavuGame* context, const float& dt) {

}

bool PacksMenuState::onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown)
{
	return true;
}

void PacksMenuState::onTouchMoved(DejavuGame* context, StatesMisc::Touch touch)
{

}

void PacksMenuState::onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection)
{

}

void PacksMenuState::onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent)
{
	/*
	switch (callbackEvent) {

	default:
		break;
	}
	*/
}

void PacksMenuState::onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender) {


}


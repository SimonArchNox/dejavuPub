#include "GameOverState.h"

GameOverState::GameOverState() {
	_innerState = GameOverInnerStates::GameOverRevealGameOverCard;
}

GameOverState::~GameOverState() {

}

void GameOverState::onEnter(DejavuGame* context) {
	_innerState = GameOverInnerStates::GameOverRevealGameOverCard;
	context->showGameOverCard();
	context->revealNextCard();
	
}

void GameOverState::onExit(DejavuGame* context) {
	
}

void GameOverState::update(DejavuGame* context, const float& dt) {

	switch (_innerState) {
	case GameOverInnerStates::GameOver:
		_innerState = GameOverInnerStates::GameOverMenu;
		context->showMainMenuHUD();
		context->showCoinsPanel();
		break;
	default:
		break;
	}

}

bool GameOverState::onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown)
{
	/*
	switch (_innerState) {

	default:
		break;
	}
	*/
	return true;
}

void GameOverState::onTouchMoved(DejavuGame* context, StatesMisc::Touch touch)
{

}

void GameOverState::onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection)
{
	/*
	switch (_innerState) {

	default:
		break;
	}
	*/

}

void GameOverState::onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent)
{	
	switch (callbackEvent) {
	case StatesMisc::CallbackEvents::flipCurrentSeqEnds:

		if (_innerState == GameOverInnerStates::GameOverRevealGameOverCard) {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			if (context->disableAds) {
				_innerState = GameOverInnerStates::GameOver;
			}
			else {
				//show ad here
				_innerState = GameOverInnerStates::GameOverShowAd;
				if (!context->checkAndShowPlacement("placement-admob"))
					_innerState = GameOverInnerStates::GameOver;
			}
#else
			_innerState = GameOverInnerStates::GameOver;
#endif
		}

		break;
	case StatesMisc::CallbackEvents::currentCardSwapSeqEnded:
		context->moveToInitGameState();
		break;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	case StatesMisc::CallbackEvents::onGmeOverAdEnded:
		if (_innerState == GameOverInnerStates::GameOverShowAd)
			_innerState = GameOverInnerStates::GameOver;

		break;
#endif
	default:
		break;
	}
	
}

void GameOverState::onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender) {

	switch (sender) {
	case StatesMisc::ButtonTouchEventEndedSender::StartButton:
	case StatesMisc::ButtonTouchEventEndedSender::PacksButton:
		context->playSwipeAnimation(StatesMisc::SwipeDirection::RIGHT);
	default:
	break;
	}

}

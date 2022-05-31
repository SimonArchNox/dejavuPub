#include "GamePlayState.h"

GamePlayState::GamePlayState() {
	_innerState = GamePlayInnerStates::GamePlayNewRound;
}

GamePlayState::~GamePlayState() {

}

void GamePlayState::onEnter(DejavuGame* context) {
	_innerState = GamePlayInnerStates::GamePlayNewRound;
}

void GamePlayState::onExit(DejavuGame* context) {
	context->hideGameplayHUD();
}

void GamePlayState::update(DejavuGame* context, const float& dt) {

	switch (_innerState) {

	case GamePlayInnerStates::GamePlayNewRound:
		if (context->_lives == 0) { //check for game over
			_innerState = GamePlayInnerStates::GamePlayZeroLives;
		}
		else {
			//growth rate here - level up
			if (context->_cardsRevealed > 0 && 
				context->_cardsRevealed % context->_pCurrentDifficultyData->growthRate == 0)
			{

				//introduce new suit - level up
				if (context->_highSuitWindow < context->_pCurrentDifficultyData->concurrentSuits)
					context->_highSuitWindow++;
				else
					context->_lowSuitWindow++;
				
				//check for game ending - when the deck is exhausted
				if (context->_cardsRevealed >= context->_numSuitsSingleGame * context->_pCurrentDifficultyData->maxCardRankBound)
					_innerState = GamePlayInnerStates::GamePlayModeEnd;	
				else
					_innerState = GamePlayInnerStates::GamePlayMakeLevelUpCard;
			}
			else
				_innerState = GamePlayInnerStates::GamePlayMakeCard;
		}
		break;

	case GamePlayInnerStates::GamePlayMakeLevelUpCard:
		_innerState = GamePlayInnerStates::GamePlayRevealLevelUpCard;
		context->clearAllCurrentCardFrontSprites();
		context->makeLevelUpCard();
		context->revealNextCard();
		break;

	case GamePlayInnerStates::GamePlayMakeCard:
		_innerState = GamePlayInnerStates::GamePlayRevealCard;
		context->clearAllCurrentCardFrontSprites();
		context->makeChallengeCard();
		context->_cardsRevealed++;
		context->revealNextCard();
		break;

	case GamePlayInnerStates::GamePlayTimerOn:
		//advance timer
		context->advanceCardTimer(dt);

		if (context->_timer > context->_timerPerCard) {
			_innerState = GamePlayInnerStates::GamePlayTimerExpires;
			context->decreasePlayerLife();
			//action to drop the card down
			context->_currentFrontCardNode->runAction(context->_currentCardTilt);
		}
		break;

	case GamePlayInnerStates::GamePlayLevelUpCardTimerOn:
		//advance timer
		context->advanceCardTimer(dt);

		if (context->_timer > context->_timerPerCard) {
			//action to drop the card down
			_innerState = GamePlayInnerStates::GamePlayTimerExpires;
			context->_currentFrontCardNode->runAction(context->_currentCardSwapLeft);
			//RAB
		}
		break;

	case GamePlayInnerStates::GamePlayZeroLives:
	case GamePlayInnerStates::GamePlayModeEnd:
		context->clearAllCurrentCardFrontSprites();
		context->moveToGameOverState();
		break;

	default:
		break;
	}


}

bool GamePlayState::onTouchBegan(DejavuGame* context, StatesMisc::Touch touchDown)
{
	switch (_innerState) {
	case GamePlayInnerStates::GamePlayTimerOn:
		context->_touchDown = Point(touchDown.x, touchDown.y);
		break;

	default:
		break;
	}
	
	return true;
}

void GamePlayState::onTouchMoved(DejavuGame* context, StatesMisc::Touch touch)
{

}

void GamePlayState::onTouchEnded(DejavuGame* context, StatesMisc::Touch touchUp, StatesMisc::SwipeDirection swipeDirection)
{
	bool isCorrect = false;
	switch (_innerState) {
	case GamePlayInnerStates::GamePlayTimerOn:
		
		if (swipeDirection == StatesMisc::SwipeDirection::NONE) return;

		_innerState = GamePlayInnerStates::GamePlayPlayerSwipe;
		context->playSwipeAnimation(swipeDirection);
		isCorrect = context->isPlayerCorrect(swipeDirection);
		if (isCorrect) {
			context->_score += context->calcPointsValue();
			context->_playerScoreLbl->setString(std::to_string(context->_score));
		}
		else {
			context->_particlesSadFace->resetSystem();
			context->_particlesCoinGain->resetSystem();
			context->decreasePlayerLife();
		}
		break;

	case GamePlayInnerStates::GamePlayLevelUpCardTimerOn:
		if (swipeDirection == StatesMisc::SwipeDirection::NONE) return;

		_innerState = GamePlayInnerStates::GamePlayLevelUpCardSwipe;
		context->playSwipeAnimation(swipeDirection);
		break;

	default:
		break;
	}
}

void GamePlayState::onCallbackEvent(DejavuGame* context, StatesMisc::CallbackEvents callbackEvent)
{	
	switch (callbackEvent) {

	case StatesMisc::CallbackEvents::flipCurrentSeqEnds:

		if (_innerState == GamePlayInnerStates::GamePlayRevealLevelUpCard)
			_innerState = GamePlayInnerStates::GamePlayLevelUpCardTimerOn;
		else //default
			_innerState = GamePlayInnerStates::GamePlayTimerOn;
		break;


	case StatesMisc::CallbackEvents::currentCardTiltEnded:
		if (!context->_isCardAlreadySeen)
			context->_currentFrontCardNode->runAction(context->_currentCardSwapLeft);
		else
			context->_currentFrontCardNode->runAction(context->_currentCardSwapRight);
		break;

	case StatesMisc::CallbackEvents::currentCardSwapSeqEnded:
		//if (_innerState == GamePlayInnerStates::GamePlayPlayerSwipe || _innerState == GamePlayInnerStates::GamePlayTimerExpires)
		
		if (_innerState == GamePlayInnerStates::GamePlayLevelUpCardSwipe || _innerState == GamePlayInnerStates::GamePlayTimerExpires)
			_innerState = GamePlayInnerStates::GamePlayMakeCard;
		else
			_innerState = GamePlayInnerStates::GamePlayNewRound;

		break;

	default:
		break;
	}
	
}

void GamePlayState::onButtonTouchEventEnded(DejavuGame* context, StatesMisc::ButtonTouchEventEndedSender sender) {

}

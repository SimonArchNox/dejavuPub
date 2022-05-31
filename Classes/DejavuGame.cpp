#include "DejavuGame.h"
#include "SimpleAudioEngine.h"
#include "StateMachine/StatesMisc.h"
#include "DataEntries.h"
#include <assert.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "../proj.android/app/jni/PluginSdkboxAds/PluginSdkboxAds.h"
#endif

USING_NS_CC;

#include <iostream>
using std::cout;
using std::endl;

const unsigned int MAX_CARD_RANK_NUM = 10;
const unsigned int DEFAULT_PACK_INDEX = 0;
const char AVAIL_PACKS_DELIMETER = ';';

DejavuGame::DejavuGame() 
{}

DejavuGame::~DejavuGame() {
	srand(time(NULL));	//clear random seed

	//flush user data
	_userDefault->flush();

	//vectors
	_currentCardRankIconsVec.clear();
	_playerLivesSprites.clear();

	//particles
	_particlesSadFace->stopSystem();
	_particlesCoinGain->stopSystem();

	//clear states
	_statesMap.clear();
}

Scene* DejavuGame::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// add layer as a child to scene
	auto layer = DejavuGame::create();

	scene->addChild(layer);
	
	return scene;
}

//------------------------------------------ INIT --------------------------------
bool DejavuGame::init() {

	if (!Layer::init())
	{
		return false;

	}

	//init data memebrs
	_screenSize = Director::getInstance()->getWinSize();
	std::srand(time(0)); //init random seed

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    addAdsListeners();
    cacheAds();
#endif
	
	//constructor like
	initDataMembers();

	//data
	loadGameConfig();
	loadGameData();
	loadUserCustomData();
	initAvailablePacks();
	initGamePack();
    initPositionMaps();
    resetPlayerStats();
    
	//visuals
	createGameScreen(); //create the screen
    createActions();
	createGUI();
	createParticles();
	createStates();

	this->hideGameplayHUD();
	this->hideMainMenuHUD();
	this->hidePacksMenu();

    //init listeners
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(DejavuGame::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(DejavuGame::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(DejavuGame::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    auto keyListener = EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(DejavuGame::onKeyPressed, this);
    keyListener->onKeyReleased = CC_CALLBACK_2(DejavuGame::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

    this->scheduleUpdate();

	this->moveToMainMenuState();

    return true;
}

void DejavuGame::initDataMembers() {
	_userDefault = UserDefault::getInstance();
	_touchDown = Point(0, 0);
    _swipeAngle = -1.0f;

	_currentPackName = "";
}

void DejavuGame::createStates() {
	_currentState = nullptr;
	
	State* mainMenuState = new MainMenuState();
	this->addState(C_MAIN_MENU_STATE, mainMenuState);

	State* initGameplayState = new InitGamePlayState();
	this->addState(C_INIT_GAMEPLAY_STATE, initGameplayState);

	State* gameplayState = new GamePlayState();
	this->addState(C_GAMEPLAY_STATE, gameplayState);

	State* gameOverState = new GameOverState();
	this->addState(C_GAMEOVER_STATE, gameOverState);

	State* packsMenuState = new PacksMenuState();
	this->addState(C_PACKS_MENU_STATE, packsMenuState);
}

void DejavuGame::createGameScreen(void) {

	//load sprites cache
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile("dejavu_sprite_sheet.plist");

	createBackground();

	//draw batch sprites
	_spriteBatchNode = SpriteBatchNode::create("dejavu_sprite_sheet.png", 100);
	this->addChild(_spriteBatchNode, kzForeground);

    _deckBack = Sprite::createWithSpriteFrameName("cardBack1.png");
    _deckBack->setPosition(_cardPosition);
    _spriteBatchNode->addChild(_deckBack, kzDeckBack);

    _currentCardBack = Sprite::createWithSpriteFrameName("cardBack1.png");
    _currentCardBack->setPosition(_cardPosition);
    _spriteBatchNode->addChild(_currentCardBack, kzCardBack);

	//start of _currentFrontCardNode
	_currentFrontCardNode = Node::create();
	_currentFrontCardNode->setPosition(_cardPosition);
	_currentFrontCardNode->setVisible(false);
	Size nodeSize = SpriteFrameCache::getInstance()->getSpriteFrameByName("cardFront.png")->getOriginalSizeInPixels();
	_currentFrontCardNode->setContentSize(nodeSize);
	this->addChild(_currentFrontCardNode, kzCardFront);
	
	_spriteBatchFrontCard = SpriteBatchNode::create("dejavu_sprite_sheet.png", 100);
	_currentFrontCardNode->addChild(_spriteBatchFrontCard, kzForeground);

    _currentCardFront = Sprite::createWithSpriteFrameName("cardFront.png");
	_currentCardFront->setPosition(Vec2(0.5f, 0.5f));
	_spriteBatchFrontCard->addChild(_currentCardFront, kzCardFront);

	//init card front icons vector
	for (unsigned int i = 0; i < MAX_CARD_RANK_NUM; i++) {
		auto sprite = Sprite::createWithSpriteFrameName("blank.png");
		sprite->setVisible(false);
		_currentCardFront->addChild(sprite, kzCardIcons, kSpriteCardIcons);
		_currentCardRankIconsVec.pushBack(sprite);
	}

    //level up card elements
    _levelUpText = Sprite::createWithSpriteFrameName("levelUpText.png");
    _levelUpText->setPosition(_currentCardFront->getContentSize().width * 0.5f, _currentCardFront->getContentSize().height * 0.7494f);
    _levelUpText->setVisible(false);
    _currentCardFront->addChild(_levelUpText, kSpriteLevelUpCard);

    _newSuiteText = Sprite::createWithSpriteFrameName("newSuiteText.png");
    _newSuiteText->setPosition(_currentCardFront->getContentSize().width * 0.5f, _currentCardFront->getContentSize().height * 0.1235f);
    _newSuiteText->setVisible(false);
    _currentCardFront->addChild(_newSuiteText, kSpriteLevelUpCard);

    _newSuiteIcon = Sprite::createWithSpriteFrameName("blank.png");
    _newSuiteIcon->setPosition(_currentCardFront->getContentSize().width * 0.5f, _currentCardFront->getContentSize().height * 0.3800f);
    _newSuiteIcon->setVisible(false);
    _currentCardFront->addChild(_newSuiteIcon, kSpriteLevelUpCard);

	//game over card elements
    _gameOverText = Sprite::createWithSpriteFrameName("gameOverText.png");
    _gameOverText->setPosition(_currentCardFront->getContentSize().width * 0.5f, _currentCardFront->getContentSize().height * 0.7861f);
    _gameOverText->setVisible(false);
    _currentCardFront->addChild(_gameOverText, kSpriteLevelUpCard);

	_yourScoreText = Sprite::createWithSpriteFrameName("yourScoreText.png");
	_yourScoreText->setPosition(_currentCardFront->getContentSize().width * 0.5f, _currentCardFront->getContentSize().height * 0.2769f);
	_yourScoreText->setVisible(false);
    _currentCardFront->addChild(_yourScoreText, kSpriteLevelUpCard);

	_scoreGameOverLbl = Label::createWithTTF(std::to_string(_score), "fonts/arial.ttf", 80.0f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
	_scoreGameOverLbl->setPosition(_currentFrontCardNode->getContentSize().width * 0.0f, _currentFrontCardNode->getContentSize().height * -0.3835f);
	_scoreGameOverLbl->setTextColor(Color4B::BLACK);
	_scoreGameOverLbl->setVisible(true);
	_currentFrontCardNode->addChild(_scoreGameOverLbl, kzGUI);
	//end of _currentFrontCardNode

}

//------------------------------------------ BACKGROUND -------------------------------
void DejavuGame::createBackground() {

	_bgGradient = TimeGradient::create();
	this->addChild(_bgGradient);
	
	_bgLayer = MovingBG::create();
	_bgLayer->setBackgroundSprites(_currentPackSprites);
	this->addChild(_bgLayer, kzBackground);
}

//------------------------------------------ PARTICLES --------------------------------
void DejavuGame::createParticles(void) {

    //particles systems
    _particlesSadFace = ParticleSystemQuad::create("particlesSadFace.plist");
    _particlesSadFace->setSourcePosition(_particlesSadFacePos);
    _particlesSadFace->stopSystem();
    this->addChild(_particlesSadFace, kzGUI);

	_particlesCoinGain = ParticleSystemQuad::create("particlesCoinGain.plist");
	_particlesCoinGain->setSourcePosition(_particlesSadFacePos);
	_particlesCoinGain->stopSystem();
	//init in CreateGUI function
	//_playerCoinsNode->addChild(_particlesCoinGain, kzGUI);
	this->addChild(_particlesCoinGain, kzGUI);

}

//------------------------------------------ ACTIONS --------------------------------
//NOTE: for actions calls to _currentState->onCallbackEvent should be after callback logic
void DejavuGame::createActions(void) {
	//actions
	_flipCard = Sequence::create(
		TargetedAction::create(_currentCardBack, ScaleTo::create(0.2f, 1.1f, 1.1f)),
		TargetedAction::create(_currentCardBack, ScaleTo::create(0.15f, 0.0f, 1.1f)),
		TargetedAction::create(_currentCardBack, CallFunc::create(std::bind(&DejavuGame::flipCardBackEnds, this))),
		TargetedAction::create(_currentFrontCardNode, CallFunc::create(std::bind(&DejavuGame::flipCurrentCardBegins, this))),
		TargetedAction::create(_currentFrontCardNode, ScaleTo::create(0.15f, 1.1f, 1.1f)),
		//TargetedAction::create(_currentCardFront, ScaleTo::create(3.0f, 1.0f, 1.0f)),
		TargetedAction::create(_currentFrontCardNode, CallFunc::create(std::bind(&DejavuGame::flipCurrentSeqEnds, this))),
		nullptr);
	_flipCard->retain();

	//card shakes animation sequence
	Size currentCardFrontSize = _currentCardFront->getBoundingBox().size;
	float tiltBy = (50.0f * 100.0f) / currentCardFrontSize.width;
	float dropTo = -((currentCardFrontSize.height / 2.0f) + 100);
	_currentCardTilt = Sequence::create(
		MoveBy::create(0.05f, Point(-tiltBy, 0.0f)),
		MoveBy::create(0.05f, Point(tiltBy * 2, 0.0f)),
		MoveBy::create(0.05f, Point(-tiltBy * 2, 0.0f)),
		MoveBy::create(0.05f, Point(tiltBy * 2, 0.0f)),
		MoveBy::create(0.05f, Point(-tiltBy * 2, 0.0f)),
		MoveBy::create(0.05f, Point(tiltBy * 2, 0.0f)),
		MoveBy::create(0.05f, Point(-tiltBy * 2, 0.0f)),
		MoveBy::create(0.05f, Point(tiltBy, 0.0f)),
		CallFunc::create(std::bind(&DejavuGame::currentCardTiltEnded, this)),
		nullptr);
	_currentCardTilt->retain();

	float leftOffscreen = _screenSize.width * -1.0f;
	_currentCardSwapLeft = Sequence::create(
		Spawn::create(RotateBy::create(0.3f, -45.0f),
			MoveBy::create(0.6f, Point(leftOffscreen, 0.0f)),
			nullptr),
		CallFunc::create(std::bind(&DejavuGame::currentCardSwapSeqEnded, this)),
		nullptr);
	_currentCardSwapLeft->retain();

	float rightOffscreen = _screenSize.width + (_screenSize.width * 0.5f);
	_currentCardSwapRight = Sequence::create(
		Spawn::create(RotateBy::create(0.3f, 45.0f),
			MoveBy::create(0.6f, Point(rightOffscreen, 0.0f)),
			nullptr),
		CallFunc::create(std::bind(&DejavuGame::currentCardSwapSeqEnded, this)),
		nullptr);
	_currentCardSwapRight->retain();

	_instructionsLayoutFadeInOut = Sequence::create(
		FadeIn::create(0.5f),
		DelayTime::create(2.0f),
		FadeOut::create(0.5f),
		CallFunc::create(std::bind(&DejavuGame::instructionsLayoutFadeInOutEnded, this)),
		nullptr);
	_instructionsLayoutFadeInOut->retain();

	/*
	_startButtonAppear = Sequence::create(
		Spawn::create(FadeIn::create(0.5f),
			MoveBy::create(0.5f, Point(leftOffscreen, 0.0f)),
			nullptr),
		CallFunc::create(std::bind(&DejavuGame::startButtonAppearSeqEnded, this)),
		nullptr);
	_currentCardSwapLeft->retain();
	*/

	_playerCoinsFadeInOut = Sequence::create(
		FadeIn::create(0.4f),
		DelayTime::create(1.0f),
		FadeOut::create(0.4f),
		CallFunc::create(std::bind(&DejavuGame::playerCoinsFadeInOutEnded, this)),
		nullptr);

	_playerCoinsFadeInOut->retain();
}

void DejavuGame::flipCardBackEnds(void) {

	this->_currentCardBack->setScaleX(1.0f);
	this->_currentCardBack->setScaleY(1.0f);

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::flipCardBackEnds);
}

void DejavuGame::flipCurrentCardBegins(void) {

	this->_currentFrontCardNode->setPosition(this->_cardPosition);
	this->_currentFrontCardNode->setScaleX(0.0f);
	this->_currentFrontCardNode->setScaleY(1.1f);
	this->_currentFrontCardNode->setVisible(true);

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::flipCurrentCardBegins);
}

void DejavuGame::flipCurrentSeqEnds(void) {

	this->resetCardTimer();

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::flipCurrentSeqEnds);
}

void DejavuGame::currentCardTiltEnded(void) {

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::currentCardTiltEnded);
}

void DejavuGame::currentCardSwapSeqEnded(void) {

	this->resetCurrentCardFrontSprite();

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::currentCardSwapSeqEnded);
}

void DejavuGame::resetCurrentCardFrontSprite(void) {
	_currentFrontCardNode->setVisible(false);
	_currentFrontCardNode->setRotation(0.0f);
}

void DejavuGame::instructionsLayoutFadeInOutEnded(void) {
	
	this->_instructionsLayout->setVisible(false);
	this->_helpButton->setEnabled(true);

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::instructionsLayoutFadeInOutEnded);
}


void DejavuGame::playerCoinsFadeInOutEnded(void) {

	_playerCoinsNode->setVisible(false);

	if (_currentState)
		_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::playerCoinsFadeInOutEnded);
}


//------------------------------------------ HUD/GUI --------------------------------
void DejavuGame::createGUI(void) {
    //hud elements
	//draw timer
	_cardTimer = ProgressTimer::create(Sprite::create("timerSprite.png"));
	_cardTimer->setPosition(_screenSize.width * 0.5f, _screenSize.height * 0.6879f);
	_cardTimer->setType(ProgressTimer::Type::RADIAL);
	_cardTimer->setPercentage(100.0f);
	this->addChild(_cardTimer, kzForeground);

	_timerIcon = Sprite::createWithSpriteFrameName("hourglassIcon.png");
	_timerIcon->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.6879f));
	_spriteBatchNode->addChild(_timerIcon, kzGUI);

	_playerScoreLbl = Label::createWithTTF(std::to_string(_score), "fonts/arial.ttf", 80.0f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
	_playerScoreLbl->setPosition(_screenSize.width * 0.4868f, _screenSize.height * 0.9027f);
	_playerScoreLbl->setTextColor(Color4B::BLACK);
	this->addChild(_playerScoreLbl, kzGUI);

    _playerScorePanel = Sprite::createWithSpriteFrameName("scorePanel.png");
    _playerScorePanel->setPosition(Vec2(_screenSize.width * 0.4868f, _screenSize.height * 0.9027f));
    _spriteBatchNode->addChild(_playerScorePanel, kzGUI);

	_playerLivesPanel = Sprite::createWithSpriteFrameName("playerLivesPanel.png");
	_playerLivesPanel->setPosition(Vec2(_screenSize.width * 0.8527f, _screenSize.height * 0.9038f));
	_spriteBatchNode->addChild(_playerLivesPanel, kzGUI);

    for (unsigned int i = 0; i < _lives; i++) {
        Sprite* playerLife = Sprite::createWithSpriteFrameName("playerLives_2.png");
        playerLife->setPosition(Vec2(_screenSize.width * _playerLivesPositions.at(i).x, _screenSize.height * _playerLivesPositions.at(i).y));
        playerLife->setVisible(false);
        _playerLivesSprites.pushBack(playerLife);
		this->addChild(playerLife, kzGUI);
    }

    _instructionsLayout = Sprite::createWithSpriteFrameName("instructionsLayout.png");
    _instructionsLayout->setPosition(Vec2(_screenSize.width * 0.5000f, _screenSize.height * 0.2792f));
	_instructionsLayout->setOpacity(0);
    _instructionsLayout->setVisible(false);
    _spriteBatchNode->addChild(_instructionsLayout, kzTop);

	_startButton = Button::create("playButton.png", "playButton.png", "playButton.png", ui::Widget::TextureResType::PLIST);
	_startButton->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.3797f));
	_startButton->addTouchEventListener(
		[&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			this->hideMainMenuHUD();
			
			if (_currentState)
				this->_currentState->onButtonTouchEventEnded(this, StatesMisc::ButtonTouchEventEndedSender::StartButton);
		}
	});
	this->addChild(_startButton, kzGUI);

    _helpButton = Button::create("helpButton.png", "helpButton.png", "helpButton.png", ui::Widget::TextureResType::PLIST);
    _helpButton->setPosition(Vec2(_screenSize.width * 0.1081f, _screenSize.height * 0.0813f));
    _helpButton->addTouchEventListener(
            [&](Ref* sender, ui::Widget::TouchEventType type) {
                if (type == ui::Widget::TouchEventType::ENDED)
                {
                    _helpButton->setEnabled(false);
                    _instructionsLayout->setVisible(true);
                    _instructionsLayout->runAction(_instructionsLayoutFadeInOut);
                }
            });
    this->addChild(_helpButton, kzGUI);

	_packsButton = Button::create("catalogButton.png", "catalogButton.png", "catalogButton.png", ui::Widget::TextureResType::PLIST);
	_packsButton->setPosition(Vec2(_screenSize.width * 0.1081f, _screenSize.height * 0.8007f));
	_packsButton->addTouchEventListener(
		[&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			//if moving here from GameOverState then perform actions
			if (_currentState)
				this->_currentState->onButtonTouchEventEnded(this, StatesMisc::ButtonTouchEventEndedSender::PacksButton);

			this->moveToPacksMenuState();
		}
	});
	this->addChild(_packsButton, kzGUI);

	createPacksMenu();

	//player coins
	_playerCoinsNode = Node::create();
	//_playerCoinsNode->setContentSize(Size(_coinsPanel->getTextureRect().size.width, _coinsPanel->getTextureRect().size.height));
	_playerCoinsNode->setVisible(false);
	_playerCoinsNode->setCascadeOpacityEnabled(true);
	_playerCoinsNode->setOpacity(0);
	this->addChild(_playerCoinsNode, kzGUI);

	_coinsPanel = Sprite::createWithSpriteFrameName("coinsPanel.png");
	_coinsPanel->setPosition(Vec2(_screenSize.width * 0.2173f, _screenSize.height * 0.8524f));
	_coinsPanel->setVisible(true);
	_playerCoinsNode->addChild(_coinsPanel, kzGUI);
	
	_coinsLbl = Label::createWithTTF(std::to_string(_score), "fonts/arial.ttf", 57.0f, Size::ZERO, TextHAlignment::RIGHT, TextVAlignment::CENTER);
	_coinsLbl->setPosition(_screenSize.width * 0.2523f, _screenSize.height * 0.8618f);
	_coinsLbl->setDimensions(_screenSize.width * 0.2083f, _screenSize.height * 0.0619f);
	_coinsLbl->setTextColor(Color4B::BLACK);
	_coinsLbl->setVisible(true);
	updateCoinsLbl(_coins);
	_playerCoinsNode->addChild(_coinsLbl, kzGUI);

}


void DejavuGame::createPacksMenu(void) {

	_packsMenuNode = Node::create();
	_packsMenuNode->setContentSize(Size(_screenSize.width, _screenSize.height));
	_packsMenuNode->setVisible(false);
	this->addChild(_packsMenuNode, kzGUI);

	_packsMenuBG = Sprite::createWithSpriteFrameName("listViewImage.png");
	_packsMenuBG->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
	_packsMenuBG->setContentSize(Size(_screenSize.width * 0.97f, _screenSize.height * 0.97f));
	_packsMenuBG->setVisible(true);
	_packsMenuNode->addChild(_packsMenuBG);

	_packsPageView = ui::PageView::create();
	_packsPageView->setAnchorPoint(Vec2(0.5, 0.5));
	_packsPageView->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
	_packsPageView->setVisible(true);
	_packsPageView->setContentSize(Size(_screenSize.width * 0.97f, _screenSize.height * 0.97f));
	_packsPageView->setItemsMargin(_screenSize.width * 0.07f);
	_packsMenuNode->addChild(_packsPageView);

	/* TURNING end event
	_packsPageView->addEventListener([&](Ref* sender, ui::PageView::EventType event) {

	});
	*/

	//create packs layouts and buttons

	for (int i = 0; i != _packsByIndexVec.size(); i++) {
		const packData* pack = _packsByIndexVec[i];

		ui::Layout* pageLayout = ui::Layout::create();

		//create pack button
		ui::Button* button = ui::Button::create(pack->packThumbnail, pack->packThumbnail, pack->packLockedThumbnail, ui::Widget::TextureResType::PLIST);
		button->setPosition(Vec2(_packsPageView->getContentSize().width * 0.5f, _packsPageView->getContentSize().height * 0.5f));
		button->addTouchEventListener(
			[=](Ref* sender, ui::Widget::TouchEventType type) {
			if (type == ui::Widget::TouchEventType::ENDED)
			{
				//select pack
				int buttonIndexTag = pack->packIndex;
				int currentPageIndex = _packsPageView->getCurrentPageIndex();
				if (buttonIndexTag == currentPageIndex)
				{
					_currentPageIndex = currentPageIndex;
					setIconPack(_packsByIndexVec[_currentPageIndex]->packName);
					setDifficultyLevel(_packsByIndexVec[_currentPageIndex]->packDifficultyLevel);
					setScoreModifier(_packsByIndexVec[_currentPageIndex]->packPriceEntry);

					updateUserSelectedPack(_currentPackName);
					this->moveToMainMenuState();
				}
			}
		});
		pageLayout->addChild(button);

		bool isPackLocked = (_availPacksSet.find(pack->packName) == _availPacksSet.end()) ? true : false;

		ui::Button* buyButton = nullptr;
		Label* priceLbl = nullptr;

		if (isPackLocked)
		{
			priceLbl = Label::createWithTTF(std::to_string(_score), "fonts/arial.ttf", 80.0f, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
			priceLbl->setPosition(_packsPageView->getContentSize().width * 0.5474f, _packsPageView->getContentSize().height * 0.1380f);
			priceLbl->setTextColor(Color4B::BLACK);
			string price = std::to_string(_packPricesVec[pack->packPriceEntry].packPrice);
			priceLbl->setString(price);
			pageLayout->addChild(priceLbl, kzTop);
			
			buyButton = ui::Button::create("buyCatalogButton2.png", "buyCatalogButton2.png", "buyCatalogButton2.png", ui::Widget::TextureResType::PLIST);
			buyButton->setPosition(Vec2(_packsPageView->getContentSize().width * 0.5f, _packsPageView->getContentSize().height * 0.1328f));
			buyButton->addTouchEventListener(
				[=](Ref* sender, ui::Widget::TouchEventType type) {
				if (type == ui::Widget::TouchEventType::ENDED)
				{
					//int buttonIndexTag = buyButton->getTag();
					int buttonIndexTag = pack->packIndex;
					int currentPageIndex = _packsPageView->getCurrentPageIndex();
					unsigned int packPrice = _packPricesVec[_packsByIndexVec[currentPageIndex]->packPriceEntry].packPrice;
					if ((buttonIndexTag == currentPageIndex && _coins >= packPrice) ||
						(_godMode == true))
					{
						_coins = _coins - packPrice;
						updateCoinsLbl(_coins);
						_availPacksSet.insert(pack->packName);

						std::string availPacks;
						for (auto elem : _availPacksSet)
							availPacks += (elem + ";");

						updateUserCoins(this->_coins);
						updateUserPacks(availPacks);

						//add pack to available packs vector
						button->setEnabled(true);
						buyButton->setEnabled(false);
						buyButton->setVisible(false);
						priceLbl->setVisible(false);
					}
				}
			});
			pageLayout->addChild(buyButton, kzGUI);
		}

		if (isPackLocked) {
			button->setEnabled(false);
			buyButton->setEnabled(true);
		}
		else {
			button->setEnabled(true);
		}

		_packsPageView->insertPage(pageLayout, pack->packIndex);
	}
	
	//set starting pack in GUI
	auto it = _packsDataMap.find(_currentPackName);
	if (it != _packsDataMap.end())
		_currentPageIndex = it->second.packIndex;
	else
		_currentPageIndex = DEFAULT_PACK_INDEX;
	
	_packsPageView->setCurrentPageIndex(_currentPageIndex);

	/*
	_packsMenuSelectButton = Button::create("selectButton.png", "selectButton.png", "selectButton.png", ui::Widget::TextureResType::PLIST);
	_packsMenuSelectButton->setPosition(Vec2(_screenSize.width * 0.5000f, _screenSize.height * 0.1328f));
	_packsMenuSelectButton->addTouchEventListener(
		[&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
		{
			//select new pack
			//IMPORTANT: order of the packs is determined by packs order in data file
			_currentPageIndex = _packsPageView->getCurrentPageIndex();
			string newIconPack = _packsByIndexVec[_currentPageIndex]->packName;
			setIconPack(newIconPack);
			setDifficultyLevel(_packsByIndexVec[_currentPageIndex]->packDifficultyLevel);
			updateUserSelectedPack();
			this->moveToMainMenuState();
		}
	});
	_packsMenuNode->addChild(_packsMenuSelectButton);
	*/
	
	_packsMenuBackButton = Button::create("cancelButton.png", "cancelButton.png", "cancelButton.png", ui::Widget::TextureResType::PLIST);
	_packsMenuBackButton->setPosition(Vec2(_screenSize.width * 0.8402f, _screenSize.height * 0.8691f));
	_packsMenuBackButton->addTouchEventListener(
		[&](Ref* sender, ui::Widget::TouchEventType type) {
		if (type == ui::Widget::TouchEventType::ENDED)
		{			
			this->moveToMainMenuState();
		}
	});
	_packsMenuNode->addChild(_packsMenuBackButton);

}

void DejavuGame::setIconPack(const string& newIconPack) {

	if (_currentPackName == newIconPack)
		return;

	//change current pack
	_currentPackName = newIconPack;
	setPackSprites(newIconPack); //sets new value to _currentPackSprites
	_bgLayer->setBackgroundSprites(_currentPackSprites);
}

//------------------------------------------ TOUCH --------------------------------
bool DejavuGame::onTouchBegan(Touch* touch, Event* event) {
    if (touch) {
		StatesMisc::Touch tap(touch->getLocation().x, touch->getLocation().y);
		if (_currentState)
			return _currentState->onTouchBegan(this, tap);
    }
    return true;
}

void DejavuGame::onTouchMoved(Touch* touch, Event* event) {
	//do nothing
}

void DejavuGame::onTouchEnded(Touch* touch, Event* event) {
    if (touch) {
        Point pointTouchUp = touch->getLocation();
		StatesMisc::SwipeDirection dir = calcSwipeDirection(_touchDown, pointTouchUp);
		StatesMisc::Touch smTouch(touch->getLocation().x, touch->getLocation().y);
		if (_currentState)
			_currentState->onTouchEnded(this, smTouch, dir);

    }
}

void DejavuGame::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {
    //do nothing here
}

void DejavuGame::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event) {
    //android back button will exit the game
    if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
        Director::getInstance()->end();
    }
}

//------------------------------------------ UPDATE FUNC --------------------------------
void DejavuGame::update(float dt) {
	
	if (_currentState)
		this->_currentState->update(this, dt);

}

//switch to InitGameState if you wish to call this function
//use context->moveToInitGameState(); 
void DejavuGame::initGame() {

	int totalIcons = _currentPackSprites->size();
	int iconIndex;
	std::set<int> uniqueSpriteIndexSet;
	std::set<int>::iterator uniqueSpriteIndex;

	_gameplaySpriteNames.clear();
	while (uniqueSpriteIndexSet.size() < _numSuitsSingleGame) {
		iconIndex = (rand() % totalIcons);
		//check if index already exists
		uniqueSpriteIndex = uniqueSpriteIndexSet.find(iconIndex);
		if (uniqueSpriteIndex == uniqueSpriteIndexSet.end()) {
			uniqueSpriteIndexSet.insert(iconIndex);
			_gameplaySpriteNames.push_back(_currentPackSprites->at(iconIndex));
		}
	}

	//this saves which cards have been displayed and which haven't
	initCardsPunchBoard();

	//reset player stats
	resetPlayerStats();
	_playerScoreLbl->setString(std::to_string(_score));

	_timer = 0.0f;
	_cardsRevealed = 0;
	_lowSuitWindow = 0;
	_highSuitWindow = 1;
	_currentCardSuiteInd = 0;
	_currentCardRank = 0;
	_isCardAlreadySeen = false;

}

void DejavuGame::clearAllCurrentCardFrontSprites() {
	//hide level up sprites
	_levelUpText->setVisible(false);
	_newSuiteText->setVisible(false);
	_newSuiteIcon->setVisible(false);

	//hide challenge card sprites
	for (int i = 0; i < _currentCardRankIconsVec.size(); i++)
		_currentCardRankIconsVec.at(i)->setVisible(false);

	//hide game over card sprites
	_gameOverText->setVisible(false);
	_yourScoreText->setVisible(false);
	_scoreGameOverLbl->setVisible(false);
}

void DejavuGame::makeChallengeCard() {

	//
	int cardRank = (rand() % _pCurrentDifficultyData->maxCardRankBound) + _pCurrentDifficultyData->minCardRankBound; // [minCardRankBound, maxCardRankBound]
	
	_currentCardSuiteInd = (rand() % _highSuitWindow) + _lowSuitWindow; // [_lowSuitWindow, _highSuitWindow]
	
	//NOTE: cardRank = 0 is not supported!!!
	for (int i = 0; i < _currentCardRankIconsVec.size(); i++)	{
		if (i < cardRank) {
			_currentCardRankIconsVec.at(i)->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(_gameplaySpriteNames[_currentCardSuiteInd]));
			
			_currentCardRankIconsVec.at(i)->setPosition(_currentCardFront->getContentSize().width * _cardRankIconsPositions[cardRank][i].x,
				_currentCardFront->getContentSize().height * _cardRankIconsPositions[cardRank][i].y);
			_currentCardRankIconsVec.at(i)->setVisible(true);
		}
		else {
			_currentCardRankIconsVec.at(i)->setVisible(false);
		}
	}

	//check card status (if has been seen)
	_isCardAlreadySeen = _cardsPunchBoard[cardRank - 1][_currentCardSuiteInd];
	//punch card as seen
	_cardsPunchBoard[cardRank - 1][_currentCardSuiteInd] = true;
}

void DejavuGame::makeLevelUpCard() {
	_levelUpText->setVisible(true);
	_newSuiteText->setVisible(true);

	_newSuiteIcon->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(_gameplaySpriteNames[ _lowSuitWindow + _highSuitWindow -1 ]));
	_newSuiteIcon->setVisible(true);

	_isCardAlreadySeen = false;
}

void DejavuGame::showGameOverCard() {
	_gameOverText->setVisible(true);
	_yourScoreText->setVisible(true);

	unsigned int finalScore = _score * _scoreModifier;
	_scoreGameOverLbl->setString(std::to_string(finalScore));
	_scoreGameOverLbl->setVisible(true);

	//update coins
	_coins += finalScore;
	updateUserCoins(_coins);
}

void DejavuGame::revealNextCard() {
	_currentCardBack->runAction(_flipCard);
	
	//note: _flipCard is a sequence with TargetedActions meaning, the actions will be applied to different nodes than
	//the node runAction was called for.
}

void DejavuGame::resetPlayerStats() {
	_lives = 3;
	_score = 0;
}

void DejavuGame::initCardsPunchBoard(void) {
	_cardsPunchBoard.clear();

	_cardsPunchBoard.resize(_pCurrentDifficultyData->maxCardRankBound);

	for (unsigned int r = 0; r < _pCurrentDifficultyData->maxCardRankBound; r++) {
		for (unsigned int s = 0; s < _numSuitsSingleGame; s++)
			_cardsPunchBoard[r].push_back(false);
	}
}

bool DejavuGame::isPlayerCorrect(const StatesMisc::SwipeDirection& swipeDirection) {
	if ((swipeDirection == StatesMisc::SwipeDirection::LEFT && _isCardAlreadySeen == true)
		|| (swipeDirection == StatesMisc::SwipeDirection::RIGHT && _isCardAlreadySeen == false))
		return false;
	else
		return true;
}

void DejavuGame::decreasePlayerLife() {
	if (_godMode) return;

	_playerLivesSprites.at(_lives - 1)->setVisible(false);
	_lives--;
}


void DejavuGame::initAvailablePacks() {

	size_t pos = 0;
	std::string token;
	while ((pos = _tmpAvailPacks.find(AVAIL_PACKS_DELIMETER)) != std::string::npos) {
		token = _tmpAvailPacks.substr(0, pos);
		_availPacksSet.insert(token);
		_tmpAvailPacks.erase(0, pos + 1 /*delimeter*/);
	}

	if (_tmpAvailPacks.length() != 0) {
		pos = _tmpAvailPacks.length();
		token = _tmpAvailPacks.substr(0, pos);
		_availPacksSet.insert(token);
		_tmpAvailPacks.erase(0, pos);
	}

	//delete the string
	_tmpAvailPacks.clear();

}

void DejavuGame::initGamePack(void) {

	auto it = _packsDataMap.find(_currentPackName);
	if (it != _packsDataMap.end()) {
		setDifficultyLevel(it->second.packDifficultyLevel);
		setScoreModifier(it->second.packPriceEntry);
	}
	else {
		//defaults - set to first icon pack - just in case
		_currentPackName = _packsByIndexVec[DEFAULT_PACK_INDEX]->packName;
		setDifficultyLevel(_packsByIndexVec[DEFAULT_PACK_INDEX]->packDifficultyLevel);
		setDifficultyLevel(_packsByIndexVec[DEFAULT_PACK_INDEX]->packPriceEntry);
	}
	
	setPackSprites(_currentPackName);
}


void DejavuGame::initPositionMaps(void) {
	//init card icons vector
	_cardRankIconsPositions = {
		{},
		{ Point(0.5f, 0.5f) },
		{ Point(0.5f, 0.8106f), Point(0.5f, 0.1894f) },
		{ Point(0.5f, 0.8553f), Point(0.5f, 0.5f), Point(0.5f, 0.1447f)},
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.2440f, 0.1447f), Point(0.7560f, 0.1447f) },
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.5f, 0.5f), Point(0.2440f, 0.1447f), Point(0.7560f, 0.1447f) },
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.2440f, 0.5f), Point(0.7560f, 0.5f), Point(0.2440f, 0.1447f),
			Point(0.7560f, 0.1447f) },
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.5f, 0.6388f), Point(0.2440f, 0.4953f), Point(0.7560f, 0.4953f),
			Point(0.2440f, 0.1447f), Point(0.7560f, 0.1447f) },
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.5f, 0.6388f), Point(0.2440f, 0.4953f), Point(0.7560f, 0.4953f),
			Point(0.5f, 0.3353f), Point(0.2440f, 0.1447f), Point(0.7560f, 0.1447f) },
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.2440f, 0.6129f), Point(0.7560f, 0.6129f), Point(0.5f, 0.5094f),
			Point(0.2440f, 0.3800f), Point(0.7560f, 0.3800f), Point(0.2440f, 0.1447f), Point(0.7560f, 0.1447f) },
		{ Point(0.2440f, 0.8553f), Point(0.7560f, 0.8553f), Point(0.5f, 0.7071f), Point(0.2440f, 0.6129f), Point(0.7560f, 0.6129f),
			Point(0.2440f, 0.3800f), Point(0.7560f, 0.3800f), Point(0.5f, 0.3800f), Point(0.2440f, 0.1447f), Point(0.7560f, 0.1447f) }
	};

	_playerLivesPositions = { Point(0.9243f, 0.9037f),
							  Point(0.8036f, 0.9037f),
							  Point(0.6829f, 0.9037f)
							 
	};

	_cardPosition.x = _screenSize.width * 0.5f;
	_cardPosition.y = _screenSize.height * 0.3898f;

	_particlesSadFacePos.x = _screenSize.width * 0.5f;
	_particlesSadFacePos.y = _screenSize.height * 0.2243f;

	_particlesCoinGainPos.x = _screenSize.width * 0.1007f;
	_particlesCoinGainPos.y = _screenSize.height * 0.8404f;
}

void DejavuGame::playSwipeAnimation(const StatesMisc::SwipeDirection& dir) {
	if (dir == StatesMisc::SwipeDirection::LEFT)
		_currentFrontCardNode->runAction(_currentCardSwapLeft);
	else if (dir == StatesMisc::SwipeDirection::RIGHT)
		_currentFrontCardNode->runAction(_currentCardSwapRight);
}

void DejavuGame::resetCardTimer(void) {
	//reset timer
	this->_timer = 0.0f;
	this->_cardTimer->setPercentage(100.0f);
}

void DejavuGame::advanceCardTimer(const float& dt) {
	this->_timer += dt;
	float percent = 100.0f - ((_timer * 100.0f) / _timerPerCard);
	this->_cardTimer->setPercentage(percent);
}

int DejavuGame::calcPointsValue() {
	int points;
	points = static_cast<int>(_highPointsValue * ((_timerPerCard - _timer) / (_timerPerCard)));
	if (points == 0)
		points = _lowPointsValue;

	return points;
}

void DejavuGame::loadGameData(void) {
	std::string jsonData = FileUtils::getInstance()->getStringFromFile("GameData.json");
	dataFile.Parse(jsonData.c_str());
	
	_numSuitsSingleGame = dataFile[GAMEPLAY_DATA][NUM_SUITS_SINGLE_GAME].GetInt();
	
	_timerPerCard = dataFile[GAMEPLAY_DATA][TIMER_PER_CARD].GetFloat();

	_highPointsValue = dataFile[GAMEPLAY_DATA][HIGH_POINTS_VALUE].GetInt();
	_lowPointsValue = dataFile[GAMEPLAY_DATA][LOW_POINTS_VALUE].GetInt();

	_godMode = dataFile[DEVELOPER_OPTIONS][GOD_MODE].GetBool();

	string packName; //pack name is used as a key

	//load packs
	for (unsigned int i = 0; i < dataFile[PACKS_DATA].Size(); i++) {
		packData pack;
		pack.packName = dataFile[PACKS_DATA][i][PACK_NAME].GetString();
		pack.packThumbnail = dataFile[PACKS_DATA][i][PACK_THUMBNAIL].GetString();
		pack.packLockedThumbnail = dataFile[PACKS_DATA][i][PACK_LOCKED_THUMBNAIL].GetString();
		pack.packDifficultyLevel = dataFile[PACKS_DATA][i][PACK_DIFFICULTY_LEVEL].GetUint();
		pack.packIndex = dataFile[PACKS_DATA][i][PACK_INDEX].GetUint();
		pack.packPriceEntry = dataFile[PACKS_DATA][i][PACK_PRICE_ENTRY].GetUint();

		_packsDataMap.insert(std::make_pair(pack.packName, pack));
	}

	//create packs lookup
	_packsByIndexVec.resize(_packsDataMap.size());
	for (auto it = _packsDataMap.begin(); it != _packsDataMap.end(); it++) {
		packData* pPack = &(it->second);
		_packsByIndexVec[pPack->packIndex] = pPack;
	}

	//load packs sprites
	vector<string> spriteVec;
	for (auto it = _packsDataMap.begin(); it != _packsDataMap.end(); it++) {
		packName = it->second.packName;
		for (rapidjson::SizeType j = 0; j < dataFile[PACKS_SPRITES][packName.c_str()].Size(); ++j) {
			spriteVec.push_back(dataFile[PACKS_SPRITES][packName.c_str()][j].GetString());
		}
		_packsSprites.insert(std::make_pair(packName, spriteVec));
		spriteVec.clear();
	}

	//load levels data
	//assert: at least one difficulty level data
	for (unsigned int i = 0; i < dataFile[DIFFICULTY_DATA].Size(); i++) {
		difficultyLevelData difficultyLevel;
		difficultyLevel.minCardRankBound = dataFile[DIFFICULTY_DATA][i][MIN_CARD_RANK_BOUND].GetInt();
		difficultyLevel.maxCardRankBound = dataFile[DIFFICULTY_DATA][i][MAX_CARD_RANK_BOUND].GetInt();
		difficultyLevel.growthRate = dataFile[DIFFICULTY_DATA][i][GROWTH_RATE].GetInt();
		difficultyLevel.concurrentSuits = dataFile[DIFFICULTY_DATA][i][CONCURRENT_SUITS].GetInt();

		_difficultyLevelsVec.push_back(difficultyLevel);
	}

	//load pricing data
	for (unsigned int i = 0; i < dataFile[PRICE_DATA].Size(); i++) {
		packPriceData priceDataEntry;
		priceDataEntry.packPrice = dataFile[PRICE_DATA][i][PACK_PRICE].GetInt();
		priceDataEntry.scoreModifier = dataFile[PRICE_DATA][i][SCORE_MODIFIER].GetInt();

		_packPricesVec.push_back(priceDataEntry);
	}

}

//this funtion sets new sprite vector to _currentPackSprites
void DejavuGame::setPackSprites(const string& currentIconPack) {
	
	auto packsSpritesIter = _packsSprites.find(currentIconPack);
	if (packsSpritesIter == _packsSprites.end())
		throw("Icon pack %s doesn't exists", currentIconPack.c_str());
	
	_currentPackSprites = &(packsSpritesIter->second);

}

void DejavuGame::setDifficultyLevel(unsigned int difficultyLevel) {
	
	if (difficultyLevel < 0 || difficultyLevel >= _difficultyLevelsVec.size())
		_pCurrentDifficultyData = &(_difficultyLevelsVec[0]);
	else
		_pCurrentDifficultyData = &(_difficultyLevelsVec[difficultyLevel]);
}


void DejavuGame::setScoreModifier(unsigned int priceEntry) {

	if (priceEntry < 0 || priceEntry >= _packPricesVec.size())
		_scoreModifier = _packPricesVec[0].scoreModifier;
	else
		_scoreModifier = _packPricesVec[priceEntry].scoreModifier;
}


void DejavuGame::loadGameConfig() {

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	disableAds = false;
#endif

}

void DejavuGame::loadUserCustomData() {
	
	_currentPackName = _userDefault->getStringForKey(USER_SELECTED_PACK);
	if (_currentPackName.empty())
		_currentPackName = _packsByIndexVec[DEFAULT_PACK_INDEX]->packName;

	_tmpAvailPacks = _userDefault->getStringForKey(USER_AVAILABLE_PACKS);
	if (_tmpAvailPacks.empty())
		_tmpAvailPacks = _packsByIndexVec[DEFAULT_PACK_INDEX]->packName;

	_coins = _userDefault->getIntegerForKey(USER_COINS);
	_highScore = _userDefault->getIntegerForKey(USER_HIGH_SCORE);
}


void DejavuGame::updateUserSelectedPack(const std::string& selecedPack) {
	_userDefault->setStringForKey(USER_SELECTED_PACK, selecedPack);
}

void DejavuGame::updateUserCoins(const int& coins) {
	_userDefault->setIntegerForKey(USER_COINS, coins);
}

void DejavuGame::updateUserPacks(const std::string& availPacks) {
	_userDefault->setStringForKey(USER_AVAILABLE_PACKS, availPacks);
}

void DejavuGame::saveUserPacksCoins(const int& coins, const std::string& availPacks) {
	updateUserCoins(coins);
	updateUserPacks(availPacks);
	_userDefault->flush();
}


//------------------------------------------ SHOW/HIDE --------------------------

void DejavuGame::showPlayerLives() {
	//display lives
	for (unsigned int i = 0; i < _lives; i++)
		_playerLivesSprites.at(i)->setVisible(true);
}

void DejavuGame::hidePlayerLives() {
	for (int i = 0; i < _playerLivesSprites.size(); i++)
		_playerLivesSprites.at(i)->setVisible(false);
}

void DejavuGame::showMainMenuHUD() {
	this->_startButton->setEnabled(true);
	this->_startButton->setVisible(true);

	this->_packsButton->setEnabled(true);
	this->_packsButton->setVisible(true);
}

void DejavuGame::hideMainMenuHUD() {
	this->_startButton->setEnabled(false);
	this->_startButton->setVisible(false);

	this->_packsButton->setEnabled(false);
	this->_packsButton->setVisible(false);
}


void DejavuGame::showGameplayHUD() {
	_playerLivesPanel->setVisible(true);
	showPlayerLives();
	_playerScoreLbl->setVisible(true);
	_playerScorePanel->setVisible(true);
	_cardTimer->setVisible(true);
	_timerIcon->setVisible(true);
}

void DejavuGame::hideGameplayHUD() {
	_playerLivesPanel->setVisible(false);
	hidePlayerLives();
	_playerScoreLbl->setVisible(false);
	_playerScorePanel->setVisible(false);
	_cardTimer->setVisible(false);
	_timerIcon->setVisible(false);
}

void DejavuGame::showPacksMenu() {
	_packsMenuNode->setVisible(true);

	if (!_playerCoinsFadeInOut->isDone())
		_playerCoinsNode->stopAction(_playerCoinsFadeInOut);

	_playerCoinsNode->setOpacity(255);
	_playerCoinsNode->setVisible(true);
	_particlesCoinGain->resetSystem();
}

void DejavuGame::hidePacksMenu() {
	_packsMenuNode->setVisible(false);
	_playerCoinsNode->setOpacity(0);
	_playerCoinsNode->setVisible(false);
}

void DejavuGame::showCoinsPanel() {

	updateCoinsLbl(_coins);
	_playerCoinsNode->setVisible(true);
	_playerCoinsNode->runAction(_playerCoinsFadeInOut);
	
}

void DejavuGame::updateCoinsLbl(const int& coins)
{
	this->_coinsLbl->setString(std::to_string(coins));
}


//------------------------------------------ ADS --------------------------------
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
void DejavuGame::addAdsListeners() {
	if (disableAds) return;

	sdkbox::PluginSdkboxAds::setListener(this);
}

void DejavuGame::cacheAds() {
	if (disableAds) return;
	
	sdkbox::PluginSdkboxAds::cache("AdMob", "gameover");
}

void DejavuGame::onAdAction(const std::string& ad_unit_id, const std::string& zone, sdkbox::AdActionType action_type) {
    switch (action_type) {
        case sdkbox::AdActionType::AD_ENDED:
            if (ad_unit_id == "AdMob" && zone == "gameover") {
				if (_currentState)
					_currentState->onCallbackEvent(this, StatesMisc::CallbackEvents::onGmeOverAdEnded);
            }

            break;
        default:
            break;
    }
}

void DejavuGame::onRewardAction(const std::string& ad_unit_id, const std::string& zone_id, float reward_amount, bool reward_succeed) {
    return;
}

//return true if placement shows, return false if placement fails to show
bool DejavuGame::checkAndShowPlacement(std::string placementName) {
	if (disableAds) return false;

	if (sdkbox::PluginSdkboxAds::isAvailable(placementName)) {
        sdkbox::PluginSdkboxAds::placement(placementName);
	    return true;
	} else
	    return false;

}
#endif
//------------------------------------------ MISC --------------------------------

float DejavuGame::angleOf2Points(Point touchDown, Point touchUp) {
	
	float angle = -1.0f;
	if (touchUp.equals(touchDown))
		angle = -1.0f;
	else {
		touchUp.subtract(touchDown);
		touchUp.normalize();
		angle = CC_RADIANS_TO_DEGREES((float)atan2(touchUp.y, touchUp.x));
		
		if (angle < 0) angle += 360;
	}
	return angle;
}

StatesMisc::SwipeDirection DejavuGame::directionByAngle(float angle) {
	StatesMisc::SwipeDirection dir = StatesMisc::SwipeDirection::NONE;
	if ((angle > -1.0f && angle <= 45.0f) || (angle > 315.0f && angle <= 360.0f))
		dir = StatesMisc::SwipeDirection::RIGHT;
	else if (angle > 135.0f && angle <= 225.0f)
		dir = StatesMisc::SwipeDirection::LEFT;

	return dir;
}

StatesMisc::SwipeDirection DejavuGame::calcSwipeDirection(Point touchDown, Point touchUp) {
    float angle = angleOf2Points(touchDown, touchUp);
	StatesMisc::SwipeDirection dir = directionByAngle(angle);
    return dir;
}

//------------------------------------------ STATES --------------------------------

void DejavuGame::setState(const string& stateName) {

	if (_currentState)
		_currentState->onExit(this);

	auto statesIter = _statesMap.find(stateName);
	if (statesIter != _statesMap.end()) {
		//switch to new state
		_currentState = statesIter->second;
		_currentState->onEnter(this);
	}
	else
		throw("Context::setState: no state found");
}

void DejavuGame::addState(const string& stateName, State* state) {
	_statesMap.insert(std::make_pair(stateName, state));
}

void DejavuGame::moveToMainMenuState() {
	this->setState(C_MAIN_MENU_STATE);
}
void DejavuGame::moveToInitGameState() {
	this->setState(C_INIT_GAMEPLAY_STATE);
}
void DejavuGame::moveToGameplayState() {
	this->setState(C_GAMEPLAY_STATE);
}

void DejavuGame::moveToGameOverState() {
	this->setState(C_GAMEOVER_STATE);
}

void DejavuGame::moveToPacksMenuState() {
	this->setState(C_PACKS_MENU_STATE);
}


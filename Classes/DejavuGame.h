#pragma once
#ifndef __DEJAVUGAME_H__
#define __DEJAVUGAME_H__

#include "cocos2d.h"
using namespace cocos2d;

#include "ui/CocosGUI.h"
using ui::Button;

#include <vector>
using std::vector;
#include <string>
using std::string;
#include <map>
using std::map;
#include <unordered_map>
using std::unordered_map;
#include <unordered_set>
using std::unordered_set;


#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include "json/rapidjson.h"	/* jason reader */
#include "json/document.h"	/* jason object */

#include "StateMachine/State.h"
#include "StateMachine/MainMenuState.h"
#include "StateMachine/InitGamePlayState.h"
#include "StateMachine/GamePlayState.h"
#include "StateMachine/GameOverState.h"
#include "StateMachine/PacksMenuState.h"

#include "GameObjects/MovingBG.h"
#include "GameObjects/TimeGradient.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "../proj.android/app/jni/PluginSdkboxAds/PluginSdkboxAds.h"
#endif

enum SpriteTag{
	kSpriteCardIcons,
	kSpriteLevelUpCard
};

enum {
	kzBackground,
	kzForeground,
	kzDeckBack,
	kzCardBack,
	kzCardFront,
	kzCardIcons,
	kzGUI,
	kzTop
};

struct packData {
	std::string packName;
	std::string packThumbnail;
	std::string packLockedThumbnail;
	unsigned int packDifficultyLevel;
	unsigned int packIndex;
	unsigned int packPriceEntry;
};

struct difficultyLevelData {
	unsigned int minCardRankBound;
	unsigned int maxCardRankBound;
	unsigned int growthRate;
	unsigned int concurrentSuits;
};

struct packPriceData {
	unsigned int packPrice;
	unsigned int scoreModifier;
};

/*--------------	MAIN CLASS	-------------------*/
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
class DejavuGame : public cocos2d::Layer, public sdkbox::PluginSdkboxAdsListener
#else
class DejavuGame : public cocos2d::Layer
#endif
{

private:
	//state machine
	unordered_map<string, State*> _statesMap;
	void setState(const string& stateName);
	void addState(const string& stateName, State* state);
	
	State* _currentState;
	const string C_MAIN_MENU_STATE = "MainMenuState";
	const string C_INIT_GAMEPLAY_STATE = "InitGamePlayState";
	const string C_GAMEPLAY_STATE = "GamePlayState";
	const string C_GAMEOVER_STATE = "GameOverState";
	const string C_PACKS_MENU_STATE = "PacksMenuState";

public:
	//game states
	void moveToMainMenuState();
	void moveToInitGameState();
	void moveToGameplayState();
	void moveToGameOverState();
	void moveToPacksMenuState();

	//game data
	UserDefault* _userDefault;
	rapidjson::Document dataFile;
	unsigned int _numSuitsSingleGame;	//how many suits are in a single run of the game
	float _timerPerCard;
	bool _godMode;
	int _highPointsValue;
	int _lowPointsValue;
	vector<difficultyLevelData> _difficultyLevelsVec;
	vector<packPriceData> _packPricesVec;
	vector<string>* _currentPackSprites;
	const difficultyLevelData* _pCurrentDifficultyData;
	unordered_map<std::string, packData> _packsDataMap;
	vector<packData*> _packsByIndexVec;
	string _tmpAvailPacks;
	unordered_set<string> _availPacksSet;

	string _currentPackName;
	unsigned int _scoreModifier;

	unordered_map<string, vector<string>> _packsSprites;
	
	//position maps
	vector<vector<Point>> _cardRankIconsPositions;
	Point _particlesSadFacePos;
	Point _particlesCoinGainPos;
	Point _cardPosition;	//used for deck, back card and front
	vector<Point> _playerLivesPositions;

	//game management
	Size _screenSize;

	//player stats
	unsigned int _lives;
	unsigned int _score;
	unsigned int _coins;
	unsigned int _highScore;

	//gameplay assets
	Node* _currentFrontCardNode;
	SpriteBatchNode* _spriteBatchNode;
	SpriteBatchNode* _spriteBatchFrontCard;
	TimeGradient *_bgGradient;
	MovingBG* _bgLayer;
	Vector<Sprite*> _currentCardRankIconsVec;
	Sprite* _currentCardFront;
	Sprite* _currentCardBack;
	Sprite* _deckBack;
	vector<string> _gameplaySpriteNames;
	Sprite* _timerIcon;
	ProgressTimer* _cardTimer;
	ParticleSystem* _particlesSadFace;
	ParticleSystem* _particlesCoinGain;
	Sprite* _playerLivesPanel;
	Label* _playerScoreLbl;
	Sprite* _playerScorePanel;
	Vector<Sprite*> _playerLivesSprites;
	Sprite* _levelUpText;
	Sprite* _newSuiteText;
	Sprite* _newSuiteIcon;
	Sprite* _gameOverText;
	Sprite* _yourScoreText;
	Label* _scoreGameOverLbl;
	Sprite* _instructionsLayout;
	
	//gui elements
	Button* _helpButton;
	Button* _startButton;
	Button* _packsButton;
	
	//packs menu
	Node* _packsMenuNode;
	Sprite* _packsMenuBG;
	ui::PageView* _packsPageView;
	unsigned int _currentPageIndex;
	Button* _packsMenuSelectButton;
	Button* _packsMenuBackButton;

	//player gold panel
	Node* _playerCoinsNode;
	Sprite* _coinsPanel;
	Label* _coinsLbl;

	//gameplay mechanism
	float _timer;
	unsigned int _cardsRevealed;	//counts how many cards have been revealed
	unsigned int _lowSuitWindow;
	unsigned int _highSuitWindow;
	std::vector<vector<bool>> _cardsPunchBoard;
	unsigned int _currentCardRank;
	unsigned int _currentCardSuiteInd;
	bool _isCardAlreadySeen;

	//sequences and actions
	Sequence* _flipCard;
	Sequence* _currentCardTilt;
	//Sequence* _currentCardGameOverTilt;
	Sequence* _currentCardSwapLeft;
	Sequence* _currentCardSwapRight;
	Sequence* _instructionsLayoutFadeInOut;
	Sequence* _startButtonAppear;
	Sequence* _playerCoinsFadeInOut;

	//gesture mechanism
	Point _touchDown;
	float _swipeAngle;
	float angleOf2Points(Point touchDown, Point touchUp);
	StatesMisc::SwipeDirection directionByAngle(float angle);
	StatesMisc::SwipeDirection calcSwipeDirection(Point touchDown, Point touchUp);

	//callbacks seq functions
	void flipCardBackEnds(void);
	void flipCurrentCardBegins(void);
	void flipCurrentSeqEnds(void);
	void currentCardTiltEnded(void);
	//void currentCardGameOverTiltEnded(void);
	void currentCardSwapSeqEnded(void);
	void resetCurrentCardFrontSprite(void);
	void instructionsLayoutFadeInOutEnded(void);
	void playerCoinsFadeInOutEnded(void);

	//private functions
	void createStates();
	void initDataMembers(void);
	void createGameScreen(void);
	void createBackground();
	void createActions(void);
	void createParticles(void);
	void createGUI(void);
	void createPacksMenu(void);
	void loadGameData(void);
	void setPackSprites(const string& currentIconPack);
	void setIconPack(const string& newIconPack);
	void setDifficultyLevel(unsigned int difficultyLevel);
	void setScoreModifier(unsigned int priceEntry);
	void loadGameConfig(void);
	void loadUserCustomData(void);
	void updateUserSelectedPack(const std::string& selecedPack);
	void updateUserCoins(const int& coins);
	void updateUserPacks(const std::string& availPacks);
	void saveUserPacksCoins(const int& coins, const std::string& availPacks);
	void updateCoinsLbl(const int& coins);
	void resetPlayerStats(void);
	void initCardsPunchBoard(void);
	void initAvailablePacks(void);
	void initGamePack(void);
	void initPositionMaps(void);
	void decreasePlayerLife(void);
	void playSwipeAnimation(const StatesMisc::SwipeDirection& dir);
	void resetCardTimer(void);
	void advanceCardTimer(const float& dt);
	int calcPointsValue();
	void clearAllCurrentCardFrontSprites(void);
	void showMainMenuHUD();
	void hideMainMenuHUD();
	void showGameplayHUD();
	void hideGameplayHUD();
	void showPlayerLives();
	void hidePlayerLives();
	void showPacksMenu();
	void hidePacksMenu();
	void showCoinsPanel();

	//gameplay functions
	void initGame();
	void makeChallengeCard();
	void makeLevelUpCard();
	void showGameOverCard();
	void revealNextCard();
	bool isRevealNextCard;
	bool isPlayerCorrect(const StatesMisc::SwipeDirection& swipeDirection);

	//ads
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	bool disableAds;
	void addAdsListeners();
    void onAdAction(const std::string& ad_unit_id, const std::string& zone, sdkbox::AdActionType action_type);
    void onRewardAction( const std::string& ad_unit_id, const std::string& zone_id, float reward_amount, bool reward_succeed);
	void cacheAds();
	bool checkAndShowPlacement(std::string placementName);
#endif
	

public:
	//static creation methods
    static cocos2d::Scene* createScene();
	CREATE_FUNC(DejavuGame);
	virtual bool init();

	DejavuGame();
	virtual ~DejavuGame();

	//touch events
	virtual bool onTouchBegan(Touch* touch, Event* event);
	virtual void onTouchMoved(Touch* touch, Event* event);
	virtual void onTouchEnded(Touch* touch, Event* event);

	virtual void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	virtual void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	virtual void update(float dt);
};

#endif //__DEJAVUGAME_H__
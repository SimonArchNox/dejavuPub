#pragma once
#ifndef __STATES_MISC_H__
#define __STATES_MISC_H__

namespace StatesMisc 
{
	enum class CallbackEvents 
	{ 
		currentCardSwapSeqEnded,
		flipCardBackEnds, 
		flipCurrentCardBegins, 
		flipCurrentSeqEnds,
		currentCardTiltEnded, 
		instructionsLayoutFadeInOutEnded,
		playerCoinsFadeInOutEnded,
		onGmeOverAdEnded
	};

	//contains element + touch event type
	enum class ButtonTouchEventEndedSender {
		StartButton,
		PacksButton
	};

	//Note: this is the same enum as in context
	enum class SwipeDirection { NONE, UP, DOWN, LEFT, RIGHT };

	struct Touch {
		float x;
		float y;

		Touch() : x(0), y(0) {};
		Touch(float xx, float yy) : x(xx), y(yy) {};
	};

}

#endif //__STATES_MISC_H__
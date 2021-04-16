#ifndef __CCKEYBOARD_DISPATCHER_H__
#define __CCKEYBOARD_DISPATCHER_H__

#include "CCKeyboardDelegate.h"
#include "cocoa/CCArray.h"

RT_ADD(
	NS_CC_BEGIN

	class CC_DLL CCKeyboardDispatcher : public CCObject 
	{
	public:
		CCKeyboardDispatcher();
		CCKeyboardDispatcher(const CCKeyboardDispatcher&);
		virtual ~CCKeyboardDispatcher();

		CCKeyboardDispatcher& operator=(const CCKeyboardDispatcher&);

		void addDelegate(CCKeyboardDelegate* pDelegate);

		void removeDelegate(CCKeyboardDelegate* pDelegate);

		void forceAddDelegate(CCKeyboardDelegate* pDelegate);

		void forceRemoveDelegate(CCKeyboardDelegate* pDelegate);

		enumKeyCodes convertKeyCode(enumKeyCodes key);

		bool dispatchKeyboardMSG(enumKeyCodes key, bool);
		
		bool getAltKeyPressed() const;

		bool getCommandKeyPressed() const;

		bool getControlKeyPressed() const;

		bool getShiftKeyPressed() const;

		const char* keyToString(enumKeyCodes key);

		void updateModifierKeys(bool, bool, bool, bool);

	protected:
		CCArray* m_pDelegates;
		//there's more here, check the initializer
	};

	NS_CC_END
)

#endif
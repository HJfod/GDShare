#ifndef __CCMOUSE_DELEGATE_H__
#define __CCMOUSE_DELEGATE_H__

#include "ccMacros.h"
#include "cocoa/CCObject.h"

RT_ADD(
	NS_CC_BEGIN

	class CC_DLL CCMouseDelegate 
	{
	public:
		CCMouseDelegate(const CCMouseDelegate&);
		CCMouseDelegate();

		CCMouseDelegate& operator=(const CCMouseDelegate&);

		virtual void rightKeyDown();

		virtual void rightKeyUp();

		virtual void scrollWheel(float x, float y);

		//pretty certain there's no fields, based on initializer
	};

	class CC_DLL CCMouseHandler : public CCObject
	{
	public:
		CCMouseHandler(const CCMouseHandler&);
		CCMouseHandler();
		virtual ~CCMouseHandler();

		CCMouseHandler& operator=(const CCMouseHandler&);

		CCMouseDelegate* getDelegate();

		static CCMouseHandler* handlerWithDelegate(CCMouseDelegate* pDelegate);

		virtual bool initWithDelegate(CCMouseDelegate* pDelegate);

		void setDelegate(CCMouseDelegate* pDelegate);

	protected:
		CCMouseDelegate* m_pDelegate;
	};

	NS_CC_END
)

#endif
#ifndef __CCMOUSE_DISPATCHER_H__
#define __CCMOUSE_DISPATCHER_H__

#include "CCMouseDelegate.h"
#include "cocoa/CCArray.h"

RT_ADD(
	NS_CC_BEGIN

	class CC_DLL STUB(CCMouseDispatcher) : public CCObject 
	{
	public:
		CCMouseDispatcher();
		CCMouseDispatcher(const CCMouseDispatcher&);
		virtual ~CCMouseDispatcher();

		CCMouseDispatcher& operator=(const CCMouseDispatcher&);

		void addDelegate(CCMouseDelegate* pDelegate);

		void removeDelegate(CCMouseDelegate* pDelegate);

		void forceAddDelegate(CCMouseDelegate* pDelegate);

		void forceRemoveDelegate(CCMouseDelegate* pDelegate);

		bool dispatchScrollMSG(float x, float y);

	protected:
		CCArray* m_pDelegates;
		//there's more here, check the initializer
	};

	NS_CC_END
)

#endif
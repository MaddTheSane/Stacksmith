//
//  CButtonPart.h
//  Stacksmith
//
//  Created by Uli Kusterer on 2014-01-02.
//  Copyright (c) 2014 Uli Kusterer. All rights reserved.
//

#ifndef __Stacksmith__CButtonPart__
#define __Stacksmith__CButtonPart__

#include "CVisiblePart.h"

namespace Calhoun {

class CButtonPart : public CVisiblePart
{
public:
	explicit CButtonPart( CLayer *inOwner ) : CVisiblePart( inOwner ) {};
	
protected:
	virtual void			LoadPropertiesFromElement( tinyxml2::XMLElement * inElement );
	
	virtual const char*		GetIdentityForDump()	{ return "Button"; };
	virtual void			DumpProperties( size_t inIndent );

protected:
	bool			mShowName;
	bool			mHighlight;
	bool			mAutoHighlight;
	bool			mSharedHighlight;
	int				mTitleWidth;
	WILDObjectID	mIconID;
	TPartTextAlign	mTextAlign;
	std::string		mFont;
	int				mTextSize;
};

}

#endif /* defined(__Stacksmith__CButtonPart__) */

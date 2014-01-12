//
//  CPartContents.h
//  Stacksmith
//
//  Created by Uli Kusterer on 30.12.13.
//  Copyright (c) 2013 Uli Kusterer. All rights reserved.
//

#ifndef __Stacksmith__CPartContents__
#define __Stacksmith__CPartContents__

#include "CRefCountedObject.h"
#include "tinyxml2.h"
#include "CObjectID.h"
#include <string>

namespace Carlson {

class CPartContents : public CRefCountedObject
{
public:
	CPartContents( tinyxml2::XMLElement * inElement );
	
	virtual void		Dump( size_t inIndent );
	
	ObjectID		GetID()					{ return mID; };
	bool				GetHighlight()			{ return mHighlight; };
	std::string			GetText()				{ return mText; };
	bool				GetIsOnBackground()		{ return mIsOnBackground; };

protected:
	ObjectID		mID;				// ID of the object whose contents we contain.
	bool				mIsOnBackground;	// Is the object with ID mID on the background or on the card layer?
	bool				mHighlight;			// The highlight property for a background button with sharedHighlight == FALSE.
	std::string			mText;				// The actual text contents.
};


typedef CRefCountedObjectRef<CPartContents>		CPartContentsRef;

}

#endif /* defined(__Stacksmith__CPartContents__) */

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
	explicit CPartContents( tinyxml2::XMLElement * inElement = NULL );
	
	virtual void		Dump( size_t inIndent );
	
	ObjectID			GetID()							{ return mID; };
	void				SetID( ObjectID inID )			{ mID = inID; };
	bool				GetHighlight()					{ return mHighlight; };
	void				SetHighlight( bool inHighlight ){ mHighlight = inHighlight; };
	std::string			GetText()						{ return mText; };
	void				SetText( std::string inText )	{ mText = inText; };
	bool				GetIsOnBackground()				{ return mIsOnBackground; };
	void				SetIsOnBackground( bool inBg )	{ mIsOnBackground = inBg; };

protected:
	ObjectID			mID;				// ID of the object whose contents we contain.
	bool				mIsOnBackground;	// Is the object with ID mID on the background or on the card layer?
	bool				mHighlight;			// The highlight property for a background button with sharedHighlight == FALSE.
	std::string			mText;				// The actual text contents.
};


typedef CRefCountedObjectRef<CPartContents>		CPartContentsRef;

}

#endif /* defined(__Stacksmith__CPartContents__) */

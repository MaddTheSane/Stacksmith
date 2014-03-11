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
#include "CAttributedString.h"
#include <string>

namespace Carlson {

class CLayer;


class CPartContents : public CRefCountedObject
{
public:
	explicit CPartContents( CLayer* owningLayer, tinyxml2::XMLElement * inElement = NULL );
	virtual ~CPartContents()	{};
	
	virtual void		SaveToElementAndStyleSheet( tinyxml2::XMLElement * inElement, CStyleSheet *styleSheet );
	virtual void		Dump( size_t inIndent );
	
	ObjectID			GetID()							{ return mID; };
	void				SetID( ObjectID inID )			{ mID = inID; IncrementChangeCount(); };
	bool				GetHighlight()					{ return mHighlight; };
	void				SetHighlight( bool inHighlight ){ mHighlight = inHighlight; IncrementChangeCount(); };
	std::string			GetText()						{ return mAttributedString.GetString(); };
	void				SetText( std::string inText )	{ mAttributedString.SetString( inText ); IncrementChangeCount(); };
	CAttributedString&	GetAttributedText()				{ return mAttributedString; };
	void				SetAttributedText( const CAttributedString& inAttrStr )		{ mAttributedString = inAttrStr; IncrementChangeCount(); };
	bool				IsOnBackground()				{ return mIsOnBackground; };
	void				SetIsOnBackground( bool inBg )	{ mIsOnBackground = inBg; };
	
	virtual void		IncrementChangeCount();

protected:
	ObjectID			mID;				// ID of the object whose contents we contain.
	bool				mIsOnBackground;	// Is the object with ID mID on the background or on the card layer?
	bool				mHighlight;			// The highlight property for a background button with sharedHighlight == FALSE.
	CAttributedString	mAttributedString;	// Actual text & styles of these contents.
	CLayer		*		mOwningLayer;
};


typedef CRefCountedObjectRef<CPartContents>		CPartContentsRef;

}

#endif /* defined(__Stacksmith__CPartContents__) */

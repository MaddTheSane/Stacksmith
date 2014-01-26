//
//  CStack.cpp
//  Stacksmith
//
//  Created by Uli Kusterer on 2013-12-29.
//  Copyright (c) 2013 Uli Kusterer. All rights reserved.
//

#include "CStack.h"
#include "CCard.h"
#include "CBackground.h"
#include "CURLConnection.h"
#include "tinyxml2.h"
#include "CTinyXMLUtils.h"


using namespace Carlson;


CStack*		CStack::sFrontStack = NULL;


CStack::~CStack()
{
	if( sFrontStack == this )
		sFrontStack = NULL;
	for( auto itty = mBackgrounds.begin(); itty != mBackgrounds.end(); itty++ )
		(*itty)->SetStack( NULL );
	for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
		(*itty)->SetStack( NULL );
}


void	CStack::Load( std::function<void(CStack*)> inCompletionBlock )
{
	if( mLoaded )
	{
		inCompletionBlock( this );
		return;
	}
	
	mLoadCompletionBlocks.push_back( inCompletionBlock );
	
	if( mLoading )	// We'll call you, too, once we have finished loading.
		return;
	
	mLoading = true;
	
	Retain();
	
	CURLRequest		request( mURL );
	//printf("Loading %s\n",mURL.c_str());
	CURLConnection::SendRequestWithCompletionHandler( request, [this,inCompletionBlock] (CURLResponse inResponse, const char* inData, size_t inDataLength) -> void
	{
		tinyxml2::XMLDocument		document;
		
		if( tinyxml2::XML_SUCCESS == document.Parse( inData, inDataLength ) )
		{
			//document.Print();
			
			tinyxml2::XMLElement	*	root = document.RootElement();
			
			mStackID = CTinyXMLUtils::GetLongLongNamed( root, "id" );
			mName = "Untitled";
			CTinyXMLUtils::GetStringNamed( root, "name", mName );
			mUserLevel = CTinyXMLUtils::GetIntNamed( root, "userLevel", 5 );
			mCantModify = CTinyXMLUtils::GetBoolNamed( root, "cantModify", false );
			mCantDelete = CTinyXMLUtils::GetBoolNamed( root, "cantDelete", false );
			mPrivateAccess = CTinyXMLUtils::GetBoolNamed( root, "privateAccess", false );
			mCantAbort = CTinyXMLUtils::GetBoolNamed( root, "cantAbort", false );
			mCantPeek = CTinyXMLUtils::GetBoolNamed( root, "cantPeek", false );
			mResizable = CTinyXMLUtils::GetBoolNamed( root, "resizable", false );
			tinyxml2::XMLElement	*	sizeElem = root->FirstChildElement( "cardSize" );
			mCardWidth = CTinyXMLUtils::GetIntNamed( sizeElem, "width", 512 );
			mCardHeight = CTinyXMLUtils::GetIntNamed( sizeElem, "height", 342 );
			
			mScript.erase();
			CTinyXMLUtils::GetStringNamed( root, "script", mScript );
			
			LoadUserPropertiesFromElement( root );
			
			// Load backgrounds:
			tinyxml2::XMLElement	*	currBgElem = root->FirstChildElement( "background" );
			size_t			slashOffset = mURL.rfind( '/' );
			if( slashOffset == std::string::npos )
				slashOffset = 0;
			
			while( currBgElem )
			{
				std::string		backgroundURL = mURL.substr(0,slashOffset);
				backgroundURL.append( 1, '/' );
				backgroundURL.append( currBgElem->Attribute("file") );
				char*			endPtr = NULL;
				ObjectID	bgID = strtoll( currBgElem->Attribute("id"), &endPtr, 10 );
				const char*		theName = currBgElem->Attribute("name");
				
				CBackground	*	theBackground = new CBackground( backgroundURL, bgID, (theName ? theName : ""), currBgElem->Attribute("file"), this );
				theBackground->Autorelease();
				theBackground->SetStack( this );
				mBackgrounds.push_back( theBackground );
				
				currBgElem = currBgElem->NextSiblingElement( "background" );
			}

			// Load cards:
			tinyxml2::XMLElement	*	currCdElem = root->FirstChildElement( "card" );
			while( currCdElem )
			{
				std::string		cardURL = mURL.substr(0,slashOffset);
				cardURL.append( 1, '/' );
				cardURL.append( currCdElem->Attribute("file") );
				char*			endPtr = NULL;
				ObjectID	cdID = strtoll( currCdElem->Attribute("id"), &endPtr, 10 );
				const char*		theName = currCdElem->Attribute("name");
				const char*	markedAttrStr = currCdElem->Attribute("marked");
				bool	marked = markedAttrStr ? (strcmp("true", markedAttrStr) == 0) : false;
				
				CCard	*	theCard = new CCard( cardURL, cdID, (theName ? theName : ""), currCdElem->Attribute("file"), this, marked );
				theCard->Autorelease();
				mCards.push_back( theCard );
				theCard->SetStack( this );
				if( marked )
					mMarkedCards.insert( theCard );
				
				currCdElem = currCdElem->NextSiblingElement( "card" );
			}
		}
		
		CallAllCompletionBlocks();
		Release();
	} );
}


void	CStack::CallAllCompletionBlocks()
{
	mLoaded = true;
	mLoading = false;
	
	for( auto itty = mLoadCompletionBlocks.begin(); itty != mLoadCompletionBlocks.end(); itty++ )
		(*itty)( this );
	mLoadCompletionBlocks.clear();
}



void	CStack::Save( const std::string& inPackagePath )
{
	if( !mLoaded )
		return;
	
	tinyxml2::XMLDocument		document;
	tinyxml2::XMLDeclaration*	declaration = document.NewDeclaration();
	declaration->SetValue("xml version=\"1.0\" encoding=\"utf-8\"");
	document.InsertEndChild( declaration );
	
	tinyxml2::XMLUnknown*	dtd = document.NewUnknown("DOCTYPE stack PUBLIC \"-//Apple, Inc.//DTD stack V 2.0//EN\" \"\"");
	document.InsertEndChild( dtd );
	
	tinyxml2::XMLElement*		root = document.NewElement("stack");
	document.InsertEndChild( root );
	
	tinyxml2::XMLElement*		nameElem = document.NewElement("name");
	nameElem->SetText(mName.c_str());
	root->InsertEndChild( nameElem );

	tinyxml2::XMLElement*		cardCountElem = document.NewElement("cardCount");
	cardCountElem->SetText((unsigned)mCards.size());
	root->InsertEndChild( cardCountElem );
	
	tinyxml2::XMLElement*		cantModifyElem = document.NewElement("cantModify");
	cantModifyElem->SetBoolFirstChild( mCantModify );
	root->InsertEndChild( cantModifyElem );
	
	tinyxml2::XMLElement*		cantDeleteElem = document.NewElement("cantDelete");
	cantDeleteElem->SetBoolFirstChild( mCantModify );
	root->InsertEndChild( cantDeleteElem );

	tinyxml2::XMLElement*		cantAbortElem = document.NewElement("cantAbort");
	cantAbortElem->SetBoolFirstChild( mCantAbort );
	root->InsertEndChild( cantAbortElem );

	tinyxml2::XMLElement*		cardSizeElem = document.NewElement("cardSize");
	tinyxml2::XMLElement*		cardSizeWidthElem = document.NewElement("width");
	cardSizeWidthElem->SetText( mCardWidth );
	cardSizeElem->InsertEndChild( cardSizeWidthElem );
	tinyxml2::XMLElement*		cardSizeHeightElem = document.NewElement("height");
	cardSizeHeightElem->SetText( mCardHeight );
	cardSizeElem->InsertEndChild( cardSizeHeightElem );
	root->InsertEndChild( cardSizeElem );

	tinyxml2::XMLElement*		scriptElem = document.NewElement("script");
	scriptElem->SetText( mScript.c_str() );
	root->InsertEndChild( scriptElem );
	
	SaveUserPropertiesToElementOfDocument( root, &document );
	
	for( auto currBackground : mBackgrounds )
	{
		tinyxml2::XMLElement*		bgElem = document.NewElement("background");
		bgElem->SetAttribute( "id", currBackground->GetID() );
		bgElem->SetAttribute( "file", currBackground->GetFileName().c_str() );
		bgElem->SetAttribute( "name", currBackground->GetName().c_str() );
		root->InsertEndChild( bgElem );
		
		currBackground->Save( inPackagePath );
	}

	for( auto currCard : mCards )
	{
		tinyxml2::XMLElement*		cdElem = document.NewElement("card");
		cdElem->SetAttribute( "id", currCard->GetID() );
		cdElem->SetAttribute( "file", currCard->GetFileName().c_str() );
		cdElem->SetAttribute( "name", currCard->GetName().c_str() );
		root->InsertEndChild( cdElem );
		
		currCard->Save( inPackagePath );
	}

	std::string	stackFilePath(inPackagePath);
	stackFilePath.append(mFileName);
	document.SaveFile( stackFilePath.c_str() );
}


void	CStack::AddCard( CCard* inCard )
{
	inCard->Retain();
	inCard->SetStack( this );
	mCards.push_back( inCard );
	
	if( inCard->IsMarked() )
		mMarkedCards.insert( inCard );
}

void	CStack::RemoveCard( CCard* inCard )
{
	if( inCard->IsMarked() )
		mMarkedCards.erase( inCard );
	
	for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
	{
		if( (*itty) == inCard )
		{
			mCards.erase( itty );
			break;
		}
	}
	inCard->SetStack( NULL );
	inCard->Release();
}


CCard*	CStack::GetCardByID( ObjectID inID )
{
	for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
	{
		if( (*itty)->GetID() == inID )
			return *itty;
	}
	
	return NULL;
}


CCard*	CStack::GetCardWithBackground( CBackground* inBg, CCard *startAtCard, bool searchForward )
{
	bool		hadStartCard = (startAtCard == NULL);
	if( searchForward )
	{
		for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
		{
			if( *itty == startAtCard )
			{
				hadStartCard = true;
				continue;
			}
			if( !hadStartCard )
				continue;
			if( (*itty)->GetBackground() == inBg )
				return *itty;
		}
		
		for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
		{
			if( (*itty)->GetBackground() == inBg )
				return *itty;
		}
	}
	else
	{
		for( auto itty = mCards.rbegin(); itty != mCards.rend(); itty++ )
		{
			if( *itty == startAtCard )
			{
				hadStartCard = true;
				continue;
			}
			if( !hadStartCard )
				continue;
			if( (*itty)->GetBackground() == inBg )
				return *itty;
		}
		
		for( auto itty = mCards.rbegin(); itty != mCards.rend(); itty++ )
		{
			if( (*itty)->GetBackground() == inBg )
				return *itty;
		}
	}
	
	return NULL;
}


CBackground*	CStack::GetBackgroundByID( ObjectID inID )
{
	for( auto itty = mBackgrounds.begin(); itty != mBackgrounds.end(); itty++ )
	{
		if( (*itty)->GetID() == inID )
			return *itty;
	}
	
	return NULL;
}


CCard*	CStack::GetCardByName( const char* inName )
{
	for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
	{
		if( strcasecmp( (*itty)->GetName().c_str(), inName ) == 0 )
			return *itty;
	}
	
	return NULL;
}


CBackground*	CStack::GetBackgroundByName( const char* inName )
{
	for( auto itty = mBackgrounds.begin(); itty != mBackgrounds.end(); itty++ )
	{
		if( strcasecmp( (*itty)->GetName().c_str(), inName ) == 0 )
			return *itty;
	}
	
	return NULL;
}


size_t	CStack::GetIndexOfCard( CCard* inCard )
{
	size_t		currIdx = 0;
	for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
	{
		if( (*itty) == inCard )
			return currIdx;
		currIdx++;
	}
	return SIZE_T_MAX;
}


size_t	CStack::GetIndexOfBackground( CBackground* inBackground )
{
	size_t		currIdx = 0;
	for( auto itty = mBackgrounds.begin(); itty != mBackgrounds.end(); itty++ )
	{
		if( (*itty) == inBackground )
			return currIdx;
		currIdx++;
	}
	return SIZE_T_MAX;
}


void	CStack::SetPeeking( bool inState )
{
	mPeeking = inState;
	CCard	*	theCard = GetCurrentCard();
	if( theCard )
		theCard->SetPeeking( inState );
}


void	CStack::Dump( size_t inIndent )
{
	const char * indentStr = IndentString( inIndent );
	printf( "%sStack ID %lld \"%s\" <%p>\n%s{\n", indentStr, mStackID, mName.c_str(), this, indentStr );
	printf( "%s\tloaded = %s\n", indentStr, (mLoaded? "true" : "false") );
	printf( "%s\tuserLevel = %d\n", indentStr, mUserLevel );
	printf( "%s\twidth = %d\n", indentStr, mCardWidth );
	printf( "%s\theight = %d\n", indentStr, mCardHeight );
	printf( "%s\tcantPeek = %s\n", indentStr, (mCantPeek? "true" : "false") );
	printf( "%s\tcantAbort = %s\n", indentStr, (mCantAbort? "true" : "false") );
	printf( "%s\tprivateAccess = %s\n", indentStr, (mPrivateAccess? "true" : "false") );
	printf( "%s\tcantDelete = %s\n", indentStr, (mCantDelete? "true" : "false") );
	printf( "%s\tcantModify = %s\n", indentStr, (mCantModify? "true" : "false") );
	printf( "%s\tresizable = %s\n", indentStr, (mResizable? "true" : "false") );
	printf( "%s\tscript = <<%s>>\n", indentStr, mScript.c_str() );
	DumpUserProperties( inIndent +1 );
	printf( "%s\tcards\n%s\t{\n", indentStr, indentStr );
	for( auto itty = mCards.begin(); itty != mCards.end(); itty++ )
		(*itty)->Dump( inIndent +2 );
	printf( "%s\t}\n%s\tbackgrounds\n%s\t{\n", indentStr, indentStr, indentStr );
	for( auto itty = mBackgrounds.begin(); itty != mBackgrounds.end(); itty++ )
		(*itty)->Dump( inIndent +2 );
	printf( "%s\t}\n%s\tmarkedCards\n%s\t{\n", indentStr, indentStr, indentStr );
	for( auto itty = mMarkedCards.begin(); itty != mMarkedCards.end(); itty++ )
	{
		CCardRef	theCard = (*itty);
		theCard->Dump( inIndent +2 );
	}
	printf( "%s\t}\n%s}\n", indentStr, indentStr );
}

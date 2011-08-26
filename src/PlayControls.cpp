/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 Bloom Studio, Inc. All rights reserved.
 *
 */

#include <boost/foreach.hpp>
#include "PlayControls.h"
#include "Globals.h"
#include "BloomGl.h" // for batch drawing
#include "BloomScene.h" // for mRoot

using namespace ci;
using namespace ci::app;
using namespace std;

void PlayControls::setup( Vec2f interfaceSize, ipod::Player *player, const Font &font, const Font &fontSmall, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex )
{   

    // create, add, and position everything...
    createChildren( font, fontSmall, uiBigButtonsTex, uiSmallButtonsTex );
    setInterfaceSize( interfaceSize );
        
    // set initial state...
    setPlayingOn( player->getPlayState() == ipod::Player::StatePlaying );    
    setAlphaOn( false ); // this is the default in WheelOverlay::setup()
    setPlaylistOn( false );
    setShowSettingsOn( G_SHOW_SETTINGS );
}

void PlayControls::createChildren( const Font &font, const Font &fontSmall, const gl::Texture &uiBigButtonsTex, const gl::Texture &uiSmallButtonsTex )
{
	float uw = uiBigButtonsTex.getWidth() / 4.0f;   // button tex width
	float uh = uiBigButtonsTex.getHeight() / 4.0f;	// button tex height

    mGalaxyButton = new SimpleButton( GOTO_GALAXY,   // ID
                                      uiBigButtonsTex,
                                      Area( uw*0, uh*1, uw*1, uh*2 ),  // on texture
                                      Area( uw*0, uh*0, uw*1, uh*1 ) );// off texture
    
    mCurrentTrackButton = new SimpleButton( GOTO_CURRENT_TRACK,   // ID
                                            uiBigButtonsTex,
                                            Area( uw*1, uh*1, uw*2, uh*2 ),  // on texture
                                            Area( uw*1, uh*0, uw*2, uh*1 ) );// off texture

	mShowSettingsButton = new ToggleButton( SETTINGS, 
                                            false, 
                                            uiBigButtonsTex,
                                            Area( uw*2, uh*1, uw*3, uh*2 ),  // on texture
                                            Area( uw*2, uh*0, uw*3, uh*1 ) );// off texture   

    mPreviousTrackButton = new SimpleButton( PREV_TRACK, 
                                             uiBigButtonsTex,
                                             Area( uw*3, uh*1, uw*4, uh*2 ),  // on texture
                                             Area( uw*3, uh*0, uw*4, uh*1 ) );// off texture
	
    mPlayPauseButton = new TwoStateButton( PLAY_PAUSE, 
                                           false, // initial state is updated in setup()
                                           uiBigButtonsTex,
                                           Area( uw*0, uh*3, uw*1, uh*4 ),  // offDown   
                                           Area( uw*0, uh*2, uw*1, uh*3 ),  // offUp
                                           Area( uw*1, uh*3, uw*2, uh*4 ),  // onDown
                                           Area( uw*1, uh*2, uw*2, uh*3 ) );// onUp

    mNextTrackButton = new SimpleButton( NEXT_TRACK, 
                                         uiBigButtonsTex,
                                         Area( uw*2, uh*3, uw*3, uh*4 ),  // on texture
                                         Area( uw*2, uh*2, uw*3, uh*3 ) );// off texture
	
    mAlphaButton = new ToggleButton( SHOW_ALPHA_FILTER,      // ID
                                     false,           // initial toggle state
                                     uiBigButtonsTex,
                                     Area( uw*3, uh*3, uw*4, uh*4 ),  // on texture
                                     Area( uw*3, uh*2, uw*4, uh*3 ) );// off texture    

    // FIXME: need a new texture for playlist button :(
    mPlaylistButton = new ToggleButton( SHOW_PLAYLIST_FILTER,      // ID
                                        false,           // initial toggle state
                                        uiBigButtonsTex,
                                        Area( uw*3, uh*3, uw*4, uh*4 ),  // on texture
                                        Area( uw*3, uh*2, uw*4, uh*3 ) );// off texture         

    // !!! SMALL BUTTONS !!!
    uw = uiSmallButtonsTex.getWidth() / 5.0f;
	uh = uiSmallButtonsTex.getHeight() / 5.0f;
    
    mPlayheadSlider = new Slider( SLIDER,          // ID
                                  uiSmallButtonsTex,
								 Area( uw*2.2f, uh*3, uw*2.3f, uh*4 ),  // bg texture
								 Area( uw*2.7f, uh*3, uw*2.8f, uh*4 ),  // fg texture
                                  Area( uw*3, uh*3, uw*4, uh*4 ),  // thumb on texture
                                  Area( uw*3, uh*2, uw*4, uh*3 )); // thumb off texture

    /////// no textures please, we're British...
    
    mTrackInfoLabel = new ScrollingLabel(NO_BUTTON, font, BRIGHT_BLUE);
    
    mElapsedTimeLabel = new TimeLabel(NO_BUTTON, fontSmall, BRIGHT_BLUE);
    
    mRemainingTimeLabel = new TimeLabel(NO_BUTTON, fontSmall, BRIGHT_BLUE);
    
    //////// little fady bits to cover the edges of scrolling bits:
    Area aLeft = Area( 0, 160, 14, 170 ); // references the uiButtons image    
    mCoverLeftTextureRect = new TextureRect( uiSmallButtonsTex, aLeft );
    // NB:- when rect is set for the right side, x1 > x2 so it is flipped
    mCoverRightTextureRect = new TextureRect( uiSmallButtonsTex, aLeft );    
}    

bool PlayControls::addedToScene()
{
    // now mRoot is valid we can add children
    addChildren(); // FIXME: make it so you can add children even if mRoot is invalid
    // add listeners to relay callbacks...
    std::cout << "registering callbacks in PlayControls" << std::endl;
    mCbTouchMoved = getRoot()->registerBloomNodeTouchMoved( this, &PlayControls::onBloomNodeTouchMoved );
    mCbTouchEnded = getRoot()->registerBloomNodeTouchEnded( this, &PlayControls::onBloomNodeTouchEnded );    
    return false;
}

bool PlayControls::removedFromScene()
{
    // remove listeners...
    // FIXME: this should also be done in destructor (?)
    getRoot()->unregisterBloomNodeTouchMoved( mCbTouchMoved );
    getRoot()->unregisterBloomNodeTouchEnded( mCbTouchEnded );    
    return false;
}

void PlayControls::addChildren()
{
    // bit of hack, this is first for batch reasons
    // (we want the little fadey bits to be drawn on top at the end)
    addChild( BloomNodeRef(mTrackInfoLabel) );

	addChild( BloomNodeRef(mPlayheadSlider) );    
    addChild( BloomNodeRef(mElapsedTimeLabel) );
    addChild( BloomNodeRef(mRemainingTimeLabel) ); 
    
    addChild( BloomNodeRef(mGalaxyButton) );
	addChild( BloomNodeRef(mCurrentTrackButton) );
    addChild( BloomNodeRef(mShowSettingsButton) );
	addChild( BloomNodeRef(mAlphaButton) );
	addChild( BloomNodeRef(mPlaylistButton) );
    addChild( BloomNodeRef(mPreviousTrackButton) );
    addChild( BloomNodeRef(mPlayPauseButton) );
    addChild( BloomNodeRef(mNextTrackButton) );	
    
    // shaded bits on top of scrolling mTrackInfoLabel
    addChild( BloomNodeRef(mCoverLeftTextureRect) );
    addChild( BloomNodeRef(mCoverRightTextureRect) );    
}


bool PlayControls::onBloomNodeTouchMoved( BloomNodeRef nodeRef )
{
    if ( nodeRef->getId() == mPlayheadSlider->getId() ) {
        mCallbacksPlayheadMoved.call( mPlayheadSlider->getValue() );
    }
    return false;
}

bool PlayControls::onBloomNodeTouchEnded( BloomNodeRef nodeRef )
{
    if ( nodeRef->getId() == mPlayheadSlider->getId() ) {
        mCallbacksPlayheadMoved.call( mPlayheadSlider->getValue() );
    }
    else if ( nodeRef->getId() > NO_BUTTON && nodeRef->getId() < LAST_BUTTON ) {
        mCallbacksButtonPressed.call(ButtonId(nodeRef->getId()));
    }
    return false;
}

void PlayControls::setInterfaceSize( Vec2f interfaceSize )
{
    mInterfaceSize = interfaceSize;
    
	const float topBorder	 = 5.0f;
	const float sideBorder	 = 10.0f;
    
    const float bSize		 = 50.0f;
	const float buttonGap	 = 1.0f;

	const float timeTexWidth = 60.0f;
    const float sliderHeight = 20.0f;
    const float sliderInset  = bSize * 2.0f + sideBorder + timeTexWidth;
    const bool  landscape    = interfaceSize.x > interfaceSize.y;
    const float sliderWidth  = landscape ? 328.0f : 201.0f;
    
    // FLY TO CURRENT TRACK-MOON BUTTON
	float y1 = topBorder;
	float y2 = y1 + bSize;
	float x1 = sideBorder;
	float x2 = x1 + bSize;        
    mGalaxyButton->setRect( x1, y1, x2, y2 );
    
	x1 += bSize + buttonGap;
	x2 = x1 + bSize;
    mCurrentTrackButton->setRect( x1, y1, x2, y2 );

    // NEXT / PLAY-PAUSE / PREVIOUS TRACK BUTTONS
    x1 = interfaceSize.x - sideBorder - bSize;
	x2 = x1 + bSize;
    mNextTrackButton->setRect( x1, y1, x2, y2 );

    x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mPlayPauseButton->setRect( x1, y1, x2, y2 );
    
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;    
    mPreviousTrackButton->setRect( x1, y1, x2, y2 );

    // SETTINGS BUTTON
	x1 -= ( bSize + buttonGap ) * 1.333f;
	x2 = x1 + bSize;    
    mShowSettingsButton->setRect( x1, y1, x2, y2 );

	// ALPHA BUTTON
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mAlphaButton->setRect( x1, y1, x2, y2 );

    // PLAYLIST BUTTON
	x1 -= bSize + buttonGap;
	x2 = x1 + bSize;
    mPlaylistButton->setRect( x1, y1, x2, y2 );	
	
    const float bgx1 = sliderInset;
    const float bgx2 = bgx1 + sliderWidth;
    const float bgy1 = 32.0f;
    const float bgy2 = bgy1 + sliderHeight;
    mPlayheadSlider->setRect( bgx1, bgy1, bgx2, bgy2 );
	
    const float ctx1 = bgx1 - 43.0f;
    const float ctx2 = bgx2 + 48.0f;
    const float cty1 = bgy1 - 16.0f;
    const float cty2 = cty1 + mTrackInfoLabel->getFontHeight();
    mTrackInfoLabel->setRect( ctx1, cty1, ctx2, cty2 );

    // FIXME: bottom right coords are made up... maybe just setPos (and getWidth) for these?
    // at least use font height for calculating y2
//    mElapsedTimeLabel->setRect( bgx1-40.0f, bgy1+2.0f, bgx1, bgy1+12.0f );
	
	mElapsedTimeLabel->setRect( ctx1 + 3.0f, bgy1+2.0f, bgx1, bgy1+12.0f );
    mRemainingTimeLabel->setRect( bgx2+18.0f, bgy1+2.0f, bgx2+58.0f, bgy1+12.0f );
}

void PlayControls::update()
{
    Vec2f interfaceSize = getRoot()->getInterfaceSize();
    if ( mInterfaceSize != interfaceSize ) {
        setInterfaceSize( interfaceSize );
    }    

    const float w	 = 15.0f;
	Rectf infoRect   = mTrackInfoLabel->getRect();
	
	if( mTrackInfoLabel->isScrollingText() ) {
        mCoverLeftTextureRect->setRect( infoRect.x1, infoRect.y1, infoRect.x1 + w, infoRect.y2 );
    } else {
        mCoverLeftTextureRect->setRect( infoRect.x1, infoRect.y1, infoRect.x1, infoRect.y1 ); // zero size (FIXME: visible true/false? or remove from scene?
    }
    mCoverRightTextureRect->setRect( infoRect.x2 + 1.0f, infoRect.y1, infoRect.x2 - ( w - 1.0f ), infoRect.y2 );
    
}

void PlayControls::enablePlayerControls( bool enable )
{
    // FIXME: perhaps grayed out or dimmed would be more subtle?
    mPreviousTrackButton->setVisible(enable);
    mPlayPauseButton->setVisible(enable);
    mNextTrackButton->setVisible(enable);    
}

void PlayControls::deepDraw()
{
    if (mVisible) {
        glPushMatrix();
        glMultMatrixf(mTransform); // FIXME only push/mult/pop if mTransform isn't identity
        bloom::gl::beginBatch();
        // draw children
        BOOST_FOREACH(BloomNodeRef child, mChildren) {        
            child->deepDraw();
        }
        bloom::gl::endBatch();
        glPopMatrix();
    }        
}

float PlayControls::getHeight()
{
    return 60.0f;
}

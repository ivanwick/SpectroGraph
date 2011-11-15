//
// File:       iTunesPlugIn.cpp
//
// Abstract:   Visual plug-in for iTunes.  Cross-platform code.
//
// Version:    2.0
//
// Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple Inc. ( "Apple" )
//             in consideration of your agreement to the following terms, and your use,
//             installation, modification or redistribution of this Apple software
//             constitutes acceptance of these terms.  If you do not agree with these
//             terms, please do not use, install, modify or redistribute this Apple
//             software.
//
//             In consideration of your agreement to abide by the following terms, and
//             subject to these terms, Apple grants you a personal, non - exclusive
//             license, under Apple's copyrights in this original Apple software ( the
//             "Apple Software" ), to use, reproduce, modify and redistribute the Apple
//             Software, with or without modifications, in source and / or binary forms;
//             provided that if you redistribute the Apple Software in its entirety and
//             without modifications, you must retain this notice and the following text
//             and disclaimers in all such redistributions of the Apple Software. Neither
//             the name, trademarks, service marks or logos of Apple Inc. may be used to
//             endorse or promote products derived from the Apple Software without specific
//             prior written permission from Apple.  Except as expressly stated in this
//             notice, no other rights or licenses, express or implied, are granted by
//             Apple herein, including but not limited to any patent rights that may be
//             infringed by your derivative works or by other works in which the Apple
//             Software may be incorporated.
//
//             The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
//             WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
//             WARRANTIES OF NON - INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
//             PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND OPERATION
//             ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
//
//             IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
//             CONSEQUENTIAL DAMAGES ( INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//             SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//             INTERRUPTION ) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
//             AND / OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED AND WHETHER
//             UNDER THEORY OF CONTRACT, TORT ( INCLUDING NEGLIGENCE ), STRICT LIABILITY OR
//             OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright © 2001-2011 Apple Inc. All Rights Reserved.
//

//-------------------------------------------------------------------------------------------------
//	includes
//-------------------------------------------------------------------------------------------------

#include "Spactrograph.h"

#include <string.h>

//-------------------------------------------------------------------------------------------------
// ProcessRenderData
//-------------------------------------------------------------------------------------------------
//
void ProcessRenderData( VisualPluginData * visualPluginData, UInt32 timeStampID, const RenderVisualData * renderData )
{
	SInt16		index;
	SInt32		channel;

	visualPluginData->renderTimeStampID	= timeStampID;

	if ( renderData == NULL )
	{
		memset( &visualPluginData->renderData, 0, sizeof(visualPluginData->renderData) );
		return;
	}

	visualPluginData->renderData = *renderData;
	
	for ( channel = 0;channel < renderData->numSpectrumChannels; channel++ )
	{
		visualPluginData->minLevel[channel] = 
			visualPluginData->maxLevel[channel] = 
			renderData->spectrumData[channel][0];

		for ( index = 1; index < kVisualNumSpectrumEntries; index++ )
		{
			UInt8		value;
			
			value = renderData->spectrumData[channel][index];

			if ( value < visualPluginData->minLevel[channel] )
				visualPluginData->minLevel[channel] = value;
			else if ( value > visualPluginData->maxLevel[channel] )
				visualPluginData->maxLevel[channel] = value;
		}
	}

	
	/* Anti-banding: we assume there is no frequency content in the highest
	 * frequency possible (which should be the case for all normal music).
	 * So if there is something there, we subtract it from all frequencies. */
#if 0 // ivan- gBandFlag is supposed to be a param var, deal with this later.
	if(gBandFlag) {
		UInt8 *spectrumDataL = visualPluginDataPtr->renderData.spectrumData[0],
        *spectrumDataR = visualPluginDataPtr->renderData.spectrumData[1];
		SInt16 i;
		SInt16 biasL = spectrumDataL[kVisualNumSpectrumEntries/2-1],
        biasR = spectrumDataR[kVisualNumSpectrumEntries/2-1];
		for( i=0; i<kVisualNumSpectrumEntries/2; i++ ) {
			spectrumDataL[i] -= (spectrumDataL[i]-biasL > 0) ? biasL : spectrumDataL[i];
			spectrumDataR[i] -= (spectrumDataR[i]-biasR > 0) ? biasR : spectrumDataR[i];
		}
	}		
#endif
    
	/* This just finds the min & max values of the spectrum data, if
	 * there's no need for this, you can drop this to save some CPU */
    /*	for (channel = 0;channel < renderData->numSpectrumChannels;channel++)
     {
     visualPluginDataPtr->minLevel[channel] = 
     visualPluginDataPtr->maxLevel[channel] = 
     renderData->spectrumData[channel][0];
     
     for (index = 1; index < kVisualNumSpectrumEntries; index++)
     {
     UInt8		value;
     
     value = renderData->spectrumData[channel][index];
     
     if (value < visualPluginDataPtr->minLevel[channel])
     visualPluginDataPtr->minLevel[channel] = value;
     else if (value > visualPluginDataPtr->maxLevel[channel])
     visualPluginDataPtr->maxLevel[channel] = value;
     }
     }*/
}


//-------------------------------------------------------------------------------------------------
//	ResetRenderData
//-------------------------------------------------------------------------------------------------
//
void ResetRenderData( VisualPluginData * visualPluginData )
{
	memset( &visualPluginData->renderData, 0, sizeof(visualPluginData->renderData) );

	// ivan- the following line and then all the rest do the same thing w/e
    memset( visualPluginData->minLevel, 0, sizeof(visualPluginData->minLevel) );
    
	visualPluginData->minLevel[0] = 
    visualPluginData->minLevel[1] =
    visualPluginData->maxLevel[0] =
    visualPluginData->maxLevel[1] = 0;
}


//-------------------------------------------------------------------------------------------------
//	UpdateInfoTimeOut
//-------------------------------------------------------------------------------------------------
//
void UpdateInfoTimeOut( VisualPluginData * visualPluginData )
{
	// reset the timeout value we will use to show the info/artwork if we have it during DrawVisual()
	visualPluginData->drawInfoTimeOut = time( NULL ) + kInfoTimeOutInSeconds;
}

//-------------------------------------------------------------------------------------------------
//	UpdatePulseRate
//-------------------------------------------------------------------------------------------------
//
void UpdatePulseRate( VisualPluginData * visualPluginData, UInt32 * ioPulseRate )
{
	// vary the pulse rate based on whether or not iTunes is currently playing
	if ( visualPluginData->playing )
		*ioPulseRate = kPlayingPulseRateInHz;
	else
		*ioPulseRate = kStoppedPulseRateInHz;
}

//-------------------------------------------------------------------------------------------------
//	UpdateTrackInfo
//-------------------------------------------------------------------------------------------------
//
void UpdateTrackInfo( VisualPluginData * visualPluginData, ITTrackInfo * trackInfo, ITStreamInfo * streamInfo )
{
	if ( trackInfo != NULL )
		visualPluginData->trackInfo = *trackInfo;
	else
		memset( &visualPluginData->trackInfo, 0, sizeof(visualPluginData->trackInfo) );

	if ( streamInfo != NULL )
		visualPluginData->streamInfo = *streamInfo;
	else
		memset( &visualPluginData->streamInfo, 0, sizeof(visualPluginData->streamInfo) );

	UpdateInfoTimeOut( visualPluginData );
}

//-------------------------------------------------------------------------------------------------
//	RequestArtwork
//-------------------------------------------------------------------------------------------------
//
static void RequestArtwork( VisualPluginData * visualPluginData )
{
	// only request artwork if this plugin is active
	if ( visualPluginData->destView != NULL )
	{
		OSStatus		status;

		status = PlayerRequestCurrentTrackCoverArt( visualPluginData->appCookie, visualPluginData->appProc );
	}
}

//-------------------------------------------------------------------------------------------------
//	PulseVisual
//-------------------------------------------------------------------------------------------------
//
void PulseVisual( VisualPluginData * visualPluginData, UInt32 timeStampID, const RenderVisualData * renderData, UInt32 * ioPulseRate )
{
	// update internal state
	ProcessRenderData( visualPluginData, timeStampID, renderData );

	// if desired, adjust the pulse rate
	UpdatePulseRate( visualPluginData, ioPulseRate );
}

//-------------------------------------------------------------------------------------------------
//	VisualPluginHandler
//-------------------------------------------------------------------------------------------------
//
static OSStatus VisualPluginHandler(OSType message,VisualPluginMessageInfo *messageInfo,void *refCon)
{
	OSStatus			status;
	VisualPluginData *	visualPluginData;
    
	visualPluginData = (VisualPluginData*) refCon;
	
	status = noErr;
    
	switch (message)
	{
            /*
             * Apple says:
             Sent when the visual plugin is registered.  The plugin should do minimal
             memory allocations here.  The resource fork of the plugin is still available.
             * I say:
             iTunes will 'register' each plugin when iTunes is started. Even though your plug-in
             will not necessarily be shown on this occasion, you can still do some initializations.
             However, don't do anything that will hog memory or take ages.
             */		
		case kVisualPluginInitMessage:
        {
			visualPluginData = (VisualPluginData*) calloc(1, sizeof(VisualPluginData));
			if (visualPluginData == NULL)
			{
				status = memFullErr;
				break;
			}
            
			visualPluginData->appCookie	= messageInfo->u.initMessage.appCookie;
			visualPluginData->appProc	= messageInfo->u.initMessage.appProc;
            
			messageInfo->u.initMessage.refCon	= (void*) visualPluginData;
            #ifdef SG_DEBUG
                fprintf(stderr, "SpectroGraph inited\n");
            #endif
			break;
		}	
            /*
             Sent when the visual plugin is unloaded
             */		
		case kVisualPluginCleanupMessage:
        {
            #ifdef SG_DEBUG
                fprintf(stderr, "Unloading SpectroGraph...\n");
            #endif
			if (visualPluginData != NULL)
				free(visualPluginData);
			break;
		}
            
            /*
             Sent when the visual plugin is enabled.  iTunes currently enables all
             loaded visual plugins.  The plugin should not do anything here.
             */
		case kVisualPluginEnableMessage:
		case kVisualPluginDisableMessage:
        {
			break;
        }
            
            /*
             Sent if the plugin requests idle messages.  Do this by setting the kVisualWantsIdleMessages
             option in the RegisterVisualMessage.options field.
             */
		case kVisualPluginIdleMessage:
        {
			/* This is where it gets nasty. Idle messages can be sent at any time: while iTunes is playing
			 * (frequently), when paused (constantly), and even when the visualizer is off (a few times
			 * per second). Moreover, _all_ plug-ins receive idle messages even if another one is active.
			 * Because I used 0xFFFFFFFF for timeBetweenData, iTunes will use 100% cpu, both during playback
			 * and while paused. This is why I included the usleep calls. Mind that usleep will pause the
			 * _entire_ iTunes process, so a sensible value must be used (1msec doesn't seem to interfere
			 * with normal operation).
			 * I need to check if iTunes is paused to avoid messing up the timing of the rendering routine,
			 * and check if the plug-in is active to avoid rendering a non-existing port. */
            
            #if 0 // ivan
			if( visualPluginData->playing == false && visualPluginData->destPort != nil ) {
				if( getuSec(gLineTimeStamp) > gDelay ) {
					startuSec(&gLineTimeStamp);
					RenderVisualPort(visualPluginData,visualPluginData->destPort,&visualPluginData->destRect,false);
				}
				else
					usleep(SG_USLEEP); // TODO: find Windows equivalent
			}
            #endif
			break;
		}
            
            /*
             Sent if the plugin requests the ability for the user to configure it.  Do this by setting
             the kVisualWantsConfigure option in the RegisterVisualMessage.options field.
             */
		case kVisualPluginConfigureMessage:
        {
			status = ConfigureVisual( visualPluginData );
			break;
		}

        /*
             Sent when iTunes is going to show the visual plugin.  At this
             point, the plugin should allocate any large buffers it needs.
        */
        /* this is mostly platform-specific so the original SpectroGraph code
           got moved into the SpactrographMac.mm file.
        */
		case kVisualPluginActivateMessage:
        {
			status = ActivateVisual( visualPluginData, messageInfo->u.activateMessage.view, messageInfo->u.activateMessage.options );
            
			// note: do not draw here if you can avoid it, a draw message will be sent as soon as possible
			
			if ( status == noErr )
				RequestArtwork( visualPluginData );
			break;
		}	

        /*
            Sent when this visual is no longer displayed.
        */
        /* ivan - copied from example code. the original was platform-specific
               and went into the SpactrographMac.mm file.
        */
		case kVisualPluginDeactivateMessage:
		{
			UpdateTrackInfo( visualPluginData, NULL, NULL );
            
			status = DeactivateVisual( visualPluginData );
			break;
		}
            

            
            
            
#if 1 // ivan - copied from new code. both these are roughly analogous to the
            // old kVisualPluginSetWindowMessage
        /*
            Sent when iTunes is moving the destination view to a new parent window (e.g. to/from fullscreen).
        */
		case kVisualPluginWindowChangedMessage:
		{
			status = MoveVisual( visualPluginData, messageInfo->u.windowChangedMessage.options );
			break;
		}
        /*
             Sent when iTunes has changed the rectangle of the currently displayed visual.
             
             Note: for custom NSView subviews, the subview's frame is automatically resized.
        */
		case kVisualPluginFrameChangedMessage:
		{
			status = ResizeVisual( visualPluginData );
			break;
		}


#else // ivan - old SpectroGraph code
        /*
             Sent when iTunes needs to change the port or rectangle of the currently
             displayed visual.
        */
		case kVisualPluginSetWindowMessage:
        {
			visualPluginData->destOptions = messageInfo->u.setWindowMessage.options;
            
			status = ChangeVisualPort(	visualPluginData,
                                    #if TARGET_OS_WIN32
                                      messageInfo->u.showWindowMessage.window,
                                    #endif
                                    #if TARGET_OS_MAC
                                      messageInfo->u.showWindowMessage.port,
                                    #endif
                                      &messageInfo->u.setWindowMessage.drawRect);
            
			if (status == noErr)
				RenderVisualPort(visualPluginData,visualPluginData->destPort,&visualPluginData->destRect,true);
			break;
        }
#endif // ivan - old SpectroGraph code
            
        /*
             It's time for the plugin to draw a new frame.
             
             For plugins using custom subviews, you should ignore this message and just
             draw in your view's draw method.  It will never be called if your subview 
             is set up properly.
        */
        /* We're using a subview on MacOS but in Windows, you'll need to call DrawVisual from here. */
		case kVisualPluginDrawMessage:
		{
            #if !USE_SUBVIEW
			DrawVisual( visualPluginData );
            #endif
			break;
		}

#if 1 // ivan - copied from new code.
        /*
             Sent for the visual plugin to update its internal animation state.
             Plugins are allowed to draw at this time but it is more efficient if they
             wait until the kVisualPluginDrawMessage is sent OR they simply invalidate
             their own subview.  The pulse message can be sent faster than the system
             will allow drawing to support spectral analysis-type plugins but drawing
             will be limited to the system refresh rate.
             */
		case kVisualPluginPulseMessage:
		{
			PulseVisual( visualPluginData,
                        messageInfo->u.pulseMessage.timeStampID,
                        messageInfo->u.pulseMessage.renderData,
                        &messageInfo->u.pulseMessage.newPulseRateInHz );
            
			InvalidateVisual( visualPluginData );
			break;
		}
#else // ivan - old SpectroGraph code
        /*
             Sent in response to an update event.  The visual plugin should update
             into its remembered port.  This will only be sent if the plugin has been
             previously given a ShowWindow message.
        */	
		case kVisualPluginUpdateMessage:
			RenderVisualPort(visualPluginData,visualPluginData->destPort,&visualPluginData->destRect,true);
			break;
#endif // ivan - old SpectroGraph code


            
            
            
            
            
            
            /*
             Sent when the player starts.
             */
		case kVisualPluginPlayMessage:
#if 1 // ivan - new example code
		{
			visualPluginData->playing = true;
			
			UpdateTrackInfo( visualPluginData, messageInfo->u.playMessage.trackInfo, messageInfo->u.playMessage.streamInfo );
            
			RequestArtwork( visualPluginData );
			
			InvalidateVisual( visualPluginData );
			break;
		}            
#else // ivan - old SpectroGraph code
			if (messageInfo->u.playMessage.trackInfo != nil)
				visualPluginData->trackInfo = *messageInfo->u.playMessage.trackInfoUnicode;
			else
				MyMemClear(&visualPluginData->trackInfo,sizeof(visualPluginData->trackInfo));
            
			if (messageInfo->u.playMessage.streamInfo != nil)
				visualPluginData->streamInfo = *messageInfo->u.playMessage.streamInfoUnicode;
			else
				MyMemClear(&visualPluginData->streamInfo,sizeof(visualPluginData->streamInfo));
            
			visualPluginData->playing = true;
			break;
#endif // ivan - old SpectroGraph code
            
            /*
             Sent when the player changes the current track information.  This
             is used when the information about a track changes,or when the CD
             moves onto the next track.  The visual plugin should update any displayed
             information about the currently playing song.
             */
		case kVisualPluginChangeTrackMessage:
#if 1 // ivan - new example code
        {
			UpdateTrackInfo( visualPluginData, messageInfo->u.changeTrackMessage.trackInfo, messageInfo->u.changeTrackMessage.streamInfo );
            
			RequestArtwork( visualPluginData );
            
			InvalidateVisual( visualPluginData );
			break;
		}

#else // ivan - old SpectroGraph code
			if (messageInfo->u.changeTrackMessage.trackInfo != nil)
				visualPluginData->trackInfo = *messageInfo->u.changeTrackMessage.trackInfoUnicode;
			else
				MyMemClear(&visualPluginData->trackInfo,sizeof(visualPluginData->trackInfo));
            
			if (messageInfo->u.changeTrackMessage.streamInfo != nil)
				visualPluginData->streamInfo = *messageInfo->u.changeTrackMessage.streamInfoUnicode;
			else
				MyMemClear(&visualPluginData->streamInfo,sizeof(visualPluginData->streamInfo));
			break;
#endif // ivan - old SpectroGraph code
            /*
             Sent when the player stops.
             */
		case kVisualPluginStopMessage:
#if 1 // ivan - new example code
        {
			visualPluginData->playing = false;
			
			ResetRenderData( visualPluginData );
            
			InvalidateVisual( visualPluginData );
			break;
		}

#else // ivan - old SpectroGraph code
			visualPluginData->playing = false;
			
			ResetRenderData(visualPluginData);
            
			RenderVisualPort(visualPluginData,visualPluginData->destPort,&visualPluginData->destRect,true);
			break;
#endif // ivan - old SpectroGraph code
            
            /*
             Sent when the player changes position.
             */
		case kVisualPluginSetPositionMessage:
        {
            break;
        }

		default:
			status = unimpErr;
			break;
	}
	return status;
}



//-------------------------------------------------------------------------------------------------
//	RegisterVisualPlugin
//-------------------------------------------------------------------------------------------------
//
OSStatus RegisterVisualPlugin( PluginMessageInfo * messageInfo )
{
	PlayerMessageInfo	playerMessageInfo;
	OSStatus			status;
		
	memset( &playerMessageInfo.u.registerVisualPluginMessage, 0, sizeof(playerMessageInfo.u.registerVisualPluginMessage) );

	GetVisualName( playerMessageInfo.u.registerVisualPluginMessage.name );

	SetNumVersion( &playerMessageInfo.u.registerVisualPluginMessage.pluginVersion, kTVisualPluginMajorVersion, kTVisualPluginMinorVersion, kTVisualPluginReleaseStage, kTVisualPluginNonFinalRelease );

	playerMessageInfo.u.registerVisualPluginMessage.options					= GetVisualOptions();
	playerMessageInfo.u.registerVisualPluginMessage.handler					= (VisualPluginProcPtr)VisualPluginHandler;
	playerMessageInfo.u.registerVisualPluginMessage.registerRefCon			= 0;
	playerMessageInfo.u.registerVisualPluginMessage.creator					= kTVisualPluginCreator;
	
	playerMessageInfo.u.registerVisualPluginMessage.pulseRateInHz			= kStoppedPulseRateInHz;	// update my state N times a second
	playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels		= 2;
	playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels		= 2;
	
	playerMessageInfo.u.registerVisualPluginMessage.minWidth				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.minHeight				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.maxWidth				= 0;	// no max width limit
	playerMessageInfo.u.registerVisualPluginMessage.maxHeight				= 0;	// no max height limit
	
	status = PlayerRegisterVisualPlugin( messageInfo->u.initMessage.appCookie, messageInfo->u.initMessage.appProc, &playerMessageInfo );
		
	return status;
}

#if 0 // SpectroGraph
/*
 RegisterVisualPlugin from SpectroGraph
 */
static OSStatus RegisterVisualPlugin(PluginMessageInfo *messageInfo)
{
	OSStatus			status;
	PlayerMessageInfo	playerMessageInfo;
	Str255				pluginName = kTVisualPluginName;
    
	MyMemClear(&playerMessageInfo.u.registerVisualPluginMessage,sizeof(playerMessageInfo.u.registerVisualPluginMessage));
	
	memcpy(&playerMessageInfo.u.registerVisualPluginMessage.name[0], &pluginName[0], pluginName[0] + 1);
    
#if TARGET_OS_MAC					
    CFStringRef tCFStringRef = CFStringCreateWithPascalString( kCFAllocatorDefault, pluginName, kCFStringEncodingUTF8 );
    if ( tCFStringRef ) 
    {
        CFIndex length = CFStringGetLength( tCFStringRef );
        if ( length > 255 ) 
        {
            length = 255;
        }
        playerMessageInfo.u.registerVisualPluginMessage.unicodeName[0] = CFStringGetBytes( tCFStringRef, CFRangeMake( 0, length ), kCFStringEncodingUnicode, 0, FALSE, (UInt8 *) &playerMessageInfo.u.registerVisualPluginMessage.unicodeName[1], 255, NULL );
        CFRelease( tCFStringRef );
    }
#endif //TARGET_OS_MAC					
    
	SetNumVersion(&playerMessageInfo.u.registerVisualPluginMessage.pluginVersion,kTVisualPluginMajorVersion,kTVisualPluginMinorVersion,kTVisualPluginReleaseStage,kTVisualPluginNonFinalRelease);
    
	playerMessageInfo.u.registerVisualPluginMessage.options					=	kVisualWantsIdleMessages 
#if TARGET_OS_MAC					
    | kVisualWantsConfigure | kVisualProvidesUnicodeName
#endif
    ;
	playerMessageInfo.u.registerVisualPluginMessage.handler					= (VisualPluginProcPtr)VisualPluginHandler;
	playerMessageInfo.u.registerVisualPluginMessage.registerRefCon			= 0;
	playerMessageInfo.u.registerVisualPluginMessage.creator					= kTVisualPluginCreator;
	
	/* This determines how often the plugin receives data. The name is deceiving because we can't
	 * get millisecond accuracy. Instead, ticks of 16msec are used, so it's impossible to go faster
	 * than 62.5 packets/second. For this plug-in that's still too slow, so I have to disable this
	 * by using 0xFFFFFFFF (= as fast as possible) and do my own speed control.
	 * If your own plug-in can do with 62.5 frames/second, by all means enable this, because otherwise
	 * you'll have to use similar ugly tricks as I to avoid 100% cpu usage. */	
	playerMessageInfo.u.registerVisualPluginMessage.timeBetweenDataInMS		= 0xFFFFFFFF;
	playerMessageInfo.u.registerVisualPluginMessage.numWaveformChannels		= 2;
	playerMessageInfo.u.registerVisualPluginMessage.numSpectrumChannels		= 2;
	
	playerMessageInfo.u.registerVisualPluginMessage.minWidth				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.minHeight				= 64;
	playerMessageInfo.u.registerVisualPluginMessage.maxWidth				= 32767;
	playerMessageInfo.u.registerVisualPluginMessage.maxHeight				= 32767;
	playerMessageInfo.u.registerVisualPluginMessage.minFullScreenBitDepth	= 0;
	playerMessageInfo.u.registerVisualPluginMessage.maxFullScreenBitDepth	= 0;
	playerMessageInfo.u.registerVisualPluginMessage.windowAlignmentInBytes	= 0;
	
	status = PlayerRegisterVisualPlugin(messageInfo->u.initMessage.appCookie,messageInfo->u.initMessage.appProc,&playerMessageInfo);
	startuSec(&gLineTimeStamp);
	startuSec(&gFrameTimeStamp);
	gnLPU = SG_MAXCHUNK;
	
	return status;
	
}
#endif // 0 SpectroGraph



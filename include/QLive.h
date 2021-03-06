/*
 *  QLive.h
 *
 *  Created by Andrea Cuius
 *  Nocte Copyright 2011 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */

#ifndef QLIVE
#define QLIVE

#pragma once

#include "cinder/app/AppBasic.h"
#include "OscSender.h"
#include "OscListener.h"
#include "cinder/Utilities.h"
#include "QLiveObject.h"
#include "QLiveModule.h"
//#include "QLiveModuleWithFixtures.h"
#include "QLiveAnalyzer.h"

#define	GET_INFO_MIN_DELAY	2.0f		// minimum delay between info requests


namespace nocte {

    

class QLive {
	
public:
    
	QLive( std::string osc_host = "localhost", int osc_live_in_port = 9001, int osc_live_out_port = 9000, bool initFromLive = true );
    
    ~QLive() {}

	void sendMessage(std::string address, std::string args = "");
	
	bool getInfo();

    void renderDebug( bool renderScenes = true, bool renderTracks = true, bool renderClips = true, bool renderDevices = true );
    
    void renderAnalyzer();
	
    void initAnalyzer( int port, const std::string &trackName, const std::string &deviceName );
    
	void play( bool playContinue = false ) 
    { 
        if ( isAlive() )
            if ( playContinue ) 
                sendMessage("/live/play/continue"); 
            else 
                sendMessage("/live/play"); 
        else
            mIsPlaying = true;
    }
	
	void stop() 
    {         
        if ( isAlive() )
            sendMessage("/live/stop");
        else
            mIsPlaying = false;
    }

	void playClip( int trackIdx, int clipIdx ) 
    { 
        QLiveTrack *track = getTrack( trackIdx );
        
        if ( !track )
            return;
        
        QLiveClip *clip = track->getClip(clipIdx);
        
        if ( !clip )
            return;

        if ( isAlive() )
            sendMessage("/live/play/clip", "i" + ci::toString(trackIdx) + " i" + ci::toString(clipIdx) );
      
        else
        {
            std::vector<QLiveClip*> clips = track->getClips();
            
            for( size_t k=0; k < clips.size(); k++ )
                if ( clips[k] == clip )
                    clip->setState( CLIP_PLAYING );
                else
                    clips[k]->setState( HAS_CLIP );
        }
    }
    
	void stopClip( int trackIdx, int clipIdx ) 
    {
        QLiveTrack *track = getTrack( trackIdx );
        
        if ( !track )
            return;
        
        QLiveClip *clip = track->getClip(clipIdx);
        
        if ( !clip )
            return;
        
        if ( isAlive() )
            sendMessage("/live/stop/clip", "i" + ci::toString(trackIdx) + " i" + ci::toString(clipIdx) ); 
        else
            clip->setState( HAS_CLIP );
    }
	    
	void setTrackVolume( int trackIdx, float volume ) 
    { 
        QLiveTrack *track = getTrack( trackIdx );
        
        if ( !track )
            return;
        
        if ( isAlive() )
            sendMessage("/live/volume", "i" + ci::toString(trackIdx) + " f" + ci::toString(volume) ); 
        else
            track->mVolume = volume;
    }
	
    void setParam( int trackIdx, int deviceIdx, int paramIdx, float value ) 
    { 
        QLiveTrack *track = getTrack(trackIdx);
        if (!track)
            return;
        
        QLiveDevice *device = track->getDevice(deviceIdx);
        if (!device)
            return;
        
        QLiveParam *param = device->getParam(paramIdx);
        if(!param)
            return;
        
        // is alive?????
        
        param->mValue = value;
        
        sendMessage("/live/device", "i" + ci::toString(trackIdx) + " i" + ci::toString(deviceIdx) + " i" + ci::toString(paramIdx) + " f" + ci::toString(value) ); 
    }

    void getTrackSends( int idx )
    {
        QLiveTrack *track = getTrack( idx );
        
        if ( !track )
            return;
        
        sendMessage( "/live/send", "i" + ci::toString(idx) );
    }

    void setTrackSend( int trackIdx, int sendIdx, float val )
    {
        QLiveTrack *track = getTrack( trackIdx );
        
        if ( !track )
            return;
        
        track->mSends[sendIdx] = val;   // if the send value doesn't change in Live, it doesn't send anything back!
        
        sendMessage( "/live/send", "i" + ci::toString(trackIdx) + " i" + ci::toString(sendIdx) + " f" + ci::toString(val) );
    }

//	void stopTrack(int track) { sendMessage("/live/stop/track", "i" + ci::toString(track) ); }

//	void setTrackName(int index, std::string name) { sendMessage("/live/name/track", "i" + ci::toString(index) + " s" + name ); };
	
//	void setClip() { // /live/name/clip         (int track, int clip, string name)              Sets clip number clip in track number track's name to name };
	
//	void playScene(int scene) {	sendMessage("/live/play/scene", "i" + ci::toString(scene) ); };
    
    std::vector<QLiveTrack*> getTracks() { return mTracks; }
    
    std::vector<QLiveScene*> getScenes() { return mScenes; }
    
	QLiveClip* getClip( int trackIdx, int clipIdx )
    {   
        QLiveTrack *track = getTrack(trackIdx);
        
        if ( track )
            return track->getClip(clipIdx);            
            
        QLiveClip *clip = NULL;
        return clip;
    }
	
    QLiveClip* getClip( const std::string trackName, const std::string clipName )
    {   
        QLiveTrack *track = getTrack(trackName);
        
        if ( track )
            return track->getClip(clipName);            
        
        QLiveClip *clip = NULL;
        return clip;
    }
    
	QLiveTrack* getTrack( int trackIdx ) 
    { 
        for( size_t k=0; k < mTracks.size(); k++ )
            if ( mTracks[k]->mIndex == trackIdx )
                return mTracks[k];
                
        QLiveTrack* track = NULL;
        return track;
    }
    
    QLiveTrack* getTrack( const std::string &name ) 
    { 
        for( size_t k=0; k < mTracks.size(); k++ )
            if ( mTracks[k]->mName == name )
                return mTracks[k];
        
        QLiveTrack* track = NULL;
        return track;
    }
    
	QLiveScene* getScene( int idx )
    {   
        for( size_t k=0; k < mScenes.size(); k++ )
            if ( mScenes[k]->mIndex == idx )
                return mScenes[k];
        
        QLiveScene* scene = NULL;  
        return scene; 
    }
    
    QLiveScene* getScene( const std::string &name )
    {   
        for( size_t k=0; k < mScenes.size(); k++ )
            if ( mScenes[k]->mName == name )
                return mScenes[k];
        
        QLiveScene* scene = NULL;  
        return scene; 
    }
	
	QLiveDevice* getDevice( int trackIdx, int deviceIdx ) 
    {         
        QLiveTrack *track = getTrack(trackIdx);
        
        if ( track )
            return track->getDevice(deviceIdx);            
        
        QLiveDevice *device = NULL;
        return device;
    }
    
    QLiveDevice* getDevice( const std::string &trackName, const std::string &deviceName )
    {         
        QLiveTrack *track = getTrack(trackName);
        
        if ( track )
            return track->getDevice(deviceName);            
        
        QLiveDevice *device = NULL;
        return device;
    }
    
    QLiveParam* getParam( int trackIdx, int deviceIdx, std::string name ) 
    { 
        QLiveTrack  *track  = getTrack( trackIdx );
        
        if ( track )
        {   
            QLiveDevice *device = track->getDevice( deviceIdx );
            if ( device )
                return device->getParam(name);
        }
        
        QLiveParam *param = NULL;
        return param;
    }

	float getParamValue( int trackIdx, int deviceIdx, std::string name ) 
    { 
        QLiveTrack  *track  = getTrack( trackIdx );
        
        if ( track )
        {   
            QLiveDevice *device = track->getDevice( deviceIdx );
            if ( device )
            {
                QLiveParam *param = device->getParam(name);
                if ( param )
                    return param->getValue();
            }
        }
        
        return 0;
    }
    
    float* getParamRef( int trackIdx, int deviceIdx, std::string name ) 
    { 
        QLiveTrack  *track  = getTrack( trackIdx );
        
        if ( track )
        {   
            QLiveDevice *device = track->getDevice( deviceIdx );
            if ( device )
            {
                QLiveParam *param = device->getParam(name);
                if ( param )
                    return param->getRef();
            }
        }
        
        float *ref = NULL;
        return ref;
    }
    
	bool isPlaying() { return mIsPlaying; }

	bool* getIsPlayingRef() { return &mIsPlaying; }
	
	QLiveTrack*	getSelectedTrack() { return mSelectedTrack; }
	
	void	shutdown();
	
	bool	isOscListenerConnected()	{ return mOscListener != NULL; }
	
	bool	isOscSenderConnected()		{ return mOscSender != NULL; }
	
	void	initOsc();
	
	float	getFreqAmplitude(int freq, int channel);
    
    float* getFftBuffer(int channel);
    float* getAmplitudeRef(int channel);

    bool isReady() { return mIsReady; }
        
    void loadSettings( const std::string &filename, bool forceXmlSettings = false );
    
    void saveSettings( const std::string &filename );
    
    bool isAlive()
    {
        if ( ci::app::getElapsedSeconds() - mPingReceivedAt < 0.5f )
            return true;

        return false;
    }
    
    bool hasAnalyzer() { return mAnalyzer != NULL; }
    
private:
    
	void	listTrackDevices(int trackIndex) { sendMessage("/live/devicelist", "i" + ci::toString(trackIndex) ); }
	
	void	receiveData();
	
	void	deleteObjects();
	
    ci::ColorA colorIntToColorA( int colorInt )
    {
        ci::ColorA color;
        color.r = (float)((colorInt >> 16) & 0xB0000FF) / 255.0f;
        color.g = (float)((colorInt >> 8) & 0xB0000FF) / 255.0f;
        color.b = (float)(colorInt & 0xB0000FF) / 255.0f;
        color.a	= 1.0f;
        
        return color;
    }
    
	void	parseScene( ci::osc::Message message );
	void	parseTrack( ci::osc::Message message );
	void	parseClip( ci::osc::Message message );
	void	parseClipInfo( ci::osc::Message message );
	void	parseDeviceList( ci::osc::Message message );
	void	parseDeviceAllParams( ci::osc::Message message );
	void	parseDeviceParam( ci::osc::Message message );
    void	parseTrackSends( ci::osc::Message message );
    
    void    debugOscMessage( ci::osc::Message message );
    
	std::vector<QLiveTrack*>		mTracks;
	std::vector<QLiveScene*>		mScenes;
	
    QLiveAnalyzer                   *mAnalyzer;
    
	ci::osc::Sender					*mOscSender;
	ci::osc::Listener				*mOscListener;
	
	bool							mIsPlaying;
	
	std::string						mOscHost;
	int								mOscLiveInPort;
	int								mOscLiveOutPort;
    
	ci::Font						mFontSmall;
	ci::Font						mFontMedium;
	
	double							mGetInfoRequestAt;
	
	QLiveTrack						*mSelectedTrack;
    
    bool                            mIsReady;
    
    double                          mPingReceivedAt;
};

}

#endif
/*
 *  QLiveModule.h
 *
 *  Created by Andrea Cuius on 08/08/2011.
 *  Nocte Copyright 2011 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 */


#ifndef QLIVE_MODULE
#define QLIVE_MODULE

#pragma once

#include "cinder/app/AppBasic.h"
//#include "QLive.h"

namespace nocte {
    
    class QLive;
    class QLiveClip;
    class QLiveTrack;
        
    class QLiveModule {

    public:
        
        QLiveModule() {}
        
        QLiveModule( QLive *live, QLiveClip *clip );
        
        ~QLiveModule() {}
        
        virtual void render( float height ) {}
        
    //	virtual void update( float *values, float masterBrightness, float baseBrightness ) {}
        
        virtual void update( float *values ) {}
        
        bool updateModule() 
        {
            updateBrightness();
            
            return updateState();
        }
        
        void play() { mIsClipPlaying = true; }
        
        void stop() { mIsClipPlaying = false; }
        
        bool updateState();
        
        bool isPlaying();
        
        QLiveClip*	getClip() { return mClip; }
        
        QLiveTrack*	getTrack();
        
        void		updateBrightness();
        
        float		getTrackVolume() { return mTrackVolume; }
        
        std::string getSettings() { return mSettingsStr; }
        
        int			getTrackIndex();
        
        std::string getName();
        
        std::string getTypeString() { return mTypeString; }
        
        
    protected:
        
        QLive			*mLive;
        QLiveClip		*mClip;
        
        bool			mIsPlaying;			// true when both clip and Live are playing
        bool			mIsClipPlaying;		// true when clip is playing
        
        float			mTrackVolume;
        
        std::string		mSettingsStr;
        std::string     mTypeString;
        
        std::vector<int>        mFreqs;
        float**                 mFftBuffer;
    };


}

#endif
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

#include "cinder/Xml.h"
//#include "QLive.h"
#include <boost/tuple/tuple.hpp>

namespace nocte {
    
    class QLive;
    class QLiveClip;
    class QLiveTrack;    
    
    class QLiveModule {

    public:
        
        QLiveModule() {}
        
        QLiveModule( QLive *live, QLiveTrack *track, QLiveClip *clip );
        
        ~QLiveModule() {}
        
        virtual void render( float height ) {}
        
        virtual void update( float *values ) {}
        
        bool updateModule();

        bool isPlaying();
        
        QLiveClip*	getClip() { return mClip; }
        
        QLiveTrack*	getTrack() { return mTrack; }
        
        void		updateBrightness();
        
        float		getTrackVolume() { return mTrackVolume; }
        
        std::string getSettings() { return mSettingsStr; }
        
        int			getTrackIndex();
        
        std::string getName();
        
        std::string getTypeString() { return mTypeString; }
        
        virtual ci::XmlTree getXmlNode() 
        {
            ci::XmlTree node( "module", "" );
            node.setAttribute( "effect",        mTrack->getName() );
            node.setAttribute( "name",          mClip->getName() );
            node.setAttribute( "track",         mTrack->getIndex() );
            node.setAttribute( "clip",          mClip->getIndex() );
            
            // params
            ci::XmlTree pNode( "param", "" );
            std::map< std::string, boost::tuple<float,float*,int,int> >::iterator it;
            for ( it=mParams.begin(); it != mParams.end(); it++ )
            {
                pNode.setAttribute( "name", it->first );
                pNode.setAttribute( "value", getParamValue(it->first) );
                node.push_back( pNode );
            }
            
            return node;
        }
        
        virtual void loadXmlNode( ci::XmlTree node )
        {       
            std::string name;
            float       value;

            for( ci::XmlTree::Iter it = node.begin("param"); it != node.end(); ++it )
            {
                name    = it->getAttributeValue<std::string>("name");
                value   = it->getAttributeValue<float>("value");    
                
                if ( mParams.count(name) )
                    boost::get<0>(mParams[name]) = value;

            }    
            
            clipStateUpdateCallback();
        }
        
        void sendLocalParamValues( const std::string &name ); // set Live value to Module local value is it's playing(is selected)

        static void saveSettings( std::vector<QLiveModule*> modules ) {}
        static void loadSettings( std::vector<QLiveModule*> modules ) {}
        
        void clipStateUpdateCallback();
        
    protected:
    
        void registerParam( int deviceIdx, const std::string &name )
        {
            QLiveDevice *device = mTrack->getDevice( deviceIdx );
            
            if ( !device )
            {
                ci::app::console() << getName() << "::registerParam() cannot find device " << deviceIdx << std::endl;
                exit(-1);
            }
            
            QLiveParam *param = device->getParam(name);
            
            if ( param )
                mParams[name] = boost::make_tuple( param->getValue(), param->getRef(), device->getIndex(), param->getIndex() );
            
            else
                mParams[name] = boost::make_tuple( 0.0f, new float(0.0f), device->getIndex(), -1 );
        }
        
        float getParamValue( const std::string &name )
        {
            return boost::get<0>(mParams[name]);
        }
        
    protected:
        
        QLive               *mLive;
        QLiveTrack          *mTrack;
        QLiveClip           *mClip;

        std::map< std::string, boost::tuple<float,float*,int,int> >   mParams;

        double              mParamsUpdatedAt;
        
        float               mTrackVolume;
        
        std::string         mSettingsStr;
        std::string         mTypeString;
        
        std::vector<int>    mFreqs;
        float**             mFftBuffer;
        
        
    };

}

#endif
/*
 *  QLiveModuleWithFixtures.h
 *
 *  Created by Andrea Cuius on 08/08/2011.
 *  Nocte Copyright 2011 . All rights reserved.
 *
 *  www.nocte.co.uk
 *
 *  REQUIRES: nocte::Scene from NocteBlock
 *
 */


#ifndef QLIVE_MODULE_WITH_FIXTURES
#define QLIVE_MODULE_WITH_FIXTURES

#pragma once

//#include "cinder/app/AppBasic.h"
#include "Fixture.h"

namespace nocte {
    
    class QLive;
    class QLiveClip;
    class QLiveTrack;
    
    class QLiveModuleWithFixtures : public QLiveModule {
        
    public:
        
        QLiveModuleWithFixtures( QLive *live, QLiveClip *clip, std::vector<Fixture*> fixtures ) : QLiveModule(live, clip), mFixtures(fixtures)
        {
            mBoundingBox = ci::AxisAlignedBox3f( ci::Vec3f(-1.0f, 0.0f, -1.0f), ci::Vec3f::one() );

            calcActiveFixtures();
        }
        
        ~QLiveModuleWithFixtures() {}
        
        
        void renderBoundingBox( bool isMouseOn, bool isSelected  ) 
        {
            ci::ColorA  boxCol = mClip->getColor();
            
            if ( isSelected )
                glLineWidth( 2.0f );
            
            // draw stroke
            ci::gl::color( boxCol );
            ci::gl::drawStrokedCube(mBoundingBox);
            
            boxCol.a = ( isSelected || isMouseOn ) ? 0.2f : 0.1f;
             
            // draw cube
            ci::gl::color( boxCol );
            ci::gl::drawCube( mBoundingBox.getCenter(), mBoundingBox.getSize() );      
                
            ci::gl::color( ci::ColorA::white() );
            
            glLineWidth( 1.0f );
        }
        
        void setBoundingBox( ci::Vec3f minVec, ci::Vec3f maxVec )
        {            
            mBoundingBox = ci::AxisAlignedBox3f( minVec, maxVec );

            calcActiveFixtures();
        }   
        
        ci::AxisAlignedBox3f getBoundingBox() { return mBoundingBox; }
        
        
    protected:
        
        void calcActiveFixtures()
        {
            mActiveFixtureIndices.clear();
            
            ci::Vec3f minVec = mBoundingBox.getMin();
            ci::Vec3f maxVec = mBoundingBox.getMax();
            
            ci::Rectf rectXY( minVec.xy(), maxVec.xy() );
            ci::Rectf rectXZ( minVec.xz(), maxVec.xz() );
            
            ci::Vec3f pos3f;
            
            for( size_t k=0; k < mFixtures.size(); k++ )
            {
                pos3f = mFixtures[k]->getPos();
                
                if ( rectXY.contains( pos3f.xy() ) && rectXZ.contains( pos3f.xz() ) )
                    mActiveFixtureIndices.push_back( k );
            }
        }
        
        std::vector<int>        mActiveFixtureIndices;
        std::vector<Fixture*>   mFixtures;
        ci::AxisAlignedBox3f    mBoundingBox;
        
    private:
        // disallow
        QLiveModuleWithFixtures(const QLiveModuleWithFixtures&);
        QLiveModuleWithFixtures& operator=(const QLiveModuleWithFixtures&);
        
    };


}

#endif
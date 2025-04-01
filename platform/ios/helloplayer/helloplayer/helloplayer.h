//
//  helloplayer.h
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/18.
//


#import <Foundation/Foundation.h>


//! Project version number for helloplayer.
FOUNDATION_EXPORT double helloplayerVersionNumber;

//! Project version string for helloplayer.
FOUNDATION_EXPORT const unsigned char helloplayerVersionString[];

// In this header, you should import all the public headers of your framework using statements like #import <helloplayer/PublicHeader.h>


#include "player/HelloPlayer.hpp"
#include "log/Logger.hpp"
#include "platform/player/HelloiOSCallbackCtx.hpp"


namespace HelloPlayerContext {
    void init();
}

//
//  HelloPlayerState.h
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, PlayState) {
    PlayStateIdle = 0,
    PlayStatePreparing,
    PlayStatePrepared,
    PlayStatePlaying,
    PlayStateSeeking,
    PlayStateSeekEnd,
    PlayStatePaused,
    PlayStateComplete
};

NS_ASSUME_NONNULL_END

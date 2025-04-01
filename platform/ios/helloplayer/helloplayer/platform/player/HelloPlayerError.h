//
//  HelloPlayerError.h
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/21.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSInteger, PlayError) {
    UnKnown = -1,
    None = 0,
    NoDataSource = -100,
    NoStreamInDataSource = -101,
    NoAudioVideoStream = -102,
    NetworkTimeout = -400
};

NS_ASSUME_NONNULL_END

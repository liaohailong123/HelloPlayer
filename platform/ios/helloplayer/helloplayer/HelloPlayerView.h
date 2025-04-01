//
//  HelloPlayerView.h
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/23.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface HelloPlayerView : UIView

- (void) setDataSource:(NSString*)url autoPlay:(bool)autoPlay useHardware:(bool)useHardware;

- (void) setVolume:(double)volume;

- (void) setSpeed:(double)speed;

@end

NS_ASSUME_NONNULL_END

#import "HelloPlayerView.h"
#include <helloplayer/helloplayer.h>

@interface HelloPlayerView ()

@property (nonatomic) HelloiOSCallbackCtx *ctx;
@property (nonatomic) HelloPlayer *player;
@property (nonatomic) PlayConfig config;
@property (nonatomic) int64_t durationUs;

@property(strong, nonatomic) CAMetalLayer *metalLayer;
@property (nonatomic, strong) UIButton *playPauseButton;
@property (nonatomic, strong) UISlider *progressSlider;
@property (nonatomic, assign) BOOL isSliding;
@property (nonatomic, strong) UILabel *timeLabel;

@end

@implementation HelloPlayerView

- (instancetype)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self setupPlayer];
        [self setupUI];
    }
    return self;
}

- (void)setupPlayer {
    self.ctx = new HelloiOSCallbackCtx();
    self.player = new HelloPlayer(self.ctx);
    
    // 捕获 self
    self.ctx->userdata = (__bridge void*)self;
    self.ctx->onPrepared = [](const Hello::MediaInfo &info, void* userdata)->void {
        HelloPlayerView *native = (__bridge HelloPlayerView*)userdata;
        native->_durationUs = info.masterClockType == 0 ? info.audioDurationUs : info.videoDurationUs;
        // 多媒体资源准备好之后,添加渲染缓冲区,承载画面
        native->_player->addSurface((__bridge_retained void*)native->_metalLayer);
    };
    self.ctx->onPlayStateChanged = [](PlayState from, PlayState to, void* userdata)->void {
        HelloPlayerView *native = (__bridge HelloPlayerView*)userdata;
        if(native->_player->isPlaying()) {
            [native updatePlayPauseBtn:NO];
        } else {
            [native updatePlayPauseBtn:YES];
        }
    };
    self.ctx->onCurrentPosition = [](int64_t ptsUs, int64_t durationUs, void* userdata)->void {
        HelloPlayerView *native = (__bridge HelloPlayerView*)userdata;
        if (!native->_isSliding) {
            float progress = ptsUs * 1.0 / durationUs;
            native->_progressSlider.value = progress;

            [native updateTimeLabel:ptsUs durationUs:durationUs];
        }
    };
    self.ctx->onBufferPosition = [](int64_t startUs, int64_t endUs, int64_t durationUs, void* userdata)->void {
//        HelloPlayerView *native = (__bridge HelloPlayerView*)userdata;

    };
    
    PlayConfig config;
    config.bufferDurationUs = 10 * 1000 * 1000;
    config.autoPlay = true;
    config.loopPlay = true;
    config.speed = 1.0;
    config.volume = 1.0;
    config.useHardware = false;
    self.config = config;
}

- (void) updateTimeLabel: (int64_t)ptsUs durationUs:(int64_t)durationUs {
    NSString *currentStr = [self formatTime:ptsUs];
    NSString *durationStr = [self formatTime:durationUs];
    self.timeLabel.text = [NSString stringWithFormat:@"%@ / %@", currentStr, durationStr];
}

- (NSString *)formatTime:(int64_t)timeUs {
    int totalSeconds = (int)(timeUs / 1000000); // 微秒转秒
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    
    if (hours > 0) {
        return [NSString stringWithFormat:@"%02d:%02d:%02d", hours, minutes, seconds];
    } else {
        return [NSString stringWithFormat:@"%02d:%02d", minutes, seconds];
    }
}

- (void)setupUI {
    
    // 设置黑色背景
    self.backgroundColor = [UIColor blackColor];
    
    self.metalLayer = [[CAMetalLayer alloc] init];
    self.metalLayer.device = MTLCreateSystemDefaultDevice();
    self.metalLayer.frame = self.frame;
    self.metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    self.metalLayer.framebufferOnly = true;
    self.metalLayer.contentsScale = 1.0;
    [self.layer addSublayer:self.metalLayer];
    
    // 播放/暂停按钮
    _playPauseButton = [UIButton buttonWithType:UIButtonTypeSystem];
    [_playPauseButton setTitle:@"播放" forState:UIControlStateNormal];
    [_playPauseButton addTarget:self action:@selector(playPauseButtonTapped) forControlEvents:UIControlEventTouchUpInside];
    _playPauseButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_playPauseButton];
    
    // 进度条
    _progressSlider = [[UISlider alloc] init];
    _progressSlider.translatesAutoresizingMaskIntoConstraints = NO;
    _progressSlider.maximumValue = 1.0;
    UIImage *thumbImage = [self createThumbImageWithSize:CGSizeMake(15, 15) color:[UIColor whiteColor]];
    [_progressSlider setThumbImage:thumbImage forState:UIControlStateNormal];
    [_progressSlider addTarget:self action:@selector(sliderTouchDown) forControlEvents:UIControlEventTouchDown];
    [_progressSlider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    [_progressSlider addTarget:self action:@selector(sliderTouchUp:) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    [self addSubview:_progressSlider];
    
    // 时间标签
    _timeLabel = [[UILabel alloc] init];
    _timeLabel.text = @"00:00/00:00";
    _timeLabel.font = [UIFont systemFontOfSize:12];
    _timeLabel.textColor = [UIColor whiteColor];
    _timeLabel.translatesAutoresizingMaskIntoConstraints = NO;
    [self addSubview:_timeLabel];
    
    // 约束
    [NSLayoutConstraint activateConstraints:@[
        // 播放按钮 左下角
        [_playPauseButton.leadingAnchor constraintEqualToAnchor:self.leadingAnchor constant:10],
        [_playPauseButton.bottomAnchor constraintEqualToAnchor:self.bottomAnchor constant:-10],
        
        // 时间标签 右下角
        [_timeLabel.trailingAnchor constraintEqualToAnchor:self.trailingAnchor constant:-10],
        [_timeLabel.centerYAnchor constraintEqualToAnchor:_playPauseButton.centerYAnchor],
        
        // 进度条 水平居中，位于按钮上方
        [_progressSlider.leadingAnchor constraintEqualToAnchor:_playPauseButton.trailingAnchor constant:10],
        [_progressSlider.trailingAnchor constraintEqualToAnchor:_timeLabel.leadingAnchor constant:-10],
        [_progressSlider.centerYAnchor constraintEqualToAnchor:_playPauseButton.centerYAnchor]
    ]];
}

- (void)layoutSubviews {
    [super layoutSubviews];
    if (self.metalLayer) {
        self.metalLayer.frame = self.bounds;
    }
}

- (UIImage *)createThumbImageWithSize:(CGSize)size color:(UIColor *)color {
    UIGraphicsBeginImageContextWithOptions(size, NO, 0.0);
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSetFillColorWithColor(context, color.CGColor);
    CGContextFillEllipseInRect(context, CGRectMake(0, 0, size.width, size.height));
    UIImage *thumbImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    return thumbImage;
}

- (void)sliderTouchDown {
    self.isSliding = YES;
}

- (void)sliderValueChanged:(UISlider *)slider {
    if (!_durationUs) return;
    
    float progress = slider.value;
    int64_t currentPtsUs = _durationUs * progress;
    
    [self updateTimeLabel:currentPtsUs durationUs:_durationUs];
}

- (void)sliderTouchUp:(UISlider *)slider {
    self.isSliding = NO;
    
    if (!_player) return;
    
    // 计算拖动位置对应的 ptsUs
    float progress = slider.value;

    int64_t seekPtsUs = _durationUs * progress;
    bool autoPlay = _player->isPlaying();
    _player->seekTo(seekPtsUs, autoPlay);
}

- (void)playPauseButtonTapped {
    if(_player->isPlaying()) {
        [self updatePlayPauseBtn:NO];
        _player->pause();
    } else {
        [self updatePlayPauseBtn:YES];
        _player->start();
    }
}

- (void)updatePlayPauseBtn:(bool)playing {
    if (playing) {
        [_playPauseButton setTitle:@"播放" forState:UIControlStateNormal];
    } else {
        [_playPauseButton setTitle:@"暂停" forState:UIControlStateNormal];
    }
}

- (void)setDataSource:(NSString*)url autoPlay:(bool)autoPlay useHardware:(bool)useHardware {
    if (!_player) {
        return;
    }
    if (_player->isPrepared()) {
        _player->reset();
    }
    const char *cUrl = [url UTF8String];
    _config.autoPlay = autoPlay;
    _config.useHardware = useHardware;
    self.player->setConfig(_config);
    self.player->setDataSource(cUrl);
    self.player->prepare();
}

- (void) setVolume:(double)volume {
    if (!_player) {
        return;
    }
    _player->setVolume(volume);
}

- (void) setSpeed:(double)speed {
    if (!_player) {
        return;
    }
    _player->setSpeed(speed);
}

- (void)dealloc
{
    if (_player) {
        _player->pause();
        delete _player;
        _player = nullptr;
    }
}

@end

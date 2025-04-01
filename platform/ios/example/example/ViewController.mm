#import "ViewController.h"
#import <helloplayer/HelloPlayerView.h>

@interface ViewController ()

@property (nonatomic, strong) HelloPlayerView *playerView;
@property (nonatomic, strong) UIStackView *buttonGrid; // 网格布局容器

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    [self setupButtonGrid]; // 创建操作按钮区域

    
    // 创建播放器
    self.playerView = [[HelloPlayerView alloc] init];
    self.playerView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:self.playerView];
    
    // 添加 AutoLayout 约束
    [NSLayoutConstraint activateConstraints:@[
        // 按钮居中 & 顶部间距
        [self.buttonGrid.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor constant:20],
        [self.buttonGrid.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
        
        // 播放器顶部紧贴按钮底部
        [self.playerView.topAnchor constraintEqualToAnchor:self.buttonGrid.bottomAnchor constant:20],
        [self.playerView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
        [self.playerView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
        // 高度可以是宽度的 9:16 比例（或者随意固定高度）
        [self.playerView.heightAnchor constraintEqualToAnchor:self.playerView.widthAnchor multiplier:1080.0/1080.0],
    ]];
}

- (void)setupButtonGrid {
    // 创建网格布局容器
    self.buttonGrid = [[UIStackView alloc] init];
    self.buttonGrid.axis = UILayoutConstraintAxisVertical; // 垂直方向
    self.buttonGrid.distribution = UIStackViewDistributionFillEqually; // 均匀分布
    self.buttonGrid.spacing = 10; // 按钮间距
    self.buttonGrid.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:self.buttonGrid];
    
    // 创建水平方向的 UIStackView（第一行）
    UIStackView *rowStack = [[UIStackView alloc] init];
    rowStack.axis = UILayoutConstraintAxisHorizontal; // 水平方向
    rowStack.distribution = UIStackViewDistributionFillEqually; // 均匀分布
    rowStack.spacing = 10; // 按钮间距
    [self.buttonGrid addArrangedSubview:rowStack];
    
    // 软解H264(网络)
    UIButton *playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"软解H264(网络)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareSoft) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 软解H265(网络)
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"软解H265(网络)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareSoft2) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 软解H264(直播)
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"软解H264(直播)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareSoft3) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    
    
    
    // 创建水平方向的 UIStackView（第二行）
    rowStack = [[UIStackView alloc] init];
    rowStack.axis = UILayoutConstraintAxisHorizontal; // 水平方向
    rowStack.distribution = UIStackViewDistributionFillEqually; // 均匀分布
    rowStack.spacing = 10; // 按钮间距
    [self.buttonGrid addArrangedSubview:rowStack];
    
    // 硬解H264(网络)
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"硬解H264(网络)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareHard) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 硬解H265(网络)
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"硬解H265(网络)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareHard2) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 硬解H264(直播)
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"硬解H264(直播)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareHard3) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    
    
    
    // 创建水平方向的 UIStackView（第三行）
    rowStack = [[UIStackView alloc] init];
    rowStack.axis = UILayoutConstraintAxisHorizontal; // 水平方向
    rowStack.distribution = UIStackViewDistributionFillEqually; // 均匀分布
    rowStack.spacing = 10; // 按钮间距
    [self.buttonGrid addArrangedSubview:rowStack];
    
    
    // 速率0.5X
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"速率0.5X" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onSpeedSlow) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 速率1.0X
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"速率1.0X" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onSpeedNormal) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 速率1.5X
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"速率1.5X" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onSpeedFast1) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // OpenGLES 渲染耗时降不下来 2.0 倍速实现不了
//    // 速率2.0X
//    prepareSoftBtn = [UIButton buttonWithType:UIButtonTypeSystem];
//    [prepareSoftBtn setTitle:@"速率2.0X" forState:UIControlStateNormal];
//    [prepareSoftBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
//    [prepareSoftBtn addTarget:self action:@selector(onSpeedFast2) forControlEvents:UIControlEventTouchUpInside];
//    prepareSoftBtn.translatesAutoresizingMaskIntoConstraints = NO;
//    [prepareSoftBtn sizeToFit];
//    // 设置按钮字体大小
//    prepareSoftBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
//    // 限制按钮最大宽度
//    [prepareSoftBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
//    [rowStack addArrangedSubview:prepareSoftBtn];
}




- (void) onPrepareSoft {
	[self.playerView setSpeed:1.0]; 
    [self.playerView setDataSource:@"http://demo-videos.qnsdk.com/VR-Panorama-Equirect-Angular-4500k.mp4" autoPlay:true useHardware:false];
}

- (void) onPrepareSoft2 {
    [self.playerView setSpeed:1.0];
    [self.playerView setDataSource:@"http://demo-videos.qnsdk.com/bbk-H265-50fps.mp4" autoPlay:true useHardware:false];
}

- (void) onPrepareSoft3 {
    [self.playerView setSpeed:1.0];
    [self.playerView setDataSource:@"rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid" autoPlay:true useHardware:false];
}

- (void) onPrepareHard {
    [self.playerView setSpeed:1.0];
    [self.playerView setDataSource:@"http://demo-videos.qnsdk.com/VR-Panorama-Equirect-Angular-4500k.mp4" autoPlay:true useHardware:true];
}

- (void) onPrepareHard2 {
    [self.playerView setSpeed:1.0];
    [self.playerView setDataSource:@"http://demo-videos.qnsdk.com/bbk-H265-50fps.mp4" autoPlay:true useHardware:true];
}

- (void) onPrepareHard3 {
    [self.playerView setSpeed:1.0];
    [self.playerView setDataSource:@"rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid" autoPlay:true useHardware:true];
}


- (void) onSpeedSlow {
    [self.playerView setSpeed:0.5];
}

- (void) onSpeedNormal {
    [self.playerView setSpeed:1.0];
}

- (void) onSpeedFast1 {
    [self.playerView setSpeed:1.5];
}
- (void) onSpeedFast2 {
    [self.playerView setSpeed:2.0];
}


- (void) onVolume:(double)volume {
    [self.playerView setVolume: volume];
}


@end

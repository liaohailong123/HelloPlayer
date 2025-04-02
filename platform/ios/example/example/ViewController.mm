#import "ViewController.h"
#import <helloplayer/HelloPlayerView.h>

@interface ViewController ()

@property (nonatomic, strong) HelloPlayerView *playerView;
@property (nonatomic, strong) UIStackView *buttonGrid; // 网格布局容器
@property (nonatomic, strong) UIStackView *volumeController; // 音量控制
@property (nonatomic, strong) UILabel *valueLabel; // 音量值
@property (nonatomic, strong) UILabel *titleLabel; // 新增的标题 UILabel


@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.view.backgroundColor = [UIColor whiteColor];
    
    // 标题

    [self setupTitleLabel];
    [self setupButtonGrid]; // 创建操作按钮区域
    [self setupVolumeController]; // 创建音量控制UI
    
    // 创建播放器
    self.playerView = [[HelloPlayerView alloc] init];
    self.playerView.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:self.playerView];
    
    // 添加 AutoLayout 约束
    [NSLayoutConstraint activateConstraints:@[
        [self.titleLabel.topAnchor constraintEqualToAnchor:self.view.safeAreaLayoutGuide.topAnchor constant:20],
        [self.titleLabel.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
        
        [self.buttonGrid.topAnchor constraintEqualToAnchor:self.titleLabel.bottomAnchor constant:20],
        [self.buttonGrid.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
        
        [self.volumeController.topAnchor constraintEqualToAnchor:self.buttonGrid.bottomAnchor],
        [self.volumeController.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor constant:20],
        [self.volumeController.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor constant:-20],
        [self.volumeController.heightAnchor constraintEqualToConstant:40],
        
        [self.playerView.topAnchor constraintEqualToAnchor:self.volumeController.bottomAnchor constant:20],
        [self.playerView.leadingAnchor constraintEqualToAnchor:self.view.leadingAnchor],
        [self.playerView.trailingAnchor constraintEqualToAnchor:self.view.trailingAnchor],
        [self.playerView.heightAnchor constraintEqualToAnchor:self.playerView.widthAnchor multiplier:1080.0/1080.0],
    ]];
}

- (void)setupTitleLabel {
    // 初始化标题 UILabel
    self.titleLabel = [[UILabel alloc] init];
    self.titleLabel.text = @"HelloPlayer"; // 设置标题文本
    self.titleLabel.font = [UIFont boldSystemFontOfSize:24]; // 设置字体为粗体，大小为24
    self.titleLabel.textColor = [UIColor blackColor]; // 设置字体颜色为黑色
    self.titleLabel.translatesAutoresizingMaskIntoConstraints = NO; // 使用 AutoLayout
    [self.view addSubview:self.titleLabel]; // 添加到视图中
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
    [playBtn setTitle:@"软解H264(本地)" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onPrepareSoft) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
    // 软解AV1(本地)
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"软解AV1(本地)" forState:UIControlStateNormal];
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
    
    // 速率2.0X
    playBtn = [UIButton buttonWithType:UIButtonTypeSystem];
    [playBtn setTitle:@"速率2.0X" forState:UIControlStateNormal];
    [playBtn setTitleColor:[UIColor blueColor] forState:UIControlStateNormal];
    [playBtn addTarget:self action:@selector(onSpeedFast2) forControlEvents:UIControlEventTouchUpInside];
    playBtn.translatesAutoresizingMaskIntoConstraints = NO;
    [playBtn sizeToFit];
    // 设置按钮字体大小
    playBtn.titleLabel.font = [UIFont systemFontOfSize:14];  // 设置字体大小为14，或者你可以根据需要调整
    // 限制按钮最大宽度
    [playBtn.widthAnchor constraintLessThanOrEqualToConstant:150].active = YES;
    [rowStack addArrangedSubview:playBtn];
    
}


- (void)setupVolumeController {
    self.volumeController = [[UIStackView alloc] init];
    self.volumeController.axis = UILayoutConstraintAxisHorizontal; // 水平方向
    self.volumeController.translatesAutoresizingMaskIntoConstraints = NO;
//    self.volumeController.backgroundColor = [UIColor orangeColor];
    [self.view addSubview:self.volumeController];
    
    
    // 时间标签
    UILabel *_volumeLabel = [[UILabel alloc] init];
    _volumeLabel.text = @"音量: ";
    _volumeLabel.font = [UIFont systemFontOfSize:12];
    _volumeLabel.textColor = [UIColor grayColor];
    [self.volumeController addArrangedSubview:_volumeLabel];
    
    // 进度条
    UISlider *_progressSlider = [[UISlider alloc] init];
    _progressSlider.translatesAutoresizingMaskIntoConstraints = NO;
    _progressSlider.maximumValue = 1.0;
    _progressSlider.value = 0.5;
    UIImage *thumbImage = [self createThumbImageWithSize:CGSizeMake(15, 15) color:[UIColor blueColor]];
    [_progressSlider setThumbImage:thumbImage forState:UIControlStateNormal];
    [_progressSlider addTarget:self action:@selector(sliderValueChanged:) forControlEvents:UIControlEventValueChanged];
    [self.volumeController addArrangedSubview:_progressSlider];
    
    // 时间标签
    _valueLabel = [[UILabel alloc] init];
    _valueLabel.text = @"50";
    _valueLabel.font = [UIFont systemFontOfSize:12];
    _valueLabel.textColor = [UIColor purpleColor];
    [self.volumeController addArrangedSubview:_valueLabel];
    
    // 约束
    [NSLayoutConstraint activateConstraints:@[
        // 播放按钮 左下角
        [_volumeLabel.leadingAnchor constraintEqualToAnchor:self.volumeController.leadingAnchor],
        [_volumeLabel.centerYAnchor constraintEqualToAnchor:self.volumeController.centerYAnchor],
        
        // 时间标签 右下角, 给父容器右侧添加间距
        [_valueLabel.trailingAnchor constraintEqualToAnchor:self.volumeController.trailingAnchor], // 间距20
        [_valueLabel.centerYAnchor constraintEqualToAnchor:self.volumeController.centerYAnchor],
        
        // 进度条 水平居中，位于按钮上方
        [_progressSlider.leadingAnchor constraintEqualToAnchor:_volumeLabel.trailingAnchor constant:10], // 间距10
        [_progressSlider.trailingAnchor constraintEqualToAnchor:_valueLabel.leadingAnchor constant:-10], // 间距10
        [_progressSlider.centerYAnchor constraintEqualToAnchor:self.volumeController.centerYAnchor]
    ]];
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


- (void)sliderValueChanged:(UISlider *)slider {
    float progress = slider.value * 2; // [0.0,1.0] 扩大到 [0.0,2.0]
    
    self.valueLabel.text = [NSString stringWithFormat:@"%.0f", slider.value*100];
    [self.playerView setVolume:progress];
}


- (void) onPrepareSoft {
    NSURL *url = [[NSBundle mainBundle] URLForResource:@"Sync-One2_Test_1080p_23.98_H.264_AAC_5.1" withExtension:@"mp4"];
    NSString *filepath = [url absoluteString];
	[self.playerView setSpeed:1.0];
    [self.playerView setDataSource:filepath autoPlay:true useHardware:false];
}

- (void) onPrepareSoft2 {
    NSURL *url = [[NSBundle mainBundle] URLForResource:@"output_av1" withExtension:@"mp4"];
    NSString *filepath = [url absoluteString];
    [self.playerView setSpeed:1.0];
    [self.playerView setDataSource:filepath autoPlay:true useHardware:false];
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

//
// Created by liaohailong on 2025/2/9.
//

#ifndef HELLOPLAYER_HELLOPLAYERERROR_HPP
#define HELLOPLAYER_HELLOPLAYERERROR_HPP


/**
 * create by liaohailong
 * 2025/2/9 9:51
 * desc: 
 */
typedef enum
{
    None = 0, // 无错误，表示成功

    NoDataSource = -100, // 资源没有设置
    NoStreamInDataSource = -101, // 资源中没有任何轨道信息
    NoAudioVideoStream = -102, // 资源中没有音视频轨道

    NetworkTimeout = -400, // 网络连接超时

} HelloPlayerError;


const char * hello_get_player_error_msg(HelloPlayerError error);


#endif //HELLOPLAYER_HELLOPLAYERERROR_HPP

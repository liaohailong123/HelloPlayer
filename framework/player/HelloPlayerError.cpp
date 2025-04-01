//
// Created by liaohailong on 2025/2/9.
//

#include "HelloPlayerError.hpp"


const char *hello_get_player_error_msg(HelloPlayerError error)
{
    switch (error)
    {
        case None:
            return "Success";
        case NoDataSource:
            return "No datasource found";
        case NoStreamInDataSource:
            return "Not found any stream in datasource ";
        case NoAudioVideoStream:
            return "Not found audio and video stream in datasource";
        case NetworkTimeout:
            return "Network connect timeout";
    }
}
//
// Created by Administrator on 2020/3/22.
//

#ifndef ANDROIDPLAYER_PLAYSTATUS_H
#define ANDROIDPLAYER_PLAYSTATUS_H

#include <string>

class PlayStatus {

public:
    bool exit;
    bool pause;
    bool seek ;
public:
    PlayStatus();
    ~PlayStatus();
};


#endif //ANDROIDPLAYER_PLAYSTATUS_H

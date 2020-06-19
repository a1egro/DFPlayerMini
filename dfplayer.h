#ifndef DFPLAYERMINI_LIBRARY_H
#define DFPLAYERMINI_LIBRARY_H

#include <climits>
#include <cerrno>
#include <fcntl.h>
#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <cstring>
#include <utility>

#include "df_constants.h"
#include "serial.h"

#define df_callable_t std::function<void(std::shared_ptr<data_package_t>)>

struct data_package_t {
    uint8_t reason;
    uint16_t payload;
};

class DFPlayer {
private:
    bool reply_mark;
    uint8_t buff[10];
    std::chrono::system_clock::time_point last_call;
    df_callable_t cb_functions[3];

    Serial* serial;

    void fill_checksum();

    void send_cmd(uint8_t cmd, uint16_t arg);

    void send_cmd(uint8_t cmd);

    std::shared_ptr<data_package_t> recv_payload();
public:
    explicit DFPlayer(std::string port);

    void cb_caller();

    bool is_playing();

    void play();

    void pause();

    void stop();

    void playpause();

    void next();

    void previous();

    void play_track(uint16_t num);

    void set_track(uint16_t num);

    void vol_up();

    void vol_down();

    void vol_set(uint16_t volume); // TODO: range?

    void setcb_trackfin(df_callable_t func);

    void clearcb_trackfin();

    void setcb_erroc(df_callable_t func);

    void clearcb_erroc();

    // callback function is called with everything the module returns if the reason is not covered by other callbacks
    void setcb_any(df_callable_t func);

    void clearcb_any();

    void set_feedback(bool get_feedback);

    void send_raw(uint8_t command, uint16_t payload);

    void set_playbacktype(DF_PLAYBACK_TYPE type);

    ~DFPlayer();
};

#endif

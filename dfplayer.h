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
#include <termios.h>
#include <unistd.h>
#include <chrono>
#include <functional>

#include "serial.h"

#define __df_callable_t std::function<void(int)>

// maps callback events to according places of functions in callback array
#define DF_CBTF 0  // track finished
#define DF_CBEO 1  // error occurred

// maps error codes to their meaning (primarily for user comfort)
#define DF_NotInitialized       0x1
#define DF_Sleeping             0x2
#define DF_SerialReceiveError   0x3
#define DF_ChecksumIncorrect    0x4
#define DF_TrackOutOfScope      0x5
#define DF_TrackNotFound        0x6
#define DF_InsertionError       0x7
#define DF_SDReadFailure        0x8
#define DF_EnteredSleepMode     0xA

// returned by the receive function
struct data_package {
    uint8_t reason;
    uint16_t payload;
};

class DFPlayer {
private:
    uint8_t buff[10];
    std::chrono::system_clock::time_point last_call;
    __df_callable_t cb_functions[2];

    Serial* serial;

    void fill_checksum();

    void send_cmd(uint8_t cmd, uint16_t arg);

    void send_cmd(uint8_t cmd);

    data_package recv_payload();
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

    void vol_set(uint16_t volume); // range?

    void setcb_tfin(__df_callable_t func);

    void clearcb_tfin();

    void setcb_erroc(__df_callable_t func);

    void clearcb_erroc();

    void send_raw(uint8_t command, uint16_t payload);

    ~DFPlayer();
};

#endif

/*
 * DFPlayer class can access the buffer of the serial receiver via the get_buff method.
 *
 * However the user of the DFPlayer class shall never be able to access the actual values of the receiver buffer.
 * They only will be able to specify callback functions for certain criteria (e.g. first track finished playing or
 * a specific error occurred -- hence callback function without parameter) or for general events (e.g. a track finished
 * or an error occurred -- callback functions have finished track number or error code as argument respectively).
 *
 *
 * TODO:
 *      setup read_async a asynchronous read function  ^
 *      refactor DFPlayer class to use recv_sct struct modifying functions  ^
 *
 *      use callback function of serial  ^
 *      allow user to set callback functions for finished tracks, etc.  ^
 *          ( usage for member functions: std::bind(&Class::callback, this, will throw show error but compiles)
 *            error reporting not tested)
 *
 *      add callback for any data received
 *
 *      data package struct (uint8_t cmd, uint16_t payload)
 *      return from raw function
 *
 *      different playback modes (shuffle, repeat, single repeat)
 *      playback types EQ, ROCK, ...
 *
 *      ad and insertion?
 *
 *      Write documentation:
 *          * setup
 *          * hardware
 *          * ...
 *          * using callback function
 *          * sending raw bytes
 *
 */
//
// Created by flex on 10.06.20.
//

#ifndef DFPLAYERMINI_DF_CONSTANTS_H
#define DFPLAYERMINI_DF_CONSTANTS_H

#define __df_callable_t std::function<void(__recv_message_t)>

// maps callback events to according places of functions in callback array
#define DF_CBTF 0  // track finished
#define DF_CBEO 1  // error occurred
#define DF_CBAN 2  // any message

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

enum DF_PLAYBACK_TYPE {
    NORMAL, POP, ROCK, JAZZ, CLASSIC, BASS
};

#endif //DFPLAYERMINI_DF_CONSTANTS_H

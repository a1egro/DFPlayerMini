//
// Created by flex on 03.04.20.
//

#ifndef DFPLAYERMINI_SERIAL_H
#define DFPLAYERMINI_SERIAL_H

#include <termios.h>
#include <cstdint>
#include <list>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <csignal>
#include <iostream>
#include <functional>


class Serial{
private:
    int fd;
    termios tty_old;

    std::function<void(void)> callback;

    struct recv_sct {
        bool updated;
        uint8_t index;
        uint8_t recv_buff[10];
    } receiver;

public:
    // string specifying the serial port e.g. /dev/ttyUSB0
    // or /dev/serial0 and opens it
    explicit Serial(std::string port);

    // called asynchronously when when SIGIO is received
    // reads 10 bytes and saves them to receiver.recv_buff, sets receiver.updated to true
    void read_async(int signal);

    // writes size bytes (uint8_t) from command on to the serial port
    void write_bytes(uint8_t *buff, unsigned long size);

    // reads size bytes (uint8_t) from the serial port and writes them to the buffer specified by buff
    // furthermore calls callback function if one was specified beforehand
    void read_bytes(uint8_t *buff, unsigned long size);                                 // -- obsolete --

    // returns receiver.updated
    int has_read();

    // sets receiver.updated to false
    void req_read();

    // returns pointer to the 10 byte large receiver.recv_buff
    uint8_t *get_rbuff();

    // set function called when something was received
    void setCallback(std::function<void(void)>);

    // resets the opened port to its initial settings and closes it
    ~Serial();
};


#endif //DFPLAYERMINI_SERIAL_H

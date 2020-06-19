//
// Created by flex on 03.04.20.
//

#include "serial.h"


Serial::Serial(std::string port) : callback(nullptr) {

    fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK); // NOLINT
    if (fd < 0) {
        perror(port.c_str());
        exit(-1);
    }

    // setup receiver struct
    receiver.updated = false;
    receiver.index = 0;
    memset(receiver.recv_buff, 0, 10);

    struct sigaction sacio;
    memset(&sacio, 0, sizeof(struct sigaction));

    /* setup internal callback function for SIGIO - use callback adapter struct*/
    sacio.sa_handler = CallbackContainer::newItem(this);
    //sacio.sa_mask = NULL;
    //sacio.sa_flags = 0;
    //sacio.sa_restorer = nullptr;
    sigaction(SIGIO, &sacio, nullptr);

    /* allow our process to receive SIGIO */
    fcntl(fd, F_SETOWN, getpid());
    /* make file descriptor asynchronous */
    fcntl(fd, F_SETFL, FASYNC);

    // save current configuration
    tcgetattr(fd, &tty_old);

    /* setup new configuration */
    termios tty; // NOLINT
    memset(&tty, 0, sizeof(tty));

    // could also use cfsetispeed/cfsetospeed to set baudrate (9600 baud rate)
    // 8 bit chars, enable receiver, no flow control (CTS/RTS)
    tty.c_cflag = (B9600 | CS8 | CLOCAL | CREAD) & ~CRTSCTS;

    // ignore bytes with wrong parity
    tty.c_iflag = IGNPAR;

    tty.c_oflag = 0;

    // non-canonical input, no echo, ...
    tty.c_lflag = 0;

    tty.c_cc[VTIME] = 0;  // no timeout
    tty.c_cc[VMIN]  = 10; // blocking read until 10 bytes received

    // clean line and activate settings
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW, &tty);
}

void Serial::read_async(int signal) {
    // read to buff and increase index
    read(fd, &receiver.recv_buff[receiver.index++], 1);

    // updated if whole receiver was updated
    if (receiver.index == BUFFER_SIZE) {
        receiver.updated = true;
        receiver.index = 0;

        if (callback) callback();
    }
}

void Serial::write_bytes(uint8_t *buff, size_t size) {
    write(fd, buff, size);
}

void Serial::read_bytes(uint8_t *buff, size_t size) {
    read(fd, buff, size);
}

int Serial::has_read() {
    return receiver.updated;
}

void Serial::req_read() {
    receiver.updated = false;
}

std::shared_ptr<uint8_t> Serial::get_rbuff() {
    // pass default delete for arrays
    std::shared_ptr<uint8_t> rbuff_cp(new uint8_t[BUFFER_SIZE], std::default_delete<uint8_t[]>());

    memcpy(rbuff_cp.get(), receiver.recv_buff, BUFFER_SIZE);

    return rbuff_cp;
}

void Serial::setCallback(std::function<void(void)> func) {
    callback = func;
}

Serial::~Serial() {
    // restore old configuration
    tcsetattr(fd, TCSANOW, &tty_old);

    close(fd);
}

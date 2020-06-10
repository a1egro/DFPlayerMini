#include "dfplayer.h"

#include <iostream>
#include <cstring>
#include <utility>

DFPlayer::DFPlayer(std::string port) : last_call(std::chrono::system_clock::now()), serial(new Serial(std::move(port))) {
    /* setup buffer */
    buff[0] = 0x7e;
    buff[1] = 0xff;
    buff[2] = 0x06;
    buff[9] = 0xef;

    /* setup callback function */
    serial->setCallback(std::bind(&DFPlayer::cb_caller, this));
}

void DFPlayer::fill_checksum() {
    uint16_t sum = 0;
    for(int i = 1; i < 7; i++) {
        sum += buff[i];
    }

    sum = (uint16_t) -sum;

    buff[7] = (uint8_t) (sum>>8); // NOLINT
    buff[8] = (uint8_t) sum;
}

void DFPlayer::send_cmd(uint8_t cmd, uint16_t arg) {
    buff[3] = cmd;
    buff[4] = 0x00;
    buff[5] = (uint8_t) (arg>>8); // NOLINT
    buff[6] = (uint8_t) arg;

    fill_checksum();

    // at least 50 ms between two commands
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = now - last_call;
    if (diff.count() < 0.05) usleep((uint) (0.05 - diff.count())*1000);

    serial->write_bytes(buff, 10);

    last_call = std::chrono::system_clock::now();
}

void DFPlayer::send_cmd(uint8_t cmd) {
    send_cmd(cmd, 0);
}

// returns pointer to 4 byte large payload
__data_package_t DFPlayer::recv_payload() {
    // wait for new data to be read
    serial->req_read();
    while (!serial->has_read());                 // TODO: TIMEOUT (see: tty.c_cc[VTIME])

    struct __data_package_t data;
    uint8_t *rbuff = serial->get_rbuff();

    // 4th byte specifies reason
    data.reason = rbuff[3];
    // 6th and 7th are data bytes
    data.payload = (rbuff[5])<<8 | rbuff[6];

    return data;
}

void DFPlayer::cb_caller() {
    uint8_t *rbuff = serial->get_rbuff();

    __recv_message_t msg = static_cast<__recv_message_t> (malloc(PACKAGE_SIZE));
    // 4th byte specifies reason
    msg->reason = rbuff[3];
    // 6th and 7th are data bytes
    msg->payload = (rbuff[5])<<8 | rbuff[6];

    __df_callable_t callable = nullptr;

    switch(msg->reason) {
        // track on sd card finished playing
        case 0x3d:
            callable = cb_functions[DF_CBTF];
            break;
        // error code
        case 0x40:
            callable = cb_functions[DF_CBEO];
            break;
        default:
            callable = cb_functions[DF_CBAN];
            break;
    }

    // call function if one is assigned
    if (callable) callable(msg);
}

bool DFPlayer::is_playing() {
    send_cmd(0x42); // query current state

    auto ans = recv_payload();

    bool playing = (ans.reason == 0x42 && ans.payload == 0x100);

    return playing;
}

void DFPlayer::play() {
    send_cmd(0x0d);
}

void DFPlayer::pause() {
    send_cmd(0x0e);
}

void DFPlayer::stop() {
    send_cmd(0x16);
}

void DFPlayer::playpause() {
    if (is_playing()) pause();
    else play();
}

void DFPlayer::next() {
    send_cmd(0x01);
}

void DFPlayer::previous() {
    send_cmd(0x02);
}

void DFPlayer::play_track(uint16_t num) {
    send_cmd(0x03, num);
}

void DFPlayer::set_track(uint16_t num) {
    send_cmd(0x03, num);
    pause();
}

void DFPlayer::vol_up() {
    send_cmd(0x04);
}

void DFPlayer::vol_down() {
    send_cmd(0x05);
}

void DFPlayer::vol_set(uint16_t volume) {
    send_cmd(0x06, volume);
}

void DFPlayer::setcb_trackfin(__df_callable_t func) {
    cb_functions[DF_CBTF] = func;
}

void DFPlayer::clearcb_trackfin() {
    cb_functions[DF_CBTF] = nullptr;
}

void DFPlayer::setcb_erroc(__df_callable_t func) {
    cb_functions[DF_CBEO] = func;
}

void DFPlayer::clearcb_erroc() {
    cb_functions[DF_CBEO] = nullptr;
}

void DFPlayer::setcb_any(__df_callable_t func) {
    cb_functions[DF_CBAN] = func;
}

void DFPlayer::clearcb_any() {
    cb_functions[DF_CBAN] = nullptr;
}

void DFPlayer::send_raw(uint8_t command, uint16_t payload) {
    send_cmd(command, payload);
}

DFPlayer::~DFPlayer() {
    delete serial;
}


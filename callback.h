//
// Created by flex on 07.04.20.
//

#ifndef DFPLAYERMINI_CALLBACK_H
#define DFPLAYERMINI_CALLBACK_H

#include <map>
#include "serial.h"


class CallbackContainer {
public:
    // looks for a unused static function
    // if one is found it is returned and the Serial instance is mapped to the corresponding template context
    // returns nullptr if not successful
    static __sighandler_t newItem(Serial *instance);

private:
    // just that the container can have a lost of callback items
    class CallbackItemBase {
    };

    template <int context> class CallbackItem : public CallbackItemBase {
    public:
        // calls the read_async function on the instance of Serial class mapped in CallbackContainer::references
        static void generatedCallback(int arg);
    };

    // maps instances of Serial to the context of the CallbackItem class whose static function can be used as C callback
    static std::map<int, Serial*> references;

    // pregenerated CallbackItem classes since templating only works with constants
    static constexpr __sighandler_t pregeneratedFunction[] = {
            CallbackItem<0x00>::generatedCallback,
            CallbackItem<0x01>::generatedCallback,
            CallbackItem<0x02>::generatedCallback,
            CallbackItem<0x03>::generatedCallback,
            CallbackItem<0x04>::generatedCallback,
            CallbackItem<0x05>::generatedCallback,
            CallbackItem<0x06>::generatedCallback,
            CallbackItem<0x07>::generatedCallback
    };

};

#endif //DFPLAYERMINI_CALLBACK_H

//
// Created by flex on 07.04.20.
//

#include "callback.h"

// define static members
constexpr __sighandler_t CallbackContainer::pregeneratedFunction[8];
std::map<int, Serial*> CallbackContainer::references;

__sighandler_t CallbackContainer::newItem(Serial *instance) {
    // look for unused static function
    int i = 0, i_max = sizeof(pregeneratedFunction)/ sizeof(pregeneratedFunction[0]);
    while (references.find(i) != references.end()) {
        i++;
        if (i == i_max)
            return nullptr;
    }

    references.insert(std::pair<int, Serial*>(i, instance));
    return pregeneratedFunction[i];
}

template<int context>
void CallbackContainer::CallbackItem<context>::generatedCallback(int arg) {
    auto it = CallbackContainer::references.find(context);
    if (it != CallbackContainer::references.end())
        it->second->read_async(arg);
}

#pragma once

class OmMouseEventState {
public:
    template <class T>
    static void Print(T* target)
    {
        printf("controlKey_(%d), altKey_(%d), shiftKey_(%d), leftMouseButton_(%d), "
               "rightMouseButton_(%d), ",
               target->controlKey_,
               target->altKey_,
               target->shiftKey_,
               target->leftMouseButton_,
               target->rightMouseButton_);

        std::cout << target->tool_;
        std::cout << "click point: " << target->dataClickPoint_;
        std::cout << "\n";
    }
};


#include "Exception.h"

void Exception(const std::string &errorString) {
    throw std::logic_error(errorString);
}

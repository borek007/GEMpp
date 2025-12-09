#ifndef GEMPP_EXCEPTION_H
#define GEMPP_EXCEPTION_H

#include <stdexcept>
#include <string>
#include "Portability.h"

/**
 * @brief Throws an Exception with a custom error message
 * @param errorString the error message
 */
void DLL_EXPORT Exception(const std::string &errorString);

#endif /* GEMPP_EXCEPTION_H */

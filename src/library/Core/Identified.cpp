#include "Identified.h"

Identified::Identified() {
    id_ = "";
}

Identified::Identified(const Identified &other) {
    id_ = other.getID();
}

Identified::Identified(const std::string &id) : id_(id) {}

Identified::~Identified() {}

const std::string &Identified::getID() const {
    return id_;
}

void Identified::setID(const std::string &id) {
    id_ = id;
}

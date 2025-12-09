#include "Variable.h"

Variable::Variable(const std::string &id, Type type, int lowerBound, int upperBound) : Identified(id), type_(type) {
    activate(lowerBound, upperBound);
    columns_ = std::unordered_map<std::string, double>();
    value_ = lowerBound_;
}

Variable::~Variable() {}

Variable::Type Variable::getType() {
    return type_;
}

void Variable::setType(Type type) {
    type_ = type;
}

void Variable::setValue(int val) {
    if(val < lowerBound_ || val > upperBound_) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Illegal value assignment for variable %s : %d is out of bounds [%d;%d]",
                     id_.c_str(), val, lowerBound_, upperBound_);
        Exception(std::string(buffer));
    }
    value_ = val;
}

int Variable::eval() {
    return value_;
}

bool Variable::isActive() {
    return (lowerBound_ != 0 || upperBound_ != 0);
}

int Variable::getLowerBound() {
    return lowerBound_;
}

void Variable::setLowerBound(int lowerBound) {
    if(lowerBound > upperBound_) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Illegal lower bound assignment for variable %s : LB=%d > UB=%d",
                     id_.c_str(), lowerBound, upperBound_);
        Exception(std::string(buffer));
    }
    if(type_ == BINARY && lowerBound != 0) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Illegal lower bound assignment for variable %s : LB=%d must be 0",
                     id_.c_str(), lowerBound);
        Exception(std::string(buffer));
    }
    lowerBound_ = lowerBound;
}

int Variable::getUpperBound() {
    return upperBound_;
}

void Variable::setUpperBound(int upperBound) {
    if(upperBound < lowerBound_) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Illegal upper bound assignment for variable %s : UB=%d < LB=%d",
                     id_.c_str(), upperBound, lowerBound_);
        Exception(std::string(buffer));
    }
    if(type_ == BINARY && upperBound != 1 && upperBound != 0) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Illegal upper bound assignment for variable %s : UB=%d must be 0 or 1",
                     id_.c_str(), upperBound);
        Exception(std::string(buffer));
    }
    upperBound_ = upperBound;
}

void Variable::activate(int lowerBound, int upperBound) {
    switch(type_) {
        case BOUNDED:
            lowerBound_ = lowerBound;
            upperBound_ = upperBound;
            break;
        case CONTINUOUS:
            lowerBound_ = lowerBound;
            upperBound_ = upperBound;
            break;
        case BINARY:
            lowerBound_ = 0;
            upperBound_ = 1;
            break;
    }
    if(lowerBound_ > upperBound_) {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer), "Illegal bounds for variable %s : LB=%d > UB=%d",
                     id_.c_str(), lowerBound_, upperBound_);
        Exception(std::string(buffer));
    }
}

void Variable::deactivate() {
    lowerBound_ = 0;
    upperBound_ = 0;
}

void Variable::addColumn(const std::string &id, double d) {
    columns_[id] = d;
}

std::unordered_map<std::string, double> &Variable::getColumns() {
    return columns_;
}

double Variable::getColumn(const std::string &id) {
    return columns_[id];
}

void Variable::print(Printer *p) {
    p->dump(id_);
}

Variable::operator Term() const {
    return std::make_pair((Variable *)this, 1.0);
}

bool operator==(Quad q1, Quad q2) {
    return (q1.first == q2.first && q1.second == q2.second) || (q1.first == q2.second && q1.second == q2.first);
}

bool isActive(QuadTerm t) {
    return (t.first.first->isActive() && t.first.second->isActive());
}

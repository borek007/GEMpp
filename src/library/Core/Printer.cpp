#include "Printer.h"

Printer::Printer(int indentWidth) : content_(""), indentWidth_(indentWidth), indentLevel_(0) {}

Printer::~Printer() {}

const std::string &Printer::getContent() const {
    return content_;
}

void Printer::setContent(const std::string &content) {
    content_ = content;
}

int Printer::getIndentWidth() {
    return indentWidth_;
}

void Printer::setIndentWidth(int indentWidth) {
    indentWidth_ = indentWidth;
}

void Printer::dump(const std::string &s) {
    for(int i=0; i<indentLevel_*indentWidth_; ++i) {
        content_ += " ";
    }
    content_ += s;
    newLine();
}

void Printer::show() {
    std::cout << content_;
}

void Printer::indent() {
    ++indentLevel_;
}

void Printer::unindent() {
    --indentLevel_;
}

void Printer::newLine() {
    content_ += "\n";
}

void Printer::reset() {
    content_.clear();
}

std::string Printer::capitalize(const std::string &s) {
    std::string result;
    std::string word;
    bool inWord = false;

    for (char c : s) {
        if (c == ' ' || c == '\t' || c == '\n') {
            if (inWord) {
                result += capitalizeWord(word);
                word.clear();
                inWord = false;
            }
            result += c;
        } else {
            word += c;
            inWord = true;
        }
    }
    if (inWord) {
        result += capitalizeWord(word);
    }

    return result;
}

std::string Printer::capitalizeWord(const std::string &w) {
    if (w.empty()) return w;
    std::string result = w;
    if (result[0] >= 'a' && result[0] <= 'z') {
        result[0] = result[0] - 'a' + 'A';
    }
    for (size_t i = 1; i < result.size(); ++i) {
        if (result[i] >= 'A' && result[i] <= 'Z') {
            result[i] = result[i] - 'A' + 'a';
        }
    }
    return result;
}

#include "FileUtils.h"
#include <sstream>
#include <algorithm>

FileUtils::FileUtils() {}

FileUtils::~FileUtils() {}

std::string FileUtils::load(const std::string &filename) {
    checkExists(filename);
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        Exception("Cannot open file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
}

void FileUtils::save(IPrintable *ip, const std::string &filename, bool append) {
    Printer p;
    ip->print(&p);
    save(p.getContent(), filename, append);
}

void FileUtils::save(const std::string &s, const std::string &filename, bool append) {
    std::ofstream file(filename, std::ios::out | std::ios::binary | (append ? std::ios::app : std::ios::trunc));
    if (!file.is_open()) {
        Exception("Cannot open file for writing: " + filename);
    }
    file << s;
    file.close();
}

bool FileUtils::remove(const std::string &filename) {
    return std::remove(filename.c_str()) == 0;
}

std::string FileUtils::getExtension(const std::string &filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filename.substr(dotPos + 1);
    }
    return "";
}

void FileUtils::checkExtension(const std::string &filename, const std::string &extension) {
    std::string ext = getExtension(filename);
    std::string extLower = ext;
    std::string expectedLower = extension;
    std::transform(extLower.begin(), extLower.end(), extLower.begin(), ::tolower);
    std::transform(expectedLower.begin(), expectedLower.end(), expectedLower.begin(), ::tolower);

    if (extLower != expectedLower) {
        Exception("The file \"" + filename + "\" is not a *." + extension + " file.");
    }
}

std::string FileUtils::changeExtension(const std::string &filename, const std::string &extension) {
    return removeExtension(filename) + "." + extension;
}

std::string FileUtils::removeExtension(const std::string &filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos) {
        return filename.substr(0, dotPos);
    }
    return filename;
}

bool FileUtils::exists(const std::string &filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

void FileUtils::checkExists(const std::string &filename) {
    if (!exists(filename)) {
        Exception("The file \"" + filename + "\" does not exist.");
    }
}

bool FileUtils::isValid(const std::string &filename) {
    return !filename.empty();
}

void FileUtils::checkValid(const std::string &filename) {
    if (!isValid(filename)) {
        Exception("The given filename is empty.");
    }
}

std::string FileUtils::slashed(const std::string &parent, const std::string &path) {
    if (isAbsolute(path) || parent == ".") {
        return path;
    }
    if (parent.empty()) {
        return path;
    }
    if (parent.back() == '/' || parent.back() == '\\') {
        return parent + path;
    }
    return parent + "/" + path;
}

std::string FileUtils::path(const std::string &filepath) {
    size_t slashPos = filepath.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        return filepath.substr(0, slashPos);
    }
    return ".";
}

std::string FileUtils::filename(const std::string &filepath) {
    size_t slashPos = filepath.find_last_of("/\\");
    if (slashPos != std::string::npos) {
        return filepath.substr(slashPos + 1);
    }
    return filepath;
}

bool FileUtils::isAbsolute(const std::string &path) {
#ifdef _WIN32
    return path.size() >= 2 && ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':';
#else
    return !path.empty() && path[0] == '/';
#endif
}

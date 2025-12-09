#ifndef GEMPP_FILEUTILS_H
#define GEMPP_FILEUTILS_H

#include <string>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include "Exception.h"
#include "IPrintable.h"

/**
 * @brief The FileUtils class is a utility that provides methods to load, save and remove files.
 * @author J.Lerouge <julien.lerouge@litislab.fr>
 */
class DLL_EXPORT FileUtils {
    public:
        /**
         * @brief Constructs a new FileUtils utility.
         */
        FileUtils();
        /**
         * @brief Destructs a FileUtils utility.
         */
        ~FileUtils();

        /**
         * @brief Loads a file and returns its content.
         * @param filename the path to the file
         * @return the content of the file
         */
        static std::string load(const std::string &filename);

        /**
         * @brief Serializes an IPrintable to a file.
         * @param ip the IPrintable object to serialize
         * @param filename the path to the file
         * @param append appends the content to the already existing file (if any) or overwrite it
         */
        static void save(IPrintable *ip, const std::string &filename, bool append = false);

        /**
         * @brief Serializes a std::string to a file.
         * @param s the std::string to serialize
         * @param filename the path to the file
         * @param append appends the content to the already existing file (if any) or overwrite it
         */
        static void save(const std::string &s, const std::string &filename, bool append = false);

        /**
         * @brief Removes a file.
         * @param filename the path to the file
         * @return true if successful, false otherwise
         */
        static bool remove(const std::string &filename);

        /**
         * @brief Returns the extension of a filename.
         * @param filename the path to the file
         */
        static std::string getExtension(const std::string &filename);

        /**
         * @brief Checks the extension of a filename.
         * Throws an exception if the extension does not correspond.
         * @param filename the path to the file
         * @param extension the file extension to check against
         */
        static void checkExtension(const std::string &filename, const std::string &extension);

        /**
         * @brief Changes the extension of a filename.
         * @param filename the filename
         * @param extension the extension
         */
        static std::string changeExtension(const std::string &filename, const std::string &extension);

        /**
         * @brief Removes the extension of a filename.
         * @param filename the filename
         */
        static std::string removeExtension(const std::string &filename);

        /**
         * @brief Checks whether the file exists.
         * Throws an exception if the file was not found.
         * @param filename the path to the file
         */
        static void checkExists(const std::string &filename);

        /**
         * @brief Checks whether the filename is valid.
         * Throws an exception if the filename is empty or null.
         * @param filename the path to the file
         */
        static void checkValid(const std::string &filename);

        /**
         * @brief Checks whether the file exists.
         * @param filename the path to the file
         * @return a boolean indicating the existence of the file
         */
        static bool exists(const std::string &filename);

        /**
         * @brief Checks whether the filename is valid.
         * @param filename the path to the file
         * @return a boolean indicating the validity of the filename
         */
        static bool isValid(const std::string &filename);

        /**
         * @brief Combines a parent path and a relative child path.
         * @param parent the parent path
         * @param path the relative child path
         * @return the complete child path, including the parent path
         */
        static std::string slashed(const std::string &parent, const std::string &path);

        /**
         * @brief Returns the path to the directory containing the file.
         * @param filepath the path to the file
         * @return the path to the directory containing the file
         */
        static std::string path(const std::string &filepath);

        /**
         * @brief Returns the name of the file, removing its absolute or relative path.
         * @param filepath the path to the file
         * @return the name of the file
         */
        static std::string filename(const std::string &filepath);

        /**
         * @brief Checks whether the path is absolute.
         * @param path the path to check
         * @return a boolean indicating whether the path is absolute or not
         */
        static bool isAbsolute(const std::string &path);
};

#endif /* GEMPP_FILEUTILS_H */

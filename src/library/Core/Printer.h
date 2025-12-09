#ifndef GEMPP_PRINTER_H
#define GEMPP_PRINTER_H

#include <string>
#include <iostream>
#include "Constants.h"

/**
 * @brief The Printer class is a utility that can print an IPrintable object.
 *
 * It can store QString text. Indentation is managed
 * with a fixed number of spaces per indentation level.
 *
 * It is possible to show the content of the Printer on standard
 * output (show()), or to retrieve it for a later use (getContent()).
 *
 * @see IPrintable
 * @see ISaveable
 * @author J.Lerouge <julien.lerouge@litislab.fr>
 */
class DLL_EXPORT Printer {
    public:
        /**
         * @brief Constructs a new Printer object.
         * @param indentWidth the number of spaces per indentation level
         */
        Printer(int indentWidth = 4);
        /**
         * @brief Destructs a Printer object.
         */
        ~Printer();

        /**
         * @brief Retrieves the content stored in the Printer.
         * @return the value of Printer::content_
         */
        const std::string &getContent() const;

        /**
         * @brief Sets the content of the Printer.
         * @param content the content to set
         */
        void setContent(const std::string &content);

        /**
         * @brief Gets the indentation width (in number of spaces) of the Printer.
         * @return the value of Printer::indentWidth_
         */
        int getIndentWidth();

        /**
         * @brief Sets the indentation width (in number of spaces) of the Printer.
         * @param indentWidth the indentation width to set
         */
        void setIndentWidth(int indentWidth);

        /**
         * @brief Shows the content of a Printer on standard output.
         */
        void show();

        /**
         * @brief Stores a std::string text in a Printer, appends a new line at the end.
         */
        void dump(const std::string &s);

        /**
         * @brief Adds one indentation level.
         */
        void indent();

        /**
         * @brief Removes one indentation level.
         */
        void unindent();

        /**
         * @brief Appends a new line to the content_.
         */
        void newLine();

        /**
         * @brief Clears the content of a Printer.
         */
        void reset();

        /**
         * @brief Capitalizes a string, by making each word's first
         * character upper case and the other characters lower case.
         */
        static std::string capitalize(const std::string &s);

        /**
         * @brief Capitalizes a word, by making its first
         * character upper case and the other characters lower case.
         */
        static std::string capitalizeWord(const std::string &w);

    private:
        /**
         * @brief The content of the Printer.
         */
        std::string content_;

        /**
         * @brief The number of spaces used per indentation level.
         */
        int indentWidth_;

        /**
         * @brief The current indentation level of the Printer.
         */
        int indentLevel_;
};

#endif /* GEMPP_PRINTER_H */

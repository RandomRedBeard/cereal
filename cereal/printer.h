#ifndef CEREAL_JSON_PRINTER_H
#define CEREAL_JSON_PRINTER_H

#include <cereal/lib_maker.h>

#include <cereal/json.h>

namespace cereal {
    namespace json {

        /**
         * Util class for pretty printing
         */
        class DLL_PUBLIC Printer {
			// Indent count can be used for pretty-fication
			int indent;

			// Tab and newline and used for aesthetics in print_newline and print_indent
			const char* tab;
			const char* newline;

			int print(char*, int, Value*, int, bool);

			int print(char*, int, Tree*, int);

			int print(char*, int, List*, int);

			int print(char*, int, const char*);

			int print_n(char*, int, const char*);

			int print(char*, int, bool);

			int print_indent(char*, int, int);

			int print_newline(char*, int);
		public:

			/**
			 * Writes Value to buffer with given character-sets
			 */
			int print(char*, int, Value*);

			/**
			 * Writes print to buffer and calls puts
			 */
			int print(Value*);
        
			/**
			 * Instatiates default printer
			 * Calls once to get bytes needed to write
			 * Allocates n + 1 bytes
			 * Calls print again
			 * puts(buffer);
			 */
			static int dprint(Value*);

			Printer(int indent = 1, const char* tab = "\t", const char* newline = "\n" );
		};
    }
}

#endif
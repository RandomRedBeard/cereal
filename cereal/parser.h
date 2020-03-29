#ifndef CEREAL_JSON_PARSER_H
#define CEREAL_JSON_PARSER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <cereal/lib_maker.h>

#include <cereal/json.h>

namespace cereal {
    namespace json {
		/**
		 * String to Value* class
		 * TODO: Behavioral specifications
		 */
        class DLL_PUBLIC Parser {
            // Returns number of bytes to any char in expect or -1 for failure
            int seek_until(const char*, const char*, const char*, bool expect_digit = false);
        
            /**
             * Method for reading cereal::json::Type::STRING
             * When dest is null, number of bytes to be written to dest is returned
             * otherwise, number of bytes read from src returned
             */
            int read_string(const char*, char*);

			/**
			 * Seeks to any valid json value
			 * Handles parsing of some json value
			 * Allocates memory to value and returns bytes read on sucess
			 * Returns -1 on failure
			 */
			int parse_value(const char*, Value**);

            /**
             * Tree specific parsing. State scoped to modular parse calls
             * Returns number of bytes read or -1 on failure
             */
            int parse_object(const char*, Tree*);

            /**
             * List specific parsing.
             * Returns number of bytes read or -1 on failure
             */
            int parse_list(const char*, List*);

            /**
             * Value containing string
             * Memory allocated in method for constructor use
             * Returns bytes read
             */
            int parse_string(const char*, Value**);

            /**
             * Value containing number
             * Memory allocated in method for constructor
             * Returns bytes read
             */
            int parse_number(const char*, Value**);

            /**
             * Value containing boolean
             */
            int parse_bool(const char*, Value**);

            /**
             * Value containing null type
             */
            int parse_null(const char*, Value**);
        public:
            /**
			 * Json specific whitespace
			 * Allow others to see what WHITESPACE means
			 */
			const static char* WHITESPACE;

            /**
             * Allocates heap memory to *value for writing
             * Returns bytes read
             */
            int parse(const char*, Value**);
        };
    }
}

#endif
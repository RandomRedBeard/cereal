#ifndef CEREAL_JSON_H
#define CEREAL_JSON_H

#include <vector>
#include <map>
#include <string>

#include <cereal/lib_maker.h>
#include <cereal/cmp_str.h>

namespace cereal {

    namespace json {

		/**
		 * Json Type
		 * For Identification
		 */
        enum DLL_PUBLIC Type {
            STRING,
            NUMBER,
            OBJECT,
            LIST,
            BOOL
        };

		/**
		 * Base class for all json values
		 * Note: Numbers are stored as char* and converted on request
		 * Note: When type is NUMBER or STRING, free is called on value
		 */
        class DLL_PUBLIC Value {
            Type type;
            void* value;
        public:
			// Null object
			Value();

            Value(Type, void*);

            virtual ~Value();

            Type get_type();

            // Allow casting for ease of use
            template<typename T=void*>
            T get_value() {
                return (T) value;
            }

			/**
			 * nullptr check
			 */
			virtual bool is_null();
        };

        // Json Array
        class DLL_PUBLIC List : public Value {
        public:
            // Store real type of Value::value
            using T = std::vector<Value*>;
            
            List();

            virtual ~List();

            /**
			 * Add value to list
			 * Null Value* will not be added to list
			 */
			void add_value(Value*);

            // Index based get
            Value* find_value(unsigned int);

			/**
			 * Make List iterable
			 */
			T::iterator begin();

			T::iterator end();

			size_t size();
        };

        // Json Object
        class DLL_PUBLIC Tree : public Value {
        public:
            // Store real type of Value::value
            using T = std::map<char*, Value*, cmp_str>;
    
            Tree();

            virtual ~Tree();

            // Wrapper for finding values on map
            Value* find_value(char*);

            // Makes a copy of const key
            Value* find_value(const char*);

            // C-style strings used
            Value* find_value(std::string);

            /**
			 * Make copy of input key to place in map container
             * Moves value to container, NOT COPY
			 * Returns 0 on sucess
			 */
			int put_value(const char*, Value*);

			/**
			 * Make Tree iterable
			 */
			T::iterator begin();

			T::iterator end();

			size_t size();
        };

    }

}

#endif
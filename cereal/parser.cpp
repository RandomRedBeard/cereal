#include "parser.h"

using namespace cereal::json;

#if defined BUILDING_DLL || defined DEV
const char* Parser::WHITESPACE = " \n\t\r";
#endif

int Parser::seek_until(const char* buffer, const char* expect, const char* allowed, bool expect_digit) {
    int i = 0;
    char c;
    while (c = *(buffer + i)) {
        if (strchr(expect, c)) {
            return i;
        }

        if (expect_digit && isdigit(c)) {
            return i;
        }

        if (!strchr(allowed, c)) {
			// Aesthetic
            if (allowed == WHITESPACE) {
                allowed = "WHITESPACE";
            }
            printf("%s", buffer);
            printf("Expected %s Allowed %s Got %c\n", expect, allowed, *(buffer + i));
            return -1;
        }

        i++;
    }

    return -1;
}

int Parser::read_string(const char* src, char* dest) {
    // Increment past "
    int i = 1;

	// Dest index
    int j = 0;
    
	int slash_count = 0;
    char c;

    while(c = *(src + i)) {
        if (c == '\\') {
            slash_count++;

            // Eat first slash
            if (slash_count == 1) {
                i++;
                continue;
            }
        }

        if (c == '"' && slash_count > 0) {
            slash_count--;
        } else if (c == '"' && slash_count == 0) {
            break;
        }

        if (c != '\\' && c != '"') {
            slash_count = 0;
        }

        if (dest) {
            *(dest + j) = *(src + i);
        }

        j++;
        i++;
    }

    if (dest) {
        // null terminate string
        *(dest + j) = '\0';
        return i;
    }

    return j;
}

int Parser::parse_value(const char* buffer, Value** value) {
	int j = 0;
	// What value am I looking at
	switch (*buffer) {
	case '{':
		// Allocate tree
		*value = new Tree();
		j = parse_object(buffer, (Tree*)*value);
		break;
	case '[':
		*value = new List();
		j = parse_list(buffer, (List*)*value);
		break;
	case '"':
		j = parse_string(buffer, value);
		break;
	case 't':
	case 'f':
		j = parse_bool(buffer, value);
		break;
	case 'n':
		j = parse_null(buffer, value);
		break;
	default:
		j = parse_number(buffer, value);
	}

	// Fall through
	if (!*value) {
		return -1;
	}

	// Parse failed, value is not null
	if (j < 0) {
		delete *value;
		return -1;
	}

	return j;
}

int Parser::parse_object(const char* buffer, Tree* tree) {
    // *buffer is {
    int i = 1;
    int state = 0;

    char* key = nullptr;

    while (*(buffer + i)) {
        // Find key or object end
        int j = seek_until(buffer + i, "\"}", WHITESPACE);

        if (j < 0) {
            state = -1;
            break;
        }

        i += j;

        // Valid object end
        if (*(buffer + i) == '}') {
            // Move past }
            i++;
            break;
        }

        // Read key
        int len = read_string(buffer + i, nullptr);
        key = (char*)malloc(len + 1);
        j = read_string(buffer + i, key);

        // Let key get cleaned up after
        if (j < 0) {
            state = -1;
            break;
        }

        // Move index after trailing "
        i += j + 1;

        // Seek to seperator
        j = seek_until(buffer + i, ":", WHITESPACE);

        // Failed to find seperator
        if (j < 0) {
            state = -1;
            break;
        }

        // Move past seperator
        i += j + 1;

        // Seek to value
        j = seek_until(buffer + i, "\"{[tfn", WHITESPACE, true);

        // Bad seek
        if (j < 0) {
            state = -1;
            break;
        }

        // Move index
        i += j;

        // Value to be recieved
        Value* value = nullptr;

		//Parse value from seek
		j = parse_value(buffer + i, &value);

		if (j < 0) {
			state = -1;
			break;
		}

        // Add value to tree
        // Unknown reason for insert failing
        if (tree->put_value(key, value) < 0) {
            state = -1;
            break;
        }
        
        // Clean up key
        free(key);
        key = nullptr;

        // Move index beyond read
        i += j;

        // Seek to comma or object end
        j = seek_until(buffer + i, ",}", WHITESPACE);

        if (j < 0) {
            state = -1;
            break;
        }

        i += j;

        // Graceful Object end
        if (*(buffer + i) == '}') {
            // Move beyond }
            i++;
            break;
        }

        // Move past comma
        i++;
    }

    // Signifies some error
    if (key) {
        free(key);
    }

    if (state == -1) {
        return -1;
    }

    return i;
}

int Parser::parse_list(const char* buffer, List* list) {
    // *buffer is [
    int i = 1;
    int state = 0;

    while (*(buffer + i)) {
        // Seek to value
        // TODO: number
        int j = seek_until(buffer + i, "\"{[tfn]", WHITESPACE, true);

        // Bad seek
        if (j < 0) {
            state = -1;
            break;
        }

        // Move index
        i += j;

        // Check for graceful exit
        if (*(buffer + i) == ']') {
            // Move beyond ]
            i++;
            break;
        }

        // Value to be recieved
        Value* value = nullptr;

        // Parse value from seek
		j = parse_value(buffer + i, &value);

		if (j < 0) {
			state = -1;
			break;
		}

        // Add value to list
        list->add_value(value);

        // Move index
        i += j;

         // Seek to comma or object end
        j = seek_until(buffer + i, ",]", WHITESPACE);

        if (j < 0) {
            state = -1;
            break;
        }

        i += j;

        // Graceful List end
        if (*(buffer + i) == ']') {
            // Move beyond }
            i++;
            break;
        }

        // Move past comma
        i++;
    }

    if (state == -1) {
        return -1;
    }

    return i;
}

int Parser::parse_string(const char* buffer, Value** value) {
    // Buffer will be on "
    int len = read_string(buffer, nullptr);
    char* v = (char*)malloc(len + 1);
    int i = read_string(buffer, v);

    *value = new Value(STRING, v);

    // Move beyond trailing "
    return i + 1;
}

int Parser::parse_number(const char* buffer, Value** value) {
    // Buffer will be some digit or period
    int i = 0;
    int decimal_count = 0;
    int c;

    // First read for length
    while (c = *(buffer + i)) {
        if (c == '.' && decimal_count == 0) {
            decimal_count++;
        } else if (c == '.') {
            return -1;
        }

        // Found non-digit
        if (!isdigit(c) && c != '.') {
            break;
        }

        i++;
    }

    // i bytes are valid
    char* dest = (char*)malloc(i + 1);
	if (!dest) {
		return -1;
	}

	memset(dest, 0, i + 1);
    memcpy(dest, buffer, i);
    *(dest + i) = 0;

    *value = new Value(NUMBER, dest);

    return i;
}

int Parser::parse_bool(const char* buffer, Value** value) {
    // Buffer is either t/f
    const char* seeking = nullptr;
    if (*buffer == 't') {
        seeking = "true";
    } else {
        seeking = "false";
    }

    int seek_len = strlen(seeking);

    for (int i = 0; i < seek_len; i++) {
        // Watch for that \0
        if (!*(buffer + i)) {
            return -1;
        }

        // Compare values
        if (*(buffer + i) != *(seeking + i)) {
            return -1;
        }
    }

    // Cheeky bool check
    bool rval = *seeking == 't';

    *value = new Value(BOOL, (void*) rval);

    return seek_len;
}

int Parser::parse_null(const char* buffer, Value** value) {
    const char* seeking = "null";
    int seek_len = strlen(seeking);

    for (int i = 0; i < seek_len; i++) {
        if (!*(buffer + i)) {
            return -1;
        }

        if (*(buffer + i) != *(seeking + i)) {
            return -1;
        }
    }

    *value = new Value(OBJECT, nullptr);

    return seek_len;
}

int Parser::parse(const char* buffer, Value** value) {
	// Seek to some json start
	int i = seek_until(buffer, "{[", WHITESPACE);

	// No start was found
	if (i < 0) {
		return -1;
	}

	// Bytes read from parse into j
	int j = -1;

	// Either an object or a list
	switch (*(buffer + i)) {
	case '{':
		*value = new Tree();
		j = parse_object(buffer + i, (Tree*)* value);
		break;
	case '[':
		*value = new List();
		j = parse_list(buffer + i, (List*)* value);
		break;
	}

	// Deallocate value
	if (j < 0) {
		delete* value;
		*value = nullptr;
		return j;
	}

	// Bytes read
	return i + j;
}
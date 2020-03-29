#include "printer.h"

using namespace cereal::json;

#define LEN(len, i) len == 0? 0 : len - i

int Printer::print(char* buffer, int len, Value* value, int depth, bool comma) {
	int i = 0;
	switch (value->get_type()) {
	case NUMBER:
		i = print_n(buffer, len, value->get_value<const char*>());
		break;
	case STRING:
		i = print(buffer, len, value->get_value<const char*>());
		break;
	case BOOL:
		i = print(buffer, len, value->get_value<bool>());
		break;
	case OBJECT:
		i = print(buffer, len, (Tree*)value, depth);
		break;
	case LIST:
		i = print(buffer, len, (List*)value, depth);
	}
	if (comma) {
		i += snprintf(buffer + i, LEN(len, i), ",");
	}

	return i + print_newline(buffer + i, LEN(len, i));
}

int Printer::print(char* buffer, int len, Tree* tree, int depth) {
	if (tree->is_null()) {
		return snprintf(buffer, len, "null");
	}

	if (tree->size() == 0) {
		return snprintf(buffer, len, "{}");
	}

	int i = snprintf(buffer, len, "{");

	i += print_newline(buffer + i, LEN(len, i));

	depth++;

	int sz = tree->size();
	int count = 0;

	for (std::pair<char*, Value*> p : *tree) {
		i += print_indent(buffer + i, LEN(len, i), depth);
		i += snprintf(buffer + i, LEN(len, i), "\"%s\": ", p.first);

		i += print(buffer + i, LEN(len, i), p.second, depth, count != sz - 1);

		count++;
	}

	depth--;

	i += print_indent(buffer + i, LEN(len, i), depth);

	i += snprintf(buffer + i, LEN(len, i), "}");

	return i;
}

int Printer::print(char* buffer, int len, List* list, int depth) {
	if (list->is_null()) {
		return snprintf(buffer, len, "null");
	}

	if (list->size() == 0) {
		return snprintf(buffer, len, "[]");
	}

	int i = snprintf(buffer, len, "[");

	i += print_newline(buffer + i, LEN(len, i));

	depth++;

	int sz = list->size();
	int count = 0;

	for (Value* value : *list) {
		i += print_indent(buffer + i, LEN(len, i), depth);
		i += print(buffer + i, LEN(len, i), value, depth, count != sz - 1);

		count++;
	}

	depth--;

	i += print_indent(buffer + i, LEN(len, i), depth);

	i += snprintf(buffer + i, LEN(len, i), "]");

	return i;
}

int Printer::print(char* buffer, int len, const char* str) {
	int sz = strlen(str);
	int i = snprintf(buffer, LEN(len, 0), "\"");
	for (int j = 0; j < sz; j++) {
		if (*(str + j) == '"') {
			i += snprintf(buffer + i, LEN(len, i), "\\");
		}

		i += snprintf(buffer + i, LEN(len, i), "%c", *(str + j));
	}

	i += snprintf(buffer + i, LEN(len, i), "\"");

	return i;
}

int Printer::print_n(char* buffer, int len, const char* n) {
	return snprintf(buffer, len, "%s", n);
}

int Printer::print(char* buffer, int len, bool b) {
	return snprintf(buffer, len, "%s", b ? "true" : "false");
}

int Printer::print_newline(char* buffer, int len) {
	return snprintf(buffer, len, "%s", newline);
}

int Printer::print_indent(char* buffer, int len, int depth) {
	int n = depth * indent;
	int j = 0;
	for (int i = 0; i < n; i++) {
		j += snprintf(buffer + j, LEN(len, j), "%s", tab);
	}
	return j;
}

int Printer::print(char* buffer, int len, Value* value) {
	return print(buffer, len, value, 0, false);
}

int Printer::print(Value* value) {
	int i = print(nullptr, 0, value);
	char* buffer = (char*)malloc(i + 1);
	if (!buffer) {
		return -1;
	}

	print(buffer, i + 1, value);
	printf("%s", buffer);
	free(buffer);

	return i;
}

int Printer::dprint(Value* value) {
	Printer p;
	int i = p.print(nullptr, 0, value, 0, false);
	if (i < 0) {
		return -1;
	}
	char* buffer = (char*)malloc(i + 1);
	if (!buffer) {
		return -1;
	}
	p.print(buffer, i + 1, value);
	printf("%s", buffer);
	free(buffer);
	return i;
}

Printer::Printer(int indent, const char* tab, const char* newline) : indent(indent), tab(tab), newline(newline) {
}
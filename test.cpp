#include <cereal/json.h>
#include <cereal/parser.h>
#include <cereal/printer.h>
#include <stdio.h>

int main() {
    const char* buffer = "{\"key\":\"Hello\"}";
    cereal::json::Parser parser;
    cereal::json::Value* value;

    parser.parse(buffer, &value);

    cereal::json::Printer p(0, " ");
    p.print(value);

    delete value;
}
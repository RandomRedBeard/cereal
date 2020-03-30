#include <cereal/json.h>
#include <cereal/parser.h>
#include <cereal/printer.h>
#include <stdio.h>

int main() {
    cereal::json::Tree t;
    cereal::json::Value* value = new cereal::json::Value(cereal::json::STRING, strdup("Thomas"));
    t.put_value("key", value);

    cereal::json::Printer p(0, " ");
    p.print(&t);
}
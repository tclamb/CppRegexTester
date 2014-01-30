#include <json11.hpp>
#include <native/native.h>
#include <string>

#include "router.hpp"

using namespace json11;
using namespace native::http;
using namespace std;

void regex_tester(request& req, response& res) {
    #define serve_error_and_return_if(x) do { if ((x)) { res.set_status(400); res.end(""); return; } } while(0)

    string err;
    auto json = Json::parse(req.get_body(), err);
    serve_error_and_return_if(!err.empty());
    serve_error_and_return_if(!json.is_object());

    const auto& object = json.object_items();
    serve_error_and_return_if(object.find("regex") == object.end());
    serve_error_and_return_if(object.find("text") == object.end());

    #undef serve_error_and_return_if

    smatch matches;
    if (object.find("search") == object.end() || !json["search"].is_bool() || json["search"].bool_value() == false) {
        regex_match(json["text"].string_value(), matches, regex{json["regex"].string_value()});
    } else {
        regex_search(json["text"].string_value(), matches, regex{json["regex"].string_value()});
    }

    res.set_status(200);
    res.set_header("Content-Type", "application/json");
    res.end(Json{
        Json::object{
            {"matches", Json::array{matches.begin(), matches.end()}}
        }
    }.dump());
}

int main() {
    http server;
    int port = 8080;

    router r;
    r.add_handle("/regex", regex_tester);

    if (!server.listen("0.0.0.0", port, r)) {
        return -1;
    }

    return native::run();
}

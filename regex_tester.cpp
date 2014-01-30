#include <json11.hpp>
#include <native/native.h>
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>

#include "router.hpp"

using namespace json11;
using namespace native::http;
using namespace std;

string slurp_file(const string& filename) {
    ifstream in(filename, std::ios::in);
    if (!in) {
        return {};
    }

    string contents;
    in.seekg(0, ios::end);
    contents.resize(in.tellg());
    in.seekg(0, ios::beg);
    in.read(&contents[0], contents.size());
    in.close();

    return move(contents);
}

unordered_map<string, const string> mimetypeByExtension = {
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
};

void serve_file(request& req, response& res, const string& filename) {
    const auto contents = slurp_file(filename);
    if (contents.empty()) {
        res.set_status(404);
        res.set_header("Content-Type", "text/plain");
        res.end("Not found\n");
        return;
    }

    smatch matches;
    if (regex_match(filename, matches, regex{R"(.+(\.[\w]+))"})) {
        if (mimetypeByExtension.find(matches[1]) != mimetypeByExtension.cend()) {
            res.set_header("Content-Type", mimetypeByExtension[matches[1]]);
        }
    }

    res.set_status(200);
    res.end(contents);
}

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
    try {
        if (object.find("search") == object.end() || !json["search"].is_bool() || json["search"].bool_value() == false) {
            regex_match(json["text"].string_value(), matches, regex{json["regex"].string_value()});
        } else {
            regex_search(json["text"].string_value(), matches, regex{json["regex"].string_value()});
        }
    } catch (regex_error&) {
        res.set_status(400);
        res.end("");
        return;
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
    r.add_handle("/", [](request& req, response& res){ serve_file(req, res, "index.html"); });
    r.add_handle("/regex", regex_tester);
    r.add_handle("/assets/.*", [](request& req, response& res){ serve_file(req, res, req.url().path().substr(1)); });

    if (!server.listen("0.0.0.0", port, r)) {
        return -1;
    }

    return native::run();
}

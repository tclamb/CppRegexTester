#ifndef __ROUTER_HPP__
#define __ROUTER_HPP__

#include <functional>
#include <native/native.h>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>

struct router
{
    std::unordered_map<std::string, std::function<void(native::http::request&, native::http::response&)>> handles;

    void add_handle(std::string s, std::function<void(native::http::request&, native::http::response&)> f) {
        handles.insert(std::make_pair(s, f));
    }

    void operator()(native::http::request& req, native::http::response& res) {
        std::smatch matches;
        for (auto& p : handles) {
            if (std::regex_match(req.url().path(), matches, std::regex{p.first})) {
                p.second(req, res);
                return;
            }
        }
        res.set_status(404);
        res.set_header("Content-Type", "text/plain");
        res.end("Page not found\n");
    }
};

#endif//__ROUTER_HPP__

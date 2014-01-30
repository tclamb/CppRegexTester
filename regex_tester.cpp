#include <native/native.h>

#include "router.hpp"

using namespace native::http;

int main() {
    http server;
    int port = 8080;

    router r;
    r.add_handle("/", [](request& req, response& res) {
        res.set_status(200);
        res.set_header("Content-Type", "test/plain");
        res.end("Hello, world!\n");
    });

    if (!server.listen("0.0.0.0", port, r)) {
        return -1;
    }

    return native::run();
}

#include "Session.h"

namespace muxer {
    std::unordered_map<uuid, boost::shared_ptr<Session>, boost::hash<uuid> > Session::sessions =
            std::unordered_map<uuid, boost::shared_ptr<Session>, boost::hash<uuid> >();
    random_generator Session::generator = random_generator();
    //TODO: this is absolutely wrong.

    awaitable<void> Session::_splice(socket &in, socket &out) {
        try {
            char data[8192];
            while (true) {
                std::size_t n = co_await in.async_read_some(boost::asio::buffer(
                        data, 8192
                ), use_awaitable);
                std::cout << "read " << n << " bytes" << std::endl;
                co_await async_write(out, boost::asio::buffer(data, n), use_awaitable);
            }
        }
        catch (boost::system::system_error &e) {
            if (e.code().value() != boost::asio::error::eof)
                throw;
        }
        stop();
        std::cerr << "disconnected, remaining sessions: " << sessions.size() << std::endl;
    }

    void Session::start(boost::asio::io_context &context) {
        try {
            co_spawn(context,
                     _splice(incoming, upstream),
                     boost::asio::detached);
            co_spawn(context,
                     _splice(upstream, incoming),
                     boost::asio::detached);
            sessions[session_id] = shared_from_this();
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
            stop();
        }
    }

    void Session::stop() {
        if (incoming.is_open())
            incoming.close();
        if (upstream.is_open())
            upstream.close();
        if (sessions.find(session_id) != sessions.end())
            sessions.erase(session_id);
    }
}

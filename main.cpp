#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/make_shared.hpp>
#include <utility>

#include "Session.h"
#include "muxers/Muxer.h"

namespace muxer {
    namespace ip = boost::asio::ip;
    using std::string;
    using boost::asio::io_context;
    using boost::asio::awaitable;
    using boost::asio::use_awaitable;
    using boost::asio::co_spawn;

    class Ingress {
    private:
        io_context &_context;
        ip::tcp::acceptor _acceptor;
        boost::shared_ptr<muxers::Muxer> _muxer;

    public:
        Ingress(io_context &context,
                const string &local_host, ushort local_port,
                boost::shared_ptr<muxers::Muxer> muxer)
                : _context(context),
                  _muxer(std::move(muxer)),
                  _acceptor(_context, ip::tcp::endpoint(
                          ip::address::from_string(local_host), local_port)) {}

        awaitable<void> accept_connections() {
            while (!_context.stopped()) {
                try {
                    auto ses = boost::make_shared<Session>(_context);
                    co_await _acceptor.async_accept(ses->incoming, use_awaitable);
                    co_await _muxer->mux(ses);
                    ses->start(_context);
                } catch (std::exception &e) {
                    std::cerr << "Ingress exception: " << e.what() << std::endl;
                }
            }
            co_return;
        }

    };
} // namespace muxer

int main(int argc, char *argv[]) {
    const std::string local_host = "0.0.0.0";
    const ushort local_port = 2333;

    boost::asio::io_context context;
    boost::asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { context.stop(); });

    try {
        auto muxer = boost::make_shared<muxer::muxers::HTTPMuxer>();
        muxer::Ingress ingress(context, local_host, local_port, muxer);
        boost::asio::co_spawn(context, ingress.accept_connections(), boost::asio::detached);

        context.run();
    } catch (std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

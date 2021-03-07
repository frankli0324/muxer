#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/make_shared.hpp>

#include "utils/utils.h"
#include "utils/logger.h"
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
                  _acceptor(_context,
                            ip::tcp::endpoint(
                                    ip::address::from_string(local_host),
                                    local_port
                            ), true), // reuse addr
                  _muxer(std::move(muxer)) {}

        awaitable<void> accept_connections() {
            while (!_context.stopped()) {
                try {
                    auto ses = boost::make_shared<Session>(_context);
                    co_await _acceptor.async_accept(ses->incoming, use_awaitable);
                    INFO << "new connection from " << ses->incoming.remote_endpoint();
                    co_await _muxer->mux(ses);
                    INFO << "connected "
                         << ses->incoming.remote_endpoint() << " to "
                         << ses->upstream.remote_endpoint();
                    ses->start(_context);
                } catch (std::exception &e) {
                    WARNING << "Ingress: " << e.what() << std::endl;
                }
            }
            co_return;
        }

    };
} // namespace muxer

int main(int argc, char *argv[]) {
    auto args = muxer::utils::argparse(argc, argv, "ltp", "");
    const std::string local_host = args['l'];
    const ushort local_port = std::stoi(args['p']);
    const std::string type = args['t'];

    boost::asio::io_context context;
    boost::asio::signal_set signals(context, SIGINT, SIGTERM);
    signals.async_wait([&](auto, auto) { context.stop(); });

    try {
        INFO << "muxer starting on " << local_host << ':' << local_port;
        auto muxer = muxer::muxers::getMuxer(type);
        muxer::Ingress ingress(context, local_host, local_port, muxer);
        boost::asio::co_spawn(context, ingress.accept_connections(), boost::asio::detached);

        context.run();
    } catch (std::exception &e) {
        FATAL << "unable to start muxer: " << e.what();
        return 1;
    }

    return 0;
}

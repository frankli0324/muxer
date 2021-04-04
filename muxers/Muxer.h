#ifndef MUXER_MUXER_H
#define MUXER_MUXER_H

#include "../Session.h"
#include "../utils/utils.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <stdexcept>

namespace muxer::muxers {
    namespace ip = boost::asio::ip;

    class Muxer {
    public:
        virtual awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) { co_return; };
    };

    class HTTPMuxer : public Muxer {
        static std::string parse(const std::string &, const std::string &);

        awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) final;
    };

    class SocksMuxer : public Muxer {
        awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) final;
    };

    static boost::shared_ptr<Muxer> getMuxer(const std::string &name) {
        if (name == "http") return boost::make_shared<HTTPMuxer>();
        if (name == "socks") return boost::make_shared<SocksMuxer>();
        return nullptr;
    }
}
#endif //MUXER_MUXER_H

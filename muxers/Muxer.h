#ifndef MUXER_MUXER_H
#define MUXER_MUXER_H

#include "../Session.h"
#include "../utils/utils.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace muxer::muxers {
    namespace ip = boost::asio::ip;

    class Muxer {
    public:
        virtual awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) { co_return; };
    };

    class HTTPMuxer : public Muxer {
        static std::string parse(const std::string &, std::string);

        awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) final;
    };

}
#endif //MUXER_MUXER_H

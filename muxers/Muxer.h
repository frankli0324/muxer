//
// Created by Frank Li on 2021/2/6.
//

#ifndef MUXER_MUXER_H
#define MUXER_MUXER_H

#include "../Session.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace muxer::muxers {
    namespace ip = boost::asio::ip;

    class Muxer {
    public:
        virtual awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) { co_return; };
    };

    class HTTPMuxer : public Muxer {
        awaitable<void> mux(boost::shared_ptr<muxer::Session> &ses) final;
    };

}
#endif //MUXER_MUXER_H

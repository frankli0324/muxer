#include "Muxer.h"

namespace muxer::muxers {
    awaitable<void> SocksMuxer::mux(boost::shared_ptr<muxer::Session> &ses) {
        co_return;
    }
}
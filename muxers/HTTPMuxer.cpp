//
// Created by Frank Li on 2021/2/6.
//
#include "Muxer.h"

namespace muxer::muxers {
    awaitable<void> HTTPMuxer::mux(boost::shared_ptr<muxer::Session> &ses) {
        char data[4096];
        auto n = co_await ses->incoming.async_read_some(
                boost::asio::buffer(data, 4096),
                use_awaitable);
        std::cout << std::string(data, n) << std::endl;
        ip::tcp::endpoint endpoint = ip::tcp::endpoint(
                ip::address::from_string("127.0.0.1"), 1234
        );
        co_await ses->upstream.async_connect(endpoint, use_awaitable);
        co_await async_write(
                ses->upstream,
                boost::asio::buffer(data, n),
                use_awaitable);
        co_return;
    }
}

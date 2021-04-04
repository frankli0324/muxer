#include "Muxer.h"

namespace muxer::muxers {
    std::string HTTPMuxer::parse(const std::string &head, const std::string &delimiter = "\r\n") {
        size_t last = 0, pos = 0;
        bool first = true;
        std::string token;
        while ((pos = head.find(delimiter, last)) != std::string::npos) {
            token = head.substr(last, pos - last);
            last = pos + delimiter.length();
            if (first) {
                first = false;
                continue;
            }
            if (token.find("Host:") != std::string::npos)
                return utils::trim(token.substr(5));
        }
        return std::string();
    }

    awaitable<void> HTTPMuxer::mux(boost::shared_ptr<muxer::Session> &ses) {
        char data[4096];
        size_t n;
        std::string head = std::string(), host = std::string();
        while (host.empty()) {
            n = co_await ses->incoming.async_read_some(
                    boost::asio::buffer(data, 4096),
                    use_awaitable);
            head += std::string(data, n);
            host = parse(head);
        }
        ip::tcp::resolver resolver(ses->incoming.get_executor());
        std::string port = "80";
        if ((n = host.find(':')) != std::string::npos) {
            port = host.substr(n + 1);
            host = host.substr(0, n);
        }
        auto result = co_await resolver.async_resolve(host, port, use_awaitable);

        co_await ses->upstream.async_connect(result->endpoint(), use_awaitable);
        co_await async_write(ses->upstream, boost::asio::buffer(head), use_awaitable);
        co_return;
    }
}

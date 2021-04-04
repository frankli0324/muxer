#include "Muxer.h"

enum CMD {
    CONNECT = 1, BIND = 2, UDP_ASSOCIATE = 3
};
enum ATYP {
    ipv4 = 1, domain = 3, ipv6 = 4
};
#define require(bytes) while (n < (bytes)) { \
            n += co_await ses->incoming.async_read_some( \
                    boost::asio::buffer(data + n, bytes - n), use_awaitable); \
        }
namespace muxer::muxers {
    using namespace boost::asio::ip;

    awaitable<ip::tcp::endpoint> get_address(ip::tcp::resolver resolver, short type, char *addr) {
        char addr_s[64];
        std::string addr_str = addr;
        std::stringstream ss;
        ip::address_v4::uint_type addr_v4 = 0;

        switch (type) {
            case ATYP::ipv4:
                for (short i = 0; i < 4; i++) {
                    addr_v4 *= 256;
                    addr_v4 += (u_char) addr[i];
                }
                co_return ip::tcp::endpoint(
                        boost::asio::ip::address_v4(addr_v4),
                        addr[4] * 16 * 16 + addr[5]
                );
            case ATYP::ipv6:
                assert(false);
                // what is "scope" for ipv6?
            case ATYP::domain:
                ss << addr[addr[0] + 1] * 16 * 16 + addr[addr[0] + 2];
                co_return (co_await resolver.async_resolve(
                        addr_str.substr(1, addr_str[0]), ss.str(),
                        use_awaitable))->endpoint();
            default:
                assert (false);
        }
    }

    void debug(int n, char *data) {
        std::stringstream ss;
        for (int i = 0; i < n; ++i)
            ss << std::hex << (int) data[i] << " ";
        DEBUG << ss.str();
    }

#undef assert

    void assert(bool condition) {
        if (!condition) throw std::exception();
    }

    awaitable<void> SocksMuxer::mux(boost::shared_ptr<muxer::Session> &ses) {
        char data[4096];
        size_t n = co_await ses->incoming.async_read_some(
                boost::asio::buffer(data, 4096), use_awaitable);
        assert(n > 0 && data[0] == 0x05);
        size_t preferred_cnt = (u_char) data[1];
        require(preferred_cnt + 2);
        char *preferred = data + 2;
        char resp[3] = "\x05\x00";
        resp[1] = 0;
        co_await async_write(ses->incoming,
                             boost::asio::buffer(resp, 2), use_awaitable);
        require(preferred_cnt + 2 + 4);
        char *request = preferred + preferred_cnt;
        assert(request[0] == 0x05);
        assert(request[1] == CMD::CONNECT);
        assert(request[2] == 0x00);
        switch (request[3]) {
            case ATYP::ipv4:
                require(preferred_cnt + 2 + 4 + 6);
                break;
            case ATYP::ipv6:
                require(preferred_cnt + 2 + 4 + 18);
                break;
            case ATYP::domain:
                require(preferred_cnt + 2 + 4 + 1);
                require(preferred_cnt + 2 + 4 + 1 + request[4] + 2);
                break;
        }
        auto target = co_await get_address(
                ip::tcp::resolver(ses->incoming.get_executor()),
                request[3], request + 4);
        co_await ses->upstream.async_connect(target, use_awaitable);
        co_await async_write(ses->incoming, boost::asio::buffer(
                "\x05\x00\x00\x01\x00\x00\x00\x00\x50"
        ), use_awaitable);
        co_return;
    }
} // namespace muxer::muxers
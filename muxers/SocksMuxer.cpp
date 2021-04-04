#include "Muxer.h"

enum CMD {
    CONNECT = 1, BIND = 2, UDP_ASSOCIATE = 3
};
enum ATYP {
    ipv4 = 1, domain = 3, ipv6 = 4
};
namespace muxer::muxers {
    using namespace boost::asio::ip;

    class socks_error : public std::runtime_error {
    public:
        u_short code;

        socks_error(u_short err, const char *msg) : runtime_error(msg), code(err) {}
    };

    awaitable<ip::tcp::endpoint> get_address(ip::tcp::resolver resolver, short type, char *addr) {
        char addr_s[64], port_s[6];
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
                throw socks_error(0x08, "IPv6 Not Supported");
                // what is "scope" for ipv6?
            case ATYP::domain:
                strncpy(addr_s, addr + 1, addr[0]);
                sprintf(port_s, "%d", addr[addr[0] + 1] * 16 * 16 + addr[addr[0] + 2]);
                co_return (co_await resolver.async_resolve(
                        addr_s, port_s, use_awaitable))->endpoint();
            default:
                throw socks_error(0x08, "Incorrect Address Type");
        }
    }

#define require(bytes) while (n < (bytes)) { \
            n += co_await ses->incoming.async_read_some( \
                    boost::asio::buffer(data + n, bytes - n), use_awaitable); \
        }


    awaitable<void> SocksMuxer::mux(boost::shared_ptr<muxer::Session> &ses) {
        char data[4096];
        size_t n = co_await ses->incoming.async_read_some(
                boost::asio::buffer(data, 4096), use_awaitable);
        if (n <= 0 || data[0] != 0x05)
            throw std::runtime_error("Incorrect SOCKS Version");
        size_t preferred_cnt = (u_char) data[1];
        require(preferred_cnt + 2);
        char *preferred = data + 2;
        co_await async_write(ses->incoming,
                             boost::asio::buffer("\x05\x00", 2), use_awaitable);
        // negotiation success
        int err_code = 0x00;
        try {
            require(preferred_cnt + 2 + 4);
            char *request = preferred + preferred_cnt;
            std::string err;
            if (request[0] != 0x05)
                throw socks_error(0x01, "Incorrect SOCKS Version");
            if (request[1] != CMD::CONNECT)
                throw socks_error(0x07, "Invalid SOCKS CMD (CONNECT only)");
            if (request[2] != 0x00)
                throw socks_error(0x01, "Incorrect Reserved Byte");

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
            try {
                co_await ses->upstream.async_connect(target, use_awaitable);
            } catch (const boost::system::system_error &e) {
                switch (e.code().value()) {
                    case boost::asio::error::network_unreachable:
                        throw socks_error(0x03, "Remote Network Unreachable");
                    case boost::asio::error::host_unreachable:
                        throw socks_error(0x04, "Remote Host Unreachable");
                    case boost::asio::error::connection_refused:
                        throw socks_error(0x05, "Remote Connection Refused");
                    default:
                        throw socks_error(0x01, "Remote Connection Error");
                }
            }
        } catch (const socks_error &e) {
            err_code = e.code;
            ERROR << e.what();
        }
        char resp[] = "\x05\x00\x00\x01\x00\x00\x00\x00\x00\x50";
        resp[1] = (char) err_code;
        resp[8] = (char) (ses->incoming.local_endpoint().port() / 16 / 16);
        resp[9] = (char) (ses->incoming.local_endpoint().port() - resp[8]);
        co_await async_write(
                ses->incoming,
                boost::asio::buffer(resp, 10),
                use_awaitable);
        co_return;
    }

#undef require
} // namespace muxer::muxers

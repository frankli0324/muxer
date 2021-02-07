#ifndef MUXER_SESSION_H
#define MUXER_SESSION_H

#include <iostream>
#include <string>
#include <unordered_map>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace muxer {
    namespace ip = boost::asio::ip;
    using socket = ip::tcp::socket;
    using boost::asio::io_context;
    using boost::asio::awaitable;
    using boost::asio::use_awaitable;
    using boost::asio::co_spawn;
    using boost::uuids::uuid;
    using boost::uuids::random_generator;

    class Session : public boost::enable_shared_from_this<Session> {
    private:
        static random_generator generator;
        static std::unordered_map<uuid, boost::shared_ptr<Session>, boost::hash<uuid> > sessions;
        //TODO: this is unacceptable, but how to avoid it?
        uuid session_id;

        awaitable<void> _splice(socket &in, socket &out);

    public:
        socket incoming, upstream;

        explicit Session(io_context &context) :
                incoming(context), upstream(context), session_id(generator()) {}

        void stop();

        void start(io_context &context);
    };


}

#endif //MUXER_SESSION_H

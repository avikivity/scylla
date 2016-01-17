/*
 * Copyright (C) 2015 Cloudius Systems, Ltd.
 */

/*
 * This file is part of Scylla.
 *
 * Scylla is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Scylla is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Scylla.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "core/reactor.hh"
#include "core/iostream.hh"
#include "core/distributed.hh"
#include "core/print.hh"
#include "core/sstring.hh"
#include "core/do_with.hh"
#include "net/api.hh"
#include "utils/serialization.hh"
#include "gms/inet_address.hh"
#include "rpc/rpc_types.hh"
#include <unordered_map>
#include "frozen_mutation.hh"
#include "frozen_schema.hh"
#include "query-request.hh"
#include "db/serializer.hh"
#include "mutation_query.hh"
#include "repair/repair.hh"

#include <seastar/net/tls.hh>

// forward declarations
namespace streaming { namespace messages {
    class stream_init_message;
    class prepare_message;
}}

namespace gms {
    class gossip_digest_syn;
    class gossip_digest_ack;
    class gossip_digest_ack2;
}

class frozen_mutation;

namespace utils {
    class UUID;
}

namespace db {
class seed_provider_type;
}

namespace net {

/* All verb handler identifiers */
enum class messaging_verb : int32_t {
    CLIENT_ID = 0,
    ECHO = 1,
    MUTATION = 2,
    MUTATION_DONE = 3,
    READ_DATA = 4,
    READ_MUTATION_DATA = 5,
    READ_DIGEST = 6,
    GOSSIP_DIGEST_SYN = 7,
    GOSSIP_DIGEST_ACK2 = 8,
    GOSSIP_SHUTDOWN = 9,
    DEFINITIONS_UPDATE = 10,
    TRUNCATE = 11,
    REPLICATION_FINISHED = 12,
    MIGRATION_REQUEST = 13,
    // Used by streaming
    STREAM_INIT_MESSAGE = 14,
    PREPARE_MESSAGE = 15,
    PREPARE_DONE_MESSAGE = 16,
    STREAM_MUTATION = 17,
    STREAM_MUTATION_DONE = 18,
    COMPLETE_MESSAGE = 19,
    // end of streaming verbs
    REPAIR_CHECKSUM_RANGE = 20,
    GET_SCHEMA_VERSION = 21,
    LAST = 22,
};

} // namespace net

namespace std {
template <>
class hash<net::messaging_verb> {
public:
    size_t operator()(const net::messaging_verb& x) const {
        return hash<int32_t>()(int32_t(x));
    }
};
} // namespace std

namespace net {

// NOTE: operator(input_stream<char>&, T&) takes a reference to uninitialized
//       T object and should use placement new in case T is non POD
struct serializer {
    template <typename T, typename Input>
    inline T read_integral(Input& input) const {
        static_assert(std::is_integral<T>::value, "T should be integral");
        T data;
        input.read(reinterpret_cast<char*>(&data), sizeof(T));
        return net::ntoh(data);
    }

    template <typename T, typename Output>
    inline void write_integral(Output& output, T data) const {
        static_assert(std::is_integral<T>::value, "T should be integral");
        data = net::hton(data);
        output.write(reinterpret_cast<const char*>(&data), sizeof(T));
    }

    // Adaptor for writing objects having db::serializer<>
    template <typename Serializable, typename Output>
    inline void write_serializable(Output& out, const Serializable& v) const {
        db::serializer<Serializable> ser(v);
        bytes b(bytes::initialized_later(), ser.size() + data_output::serialized_size<uint32_t>());
        data_output d_out(b);
        d_out.write<uint32_t>(ser.size());
        ser.write(d_out);
        return out.write(reinterpret_cast<const char*>(b.c_str()), b.size());
    }

    // Adaptor for reading objects having db::serializer<>
    template <typename Serializable, typename Input>
    inline Serializable read_serializable(Input& in) const {
        auto sz = read_integral<uint32_t>(in);
        bytes data(bytes::initialized_later(), sz);
        in.read(reinterpret_cast<char*>(data.begin()), sz);
        data_input din(data);
        return db::serializer<Serializable>::read(din);
    }

    // For integer type
    template <typename Input>
    bool read(Input& input, rpc::type<bool>) const { return read(input, rpc::type<uint8_t>()); }
    template <typename Input>
    int8_t read(Input& input, rpc::type<int8_t>) const { return read_integral<int8_t>(input); }
    template <typename Input>
    uint8_t read(Input& input, rpc::type<uint8_t>) const { return read_integral<uint8_t>(input); }
    template <typename Input>
    int16_t read(Input& input, rpc::type<int16_t>) const { return read_integral<int16_t>(input); }
    template <typename Input>
    uint16_t read(Input& input, rpc::type<uint16_t>) const { return read_integral<uint16_t>(input); }
    template <typename Input>
    int32_t read(Input& input, rpc::type<int32_t>) const { return read_integral<int32_t>(input); }
    template <typename Input>
    uint32_t read(Input& input, rpc::type<uint32_t>) const { return read_integral<uint32_t>(input); }
    template <typename Input>
    int64_t read(Input& input, rpc::type<int64_t>) const { return read_integral<int64_t>(input); }
    template <typename Input>
    uint64_t read(Input& input, rpc::type<uint64_t>) const { return read_integral<uint64_t>(input); }
    template <typename Output>
    void write(Output& output, bool data) const { write(output, uint8_t(data)); }
    template <typename Output>
    void write(Output& output, int8_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, uint8_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, int16_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, uint16_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, int32_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, uint32_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, int64_t data) const { write_integral(output, data); }
    template <typename Output>
    void write(Output& output, uint64_t data) const { write_integral(output, data); }

    // For vectors
    template <typename T, typename Output>
    inline void write(Output& out, const std::vector<T>& v) const {
        write(out, uint32_t(v.size()));
        for (auto&& e : v) {
            write(out, e);
        }
    }
    template <typename T, typename Input>
    inline std::vector<T> read(Input& in, rpc::type<std::vector<T>>) const {
        auto sz = read(in, rpc::type<uint32_t>());
        std::vector<T> v;
        v.reserve(sz);
        while (sz--) {
            v.push_back(read(in, rpc::type<T>()));
        }
        return v;
    }

    // For messaging_verb
    template <typename Output>
    void write(Output& out, messaging_verb v) const {
        return write(out, std::underlying_type_t<messaging_verb>(v));
    }
    template <typename Input>
    messaging_verb operator()(Input& in, rpc::type<messaging_verb>) const {
        return messaging_verb(read(in, rpc::type<std::underlying_type_t<messaging_verb>>()));
    }

    // For sstring
    template <typename Output>
    void write(Output& out, const sstring& v) const {
        write(out, uint32_t(v.size()));
        out.write(v.begin(), v.size());
    }
    template <typename Input>
    sstring read(Input& in, rpc::type<sstring>) const {
        auto sz = read(in, rpc::type<uint32_t>());
        sstring v(sstring::initialized_later(), sz);
        in.read(v.begin(), sz);
        return v;
    }

    // For frozen_mutation
    template <typename Output>
    void write(Output& out, const frozen_mutation& v) const{
        return write_serializable(out, v);
    }
    template <typename Input>
    frozen_mutation read(Input& in, rpc::type<frozen_mutation>) const {
        return read_serializable<frozen_mutation>(in);
    }

    // For frozen_schema
    template <typename Output>
    void write(Output& out, const frozen_schema& v) const{
        return write_serializable(out, v);
    }
    template <typename Input>
    frozen_schema read(Input& in, rpc::type<frozen_schema>) const {
        return read_serializable<frozen_schema>(in);
    }

    // For reconcilable_result
    template <typename Output>
    void write(Output& out, const reconcilable_result& v) const{
        return write_serializable(out, v);
    }
    template <typename Input>
    reconcilable_result read(Input& in, rpc::type<reconcilable_result>) const {
        return read_serializable<reconcilable_result>(in);
    }

    template <typename Output>
    void write(Output& out, const query::result& v) const;
    template <typename Input>
    query::result read(Input& in, rpc::type<query::result>) const;


    // Default implementation for any type which knows how to serialize itself
    // with methods serialize(), deserialize() and serialized_size() with the
    // following signatures:
    //     void serialize(bytes::iterator& out) const;
    //     size_t serialized_size() const;
    //     static T deserialize(bytes_view& in);
    //
    // One inefficiency inherent in this API is that deserialize() expects
    // the serialized data to have been already read into a contiguous buffer,
    // and to do this, the reader needs to know in advance how much to read,
    // so we are forced to precede the serialized data by its length - even
    // though the deserialize() function should already know where to stop.
    // Even a fixed-length object will end up preceeded by its length.
    // This waste can be avoided by implementing special read()/write()
    // functions for this type, above.
    template <typename T, typename Output>
    void write(Output& out, const T& v) const {
        uint32_t sz = v.serialized_size();
        write(out, sz);
        bytes b(bytes::initialized_later(), sz);
        auto _out = b.begin();
        v.serialize(_out);
        out.write(reinterpret_cast<const char*>(b.c_str()), sz);
    }
    template <typename T, typename Input>
    T read(Input& in, rpc::type<T>) const {
        auto sz = read(in, rpc::type<uint32_t>());
        bytes b(bytes::initialized_later(), sz);
        in.read(reinterpret_cast<char*>(b.begin()), sz);
        bytes_view bv(b);
        return T::deserialize(bv);
    }

    template <typename Output, typename T>
    void write(Output& out, const foreign_ptr<T>& v) const {
        return write(out, *v);
    }
    template <typename Input, typename T>
    foreign_ptr<T> read(Input& in, rpc::type<foreign_ptr<T>>) const {
        return make_foreign(read(in, rpc::type<T>()));
    }

    template <typename Output, typename T>
    void write(Output& out, const lw_shared_ptr<T>& v) const {
        return write(out, *v);
    }
    template <typename Input, typename T>
    lw_shared_ptr<T> read(Input& in, rpc::type<lw_shared_ptr<T>>) const {
        return make_lw_shared(read(in, rpc::type<T>()));
    }
};

// thunk from new-style free function serialization to old-style member function
template <typename Output, typename T>
inline
void
write(serializer s, Output& out, const T& data) {
    s.write(out, data);
}

template <typename Input, typename T>
inline
T
read(serializer s, Input& in, rpc::type<T> type) {
    return s.read(in, type);
}

struct msg_addr {
    gms::inet_address addr;
    uint32_t cpu_id;
    friend bool operator==(const msg_addr& x, const msg_addr& y);
    friend bool operator<(const msg_addr& x, const msg_addr& y);
    friend std::ostream& operator<<(std::ostream& os, const msg_addr& x);
    struct hash {
        size_t operator()(const msg_addr& id) const;
    };
};

class messaging_service : public seastar::async_sharded_service<messaging_service> {
public:
    struct rpc_protocol_wrapper;
    struct rpc_protocol_client_wrapper;
    struct rpc_protocol_server_wrapper;
    struct shard_info;

    using msg_addr = net::msg_addr;
    using inet_address = gms::inet_address;
    using UUID = utils::UUID;
    using clients_map = std::unordered_map<msg_addr, shard_info, msg_addr::hash>;

    // FIXME: messaging service versioning
    static constexpr int32_t current_version = 0;

    struct shard_info {
        shard_info(shared_ptr<rpc_protocol_client_wrapper>&& client);
        shared_ptr<rpc_protocol_client_wrapper> rpc_client;
        rpc::stats get_stats() const;
    };

    void foreach_client(std::function<void(const msg_addr& id, const shard_info& info)> f) const;

    void increment_dropped_messages(messaging_verb verb);

    uint64_t get_dropped_messages(messaging_verb verb) const;

    const uint64_t* get_dropped_messages() const;

    int32_t get_raw_version(const gms::inet_address& endpoint) const;

    bool knows_version(const gms::inet_address& endpoint) const;

    enum class encrypt_what {
        none,
        rack,
        dc,
        all,
    };

private:
    gms::inet_address _listen_address;
    uint16_t _port;
    uint16_t _ssl_port;
    encrypt_what _encrypt_what;
    // map: Node broadcast address -> Node internal IP for communication within the same data center
    std::unordered_map<gms::inet_address, gms::inet_address> _preferred_ip_cache;
    std::unique_ptr<rpc_protocol_wrapper> _rpc;
    std::unique_ptr<rpc_protocol_server_wrapper> _server;
    ::shared_ptr<seastar::tls::server_credentials> _credentials;
    std::unique_ptr<rpc_protocol_server_wrapper> _server_tls;
    std::array<clients_map, 2> _clients;
    uint64_t _dropped_messages[static_cast<int32_t>(messaging_verb::LAST)] = {};
public:
    using clock_type = std::chrono::steady_clock;
public:
    messaging_service(gms::inet_address ip = gms::inet_address("0.0.0.0"), uint16_t port = 7000);
    messaging_service(gms::inet_address ip, uint16_t port, encrypt_what,
            uint16_t ssl_port, ::shared_ptr<seastar::tls::server_credentials>);
    ~messaging_service();
public:
    uint16_t port();
    gms::inet_address listen_address();
    future<> stop();
    static rpc::no_wait_type no_wait();
public:
    gms::inet_address get_preferred_ip(gms::inet_address ep);
    future<> init_local_preferred_ip_cache();
    void cache_preferred_ip(gms::inet_address ep, gms::inet_address ip);

    // Wrapper for STREAM_INIT_MESSAGE verb
    void register_stream_init_message(std::function<future<unsigned> (const rpc::client_info& cinfo, streaming::messages::stream_init_message msg)>&& func);
    future<unsigned> send_stream_init_message(msg_addr id, streaming::messages::stream_init_message msg);

    // Wrapper for PREPARE_MESSAGE verb
    void register_prepare_message(std::function<future<streaming::messages::prepare_message> (const rpc::client_info& cinfo,
            streaming::messages::prepare_message msg, UUID plan_id,
            unsigned dst_cpu_id)>&& func);
    future<streaming::messages::prepare_message> send_prepare_message(msg_addr id, streaming::messages::prepare_message msg, UUID plan_id,
            unsigned dst_cpu_id);

    // Wrapper for PREPARE_DONE_MESSAGE verb
    void register_prepare_done_message(std::function<future<> (const rpc::client_info& cinfo, UUID plan_id, unsigned dst_cpu_id)>&& func);
    future<> send_prepare_done_message(msg_addr id, UUID plan_id, unsigned dst_cpu_id);

    // Wrapper for STREAM_MUTATION verb
    void register_stream_mutation(std::function<future<> (const rpc::client_info& cinfo, UUID plan_id, frozen_mutation fm, unsigned dst_cpu_id)>&& func);
    future<> send_stream_mutation(msg_addr id, UUID plan_id, frozen_mutation fm, unsigned dst_cpu_id);

    void register_stream_mutation_done(std::function<future<> (const rpc::client_info& cinfo, UUID plan_id, std::vector<range<dht::token>> ranges, UUID cf_id, unsigned dst_cpu_id)>&& func);
    future<> send_stream_mutation_done(msg_addr id, UUID plan_id, std::vector<range<dht::token>> ranges, UUID cf_id, unsigned dst_cpu_id);

    void register_complete_message(std::function<future<> (const rpc::client_info& cinfo, UUID plan_id, unsigned dst_cpu_id)>&& func);
    future<> send_complete_message(msg_addr id, UUID plan_id, unsigned dst_cpu_id);

    // Wrapper for REPAIR_CHECKSUM_RANGE verb
    void register_repair_checksum_range(std::function<future<partition_checksum> (sstring keyspace, sstring cf, query::range<dht::token> range)>&& func);
    void unregister_repair_checksum_range();
    future<partition_checksum> send_repair_checksum_range(msg_addr id, sstring keyspace, sstring cf, query::range<dht::token> range);

    // Wrapper for ECHO verb
    void register_echo(std::function<future<> ()>&& func);
    void unregister_echo();
    future<> send_echo(msg_addr id);

    // Wrapper for GOSSIP_SHUTDOWN
    void register_gossip_shutdown(std::function<rpc::no_wait_type (inet_address from)>&& func);
    void unregister_gossip_shutdown();
    future<> send_gossip_shutdown(msg_addr id, inet_address from);

    // Wrapper for GOSSIP_DIGEST_SYN
    void register_gossip_digest_syn(std::function<future<gms::gossip_digest_ack> (gms::gossip_digest_syn)>&& func);
    void unregister_gossip_digest_syn();
    future<gms::gossip_digest_ack> send_gossip_digest_syn(msg_addr id, gms::gossip_digest_syn msg);

    // Wrapper for GOSSIP_DIGEST_ACK2
    void register_gossip_digest_ack2(std::function<rpc::no_wait_type (gms::gossip_digest_ack2)>&& func);
    void unregister_gossip_digest_ack2();
    future<> send_gossip_digest_ack2(msg_addr id, gms::gossip_digest_ack2 msg);

    // Wrapper for DEFINITIONS_UPDATE
    void register_definitions_update(std::function<rpc::no_wait_type (const rpc::client_info& cinfo, std::vector<frozen_mutation> fm)>&& func);
    void unregister_definitions_update();
    future<> send_definitions_update(msg_addr id, std::vector<frozen_mutation> fm);

    // Wrapper for MIGRATION_REQUEST
    void register_migration_request(std::function<future<std::vector<frozen_mutation>> ()>&& func);
    void unregister_migration_request();
    future<std::vector<frozen_mutation>> send_migration_request(msg_addr id);

    // FIXME: response_id_type is an alias in service::storage_proxy::response_id_type
    using response_id_type = uint64_t;
    // Wrapper for MUTATION
    void register_mutation(std::function<future<rpc::no_wait_type> (const rpc::client_info&, frozen_mutation fm, std::vector<inet_address> forward,
        inet_address reply_to, unsigned shard, response_id_type response_id)>&& func);
    void unregister_mutation();
    future<> send_mutation(msg_addr id, clock_type::time_point timeout, const frozen_mutation& fm, std::vector<inet_address> forward,
        inet_address reply_to, unsigned shard, response_id_type response_id);

    // Wrapper for MUTATION_DONE
    void register_mutation_done(std::function<future<rpc::no_wait_type> (const rpc::client_info& cinfo, unsigned shard, response_id_type response_id)>&& func);
    void unregister_mutation_done();
    future<> send_mutation_done(msg_addr id, unsigned shard, response_id_type response_id);

    // Wrapper for READ_DATA
    // Note: WTH is future<foreign_ptr<lw_shared_ptr<query::result>>
    void register_read_data(std::function<future<foreign_ptr<lw_shared_ptr<query::result>>> (const rpc::client_info&, query::read_command cmd, query::partition_range pr)>&& func);
    void unregister_read_data();
    future<query::result> send_read_data(msg_addr id, const query::read_command& cmd, const query::partition_range& pr);

    // Wrapper for GET_SCHEMA_VERSION
    void register_get_schema_version(std::function<future<frozen_schema>(unsigned, table_schema_version)>&& func);
    void unregister_get_schema_version();
    future<frozen_schema> send_get_schema_version(msg_addr, table_schema_version);

    // Wrapper for READ_MUTATION_DATA
    void register_read_mutation_data(std::function<future<foreign_ptr<lw_shared_ptr<reconcilable_result>>> (const rpc::client_info&, query::read_command cmd, query::partition_range pr)>&& func);
    void unregister_read_mutation_data();
    future<reconcilable_result> send_read_mutation_data(msg_addr id, const query::read_command& cmd, const query::partition_range& pr);

    // Wrapper for READ_DIGEST
    void register_read_digest(std::function<future<query::result_digest> (const rpc::client_info&, query::read_command cmd, query::partition_range pr)>&& func);
    void unregister_read_digest();
    future<query::result_digest> send_read_digest(msg_addr id, const query::read_command& cmd, const query::partition_range& pr);

    // Wrapper for TRUNCATE
    void register_truncate(std::function<future<>(sstring, sstring)>&& func);
    void unregister_truncate();
    future<> send_truncate(msg_addr, std::chrono::milliseconds, sstring, sstring);

    // Wrapper for REPLICATION_FINISHED verb
    void register_replication_finished(std::function<future<> (inet_address from)>&& func);
    void unregister_replication_finished();
    future<> send_replication_finished(msg_addr id, inet_address from);
    void foreach_server_connection_stats(std::function<void(const rpc::client_info&, const rpc::stats&)>&& f) const;
public:
    // Return rpc::protocol::client for a shard which is a ip + cpuid pair.
    shared_ptr<rpc_protocol_client_wrapper> get_rpc_client(messaging_verb verb, msg_addr id);
    void remove_rpc_client_one(clients_map& clients, msg_addr id, bool dead_only);
    void remove_error_rpc_client(messaging_verb verb, msg_addr id);
    void remove_rpc_client(msg_addr id);
    std::unique_ptr<rpc_protocol_wrapper>& rpc();
    static msg_addr get_source(const rpc::client_info& client);
};

extern distributed<messaging_service> _the_messaging_service;

inline distributed<messaging_service>& get_messaging_service() {
    return _the_messaging_service;
}

inline messaging_service& get_local_messaging_service() {
    return _the_messaging_service.local();
}

future<> init_messaging_service(sstring listen_address, db::seed_provider_type seed_provider);
} // namespace net

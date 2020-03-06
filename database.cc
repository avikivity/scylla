#include <atomic>
#include <cstring>
#ifdef SEASTAR_USE_STD_OPTIONAL_VARIANT_STRINGVIEW
#include <optional>
#include <variant>
#else
#endif
#if __cplusplus >= 201703L && __has_include(<filesystem>)
#include <filesystem>
#else
#endif
#if __cplusplus >= 201703L && __has_include(<memory_resource>)
#define SEASTAR_HAS_POLYMORPHIC_ALLOCATOR
#include <memory_resource>
namespace seastar {
namespace compat {
using memory_resource = std::pmr::memory_resource;
template<typename T>
using polymorphic_allocator = std::pmr::polymorphic_allocator<T>;
static inline
memory_resource* pmr_get_default_resource() {
    return std::pmr::get_default_resource();
}
}
}
#elif __cplusplus >= 201703L && __has_include(<experimental/memory_resource>)
#define SEASTAR_HAS_POLYMORPHIC_ALLOCATOR
namespace seastar {
namespace compat {
using memory_resource = std::experimental::pmr::memory_resource;
template<typename T>
using polymorphic_allocator = std::experimental::pmr::polymorphic_allocator<T>;
static inline
memory_resource* pmr_get_default_resource() {
    return std::experimental::pmr::get_default_resource();
}
}
}
#else
namespace seastar {
namespace compat {
class memory_resource {};
static
memory_resource* pmr_get_default_resource() ;
template <typename T>
class polymorphic_allocator {
public:
    explicit polymorphic_allocator(memory_resource*);
    T* allocate( std::size_t n ) ;
    void deallocate(T* p, std::size_t n ) ;
};
}
}
#endif
#define SEASTAR_ASAN_ENABLED
namespace seastar {
namespace compat {
#ifdef SEASTAR_USE_STD_OPTIONAL_VARIANT_STRINGVIEW
template <typename T>
using optional = std::optional<T>;
using nullopt_t = std::nullopt_t;
inline constexpr auto nullopt = std::nullopt;
template <typename T>
inline constexpr optional<std::decay_t<T>> make_optional(T&& value) {
    return std::make_optional(std::forward<T>(value));
}
template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string_view = std::basic_string_view<CharT, Traits>;
template <typename CharT, typename Traits = std::char_traits<CharT>>
std::string string_view_to_string(const basic_string_view<CharT, Traits>& v) {
    return std::string(v);
}
template <typename... Types>
using variant = std::variant<Types...>;
template <std::size_t I, typename... Types>
constexpr std::variant_alternative_t<I, variant<Types...>>& get(variant<Types...>& v) {
    return std::get<I>(v);
}
template <std::size_t I, typename... Types>
constexpr const std::variant_alternative_t<I, variant<Types...>>& get(const variant<Types...>& v) {
    return std::get<I>(v);
}
template <std::size_t I, typename... Types>
constexpr std::variant_alternative_t<I, variant<Types...>>&& get(variant<Types...>&& v) {
    return std::get<I>(v);
}
template <std::size_t I, typename... Types>
constexpr const std::variant_alternative_t<I, variant<Types...>>&& get(const variant<Types...>&& v) {
    return std::get<I>(v);
}
template <typename U, typename... Types>
constexpr U& get(variant<Types...>& v) {
    return std::get<U>(v);
}
template <typename U, typename... Types>
constexpr const U& get(const variant<Types...>& v) {
    return std::get<U>(v);
}
template <typename U, typename... Types>
constexpr U&& get(variant<Types...>&& v) {
    return std::get<U>(v);
}
template <typename U, typename... Types>
constexpr const U&& get(const variant<Types...>&& v) {
    return std::get<U>(v);
}
template <typename U, typename... Types>
constexpr U* get_if(variant<Types...>* v) {
    return std::get_if<U>(v);
}
template <typename U, typename... Types>
constexpr const U* get_if(const variant<Types...>* v) {
    return std::get_if<U>(v);
}
#else
template <typename T>
using optional = std::experimental::optional<T>;
using nullopt_t = std::experimental::nullopt_t;
constexpr auto nullopt = std::experimental::nullopt;
template <typename T>
inline constexpr optional<std::decay_t<T>> make_optional(T&& value) {
    return std::experimental::make_optional(std::forward<T>(value));
}
template <typename CharT, typename Traits = std::char_traits<CharT>>
using basic_string_view = std::experimental::basic_string_view<CharT, Traits>;
template <typename CharT, typename Traits = std::char_traits<CharT>>
std::string string_view_to_string(const basic_string_view<CharT, Traits>& v) ;
template <typename... Types>
using variant = boost::variant<Types...>;
template<typename U, typename... Types>
U& get(variant<Types...>& v) {
    return boost::get<U, Types...>(v);
}
template<typename U, typename... Types>
U&& get(variant<Types...>&& v) {
    return boost::get<U, Types...>(v);
}
template<typename U, typename... Types>
const U& get(const variant<Types...>& v) ;
template<typename U, typename... Types>
const U&& get(const variant<Types...>&& v) ;
template<typename U, typename... Types>
U* get_if(variant<Types...>* v) {
    return boost::get<U, Types...>(v);
}
template<typename U, typename... Types>
const U* get_if(const variant<Types...>* v) ;
#endif
namespace filesystem = std::filesystem;
using string_view = basic_string_view<char>;
} 
} 
#if __cplusplus >= 201703L && defined(__cpp_guaranteed_copy_elision)
#define SEASTAR_COPY_ELISION(x) x
#else
#define SEASTAR_COPY_ELISION(x) std::move(x)
#endif
namespace seastar {
class deleter final {
public:
    struct impl;
    struct raw_object_tag {};
private:
    impl* _impl = nullptr;
public:
    deleter() = default;
    deleter(const deleter&) = delete;
    deleter(deleter&& x)  ;
    explicit deleter(impl* i)  ;
    deleter(raw_object_tag tag, void* object)  ;
    ~deleter();
    deleter& operator=(deleter&& x) noexcept;
    deleter& operator=(deleter&) = delete;
    deleter share();
    explicit operator bool() const ;
    void reset(impl* i) ;
    void append(deleter d);
private:
    static bool is_raw_object(impl* i) {
        auto x = reinterpret_cast<uintptr_t>(i);
        return x & 1;
    }
    bool is_raw_object() const {
        return is_raw_object(_impl);
    }
    static void* to_raw_object(impl* i) {
        auto x = reinterpret_cast<uintptr_t>(i);
        return reinterpret_cast<void*>(x & ~uintptr_t(1));
    }
    void* to_raw_object() const {
        return to_raw_object(_impl);
    }
    impl* from_raw_object(void* object) {
        auto x = reinterpret_cast<uintptr_t>(object);
        return reinterpret_cast<impl*>(x | 1);
    }
};
struct deleter::impl {
    unsigned refs = 1;
    deleter next;
    impl(deleter next) : next(std::move(next)) {}
    virtual ~impl() {}
};
inline
deleter::~deleter() {
    if (is_raw_object()) {
        std::free(to_raw_object());
        return;
    }
    if (_impl && --_impl->refs == 0) {
        delete _impl;
    }
}
inline
deleter& deleter::operator=(deleter&& x) noexcept {
    if (this != &x) {
        this->~deleter();
        new (this) deleter(std::move(x));
    }
    return *this;
}
template <typename Deleter>
struct lambda_deleter_impl final : deleter::impl {
    Deleter del;
    lambda_deleter_impl(deleter next, Deleter&& del)
        : impl(std::move(next)), del(std::move(del)) {}
    virtual ~lambda_deleter_impl() override { del(); }
};
template <typename Object>
struct object_deleter_impl final : deleter::impl {
    Object obj;
    object_deleter_impl(deleter next, Object&& obj)
        : impl(std::move(next)), obj(std::move(obj)) {}
};
template <typename Object>
inline
object_deleter_impl<Object>* make_object_deleter_impl(deleter next, Object obj) {
    return new object_deleter_impl<Object>(std::move(next), std::move(obj));
}
template <typename Object>
deleter
make_deleter(deleter next, Object o) {
    return deleter(new lambda_deleter_impl<Object>(std::move(next), std::move(o)));
}
template <typename Object>
deleter
make_deleter(Object o) {
    return make_deleter(deleter(), std::move(o));
}
struct free_deleter_impl final : deleter::impl {
    void* obj;
    free_deleter_impl(void* obj) : impl(deleter()), obj(obj) {}
    virtual ~free_deleter_impl() override { std::free(obj); }
};
inline
deleter
deleter::share() {
    if (!_impl) {
        return deleter();
    }
    if (is_raw_object()) {
        _impl = new free_deleter_impl(to_raw_object());
    }
    ++_impl->refs;
    return deleter(_impl);
}
inline
void deleter::append(deleter d) {
    if (!d._impl) {
        return;
    }
    impl* next_impl = _impl;
    deleter* next_d = this;
    while (next_impl) {
        if (next_impl == d._impl) {
            return; 
        }
        if (is_raw_object(next_impl)) {
            next_d->_impl = next_impl = new free_deleter_impl(to_raw_object(next_impl));
        }
        if (next_impl->refs != 1) {
            next_d->_impl = next_impl = make_object_deleter_impl(deleter(next_impl), std::move(d));
            return;
        }
        next_d = &next_impl->next;
        next_impl = next_d->_impl;
    }
    next_d->_impl = d._impl;
    d._impl = nullptr;
}
inline
deleter
make_free_deleter(void* obj) {
    if (!obj) {
        return deleter();
    }
    return deleter(deleter::raw_object_tag(), obj);
}
inline
deleter
make_free_deleter(deleter next, void* obj) {
    return make_deleter(std::move(next), [obj] () mutable { std::free(obj); });
}
template <typename T>
inline
deleter
make_object_deleter(T&& obj) {
    return deleter{make_object_deleter_impl(deleter(), std::move(obj))};
}
template <typename T>
inline
deleter
make_object_deleter(deleter d, T&& obj) {
    return deleter{make_object_deleter_impl(std::move(d), std::move(obj))};
}
}
#include <algorithm>
namespace seastar {
template <typename CharType>
class temporary_buffer {
    static_assert(sizeof(CharType) == 1, "must buffer stream of bytes");
    CharType* _buffer;
    size_t _size;
    deleter _deleter;
public:
    explicit temporary_buffer(size_t size)
        : _buffer(static_cast<CharType*>(malloc(size * sizeof(CharType)))), _size(size)
        , _deleter(make_free_deleter(_buffer)) {
        if (size && !_buffer) {
            throw std::bad_alloc();
        }
    }
    temporary_buffer()
        : _buffer(nullptr)
        , _size(0) {}
    temporary_buffer(const temporary_buffer&) = delete;
    temporary_buffer(temporary_buffer&& x) noexcept : _buffer(x._buffer), _size(x._size), _deleter(std::move(x._deleter)) {
        x._buffer = nullptr;
        x._size = 0;
    }
    temporary_buffer(CharType* buf, size_t size, deleter d)
        : _buffer(buf), _size(size), _deleter(std::move(d)) {}
    temporary_buffer(const CharType* src, size_t size) : temporary_buffer(size) {
        std::copy_n(src, size, _buffer);
    }
    void operator=(const temporary_buffer&) = delete;
    temporary_buffer& operator=(temporary_buffer&& x) noexcept {
        if (this != &x) {
            _buffer = x._buffer;
            _size = x._size;
            _deleter = std::move(x._deleter);
            x._buffer = nullptr;
            x._size = 0;
        }
        return *this;
    }
    const CharType* get() const { return _buffer; }
    CharType* get_write() ;
    size_t size() const ;
    const CharType* begin() const ;
    const CharType* end() const ;
    temporary_buffer prefix(size_t size) && ;
    CharType operator[](size_t pos) const ;
    bool empty() const ;
    explicit operator bool() const ;
    temporary_buffer share() ;
    temporary_buffer share(size_t pos, size_t len) ;
    temporary_buffer clone() const ;
    void trim_front(size_t pos) ;
    void trim(size_t pos) ;
    deleter release() ;
    static temporary_buffer aligned(size_t alignment, size_t size) ;
    bool operator==(const temporary_buffer<char>& o) const ;
    bool operator!=(const temporary_buffer<char>& o) const ;
};
}
namespace seastar {
template <typename char_type, typename Size, Size max_size, bool NulTerminate = true>
class basic_sstring;
using sstring = basic_sstring<char, uint32_t, 15>;
template <typename string_type = sstring, typename T>
 string_type to_sstring(T value);
template <typename char_type, typename Size, Size max_size, bool NulTerminate>
class basic_sstring {
    static_assert(
            (std::is_same<char_type, char>::value
             || std::is_same<char_type, signed char>::value
             || std::is_same<char_type, unsigned char>::value),
            "basic_sstring only supports single byte char types");
    union contents {
        struct external_type {
            char_type* str;
            Size size;
            int8_t pad;
        } external;
        struct internal_type {
            char_type str[max_size];
            int8_t size;
        } internal;
        static_assert(sizeof(external_type) <= sizeof(internal_type), "max_size too small");
        static_assert(max_size <= 127, "max_size too large");
    } u;
    bool is_internal() const noexcept ;
    bool is_external() const noexcept ;
    const char_type* str() const ;
    char_type* str() ;
    template <typename string_type, typename T>
    static string_type to_sstring_sprintf(T value, const char* fmt) ;
    template <typename string_type>
    static string_type to_sstring(int value) ;
    template <typename string_type>
    static string_type to_sstring(unsigned value) ;
    template <typename string_type>
    static string_type to_sstring(long value) ;
    template <typename string_type>
    static string_type to_sstring(unsigned long value) ;
    template <typename string_type>
    static string_type to_sstring(long long value) ;
    template <typename string_type>
    static string_type to_sstring(unsigned long long value) ;
    template <typename string_type>
    static string_type to_sstring(float value) ;
    template <typename string_type>
    static string_type to_sstring(double value) ;
    template <typename string_type>
    static string_type to_sstring(long double value) ;
     ;
     ;
     ;
public:
    using value_type = char_type;
    using traits_type = std::char_traits<char_type>;
    using allocator_type = std::allocator<char_type>;
    using reference = char_type&;
    using const_reference = const char_type&;
    using pointer = char_type*;
    using const_pointer = const char_type*;
    using iterator = char_type*;
    using const_iterator = const char_type*;
    using difference_type = ssize_t;  
    using size_type = Size;
    static constexpr size_type  npos = static_cast<size_type>(-1);
    static constexpr unsigned padding() ;
public:
    struct initialized_later {};
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    operator std::basic_string<char_type>() const {
        return { str(), size() };
    }
    size_t size() const noexcept {
        return is_internal() ? u.internal.size : u.external.size;
    }
    size_t length() const noexcept {
        return size();
    }
    size_t find(char_type t, size_t pos = 0) const noexcept {
        const char_type* it = str() + pos;
        const char_type* end = str() + size();
        while (it < end) {
            if (*it == t) {
                return it - str();
            }
            it++;
        }
        return npos;
    }
    size_t find(const basic_sstring& s, size_t pos = 0) const noexcept {
        const char_type* it = str() + pos;
        const char_type* end = str() + size();
        const char_type* c_str = s.str();
        const char_type* c_str_end = s.str() + s.size();
        while (it < end) {
            auto i = it;
            auto j = c_str;
            while ( i < end && j < c_str_end && *i == *j) {
                i++;
                j++;
            }
            if (j == c_str_end) {
                return it - str();
            }
            it++;
        }
        return npos;
    }
    size_t find_last_of (char_type c, size_t pos = npos) const noexcept {
        const char_type* str_start = str();
        if (size()) {
            if (pos >= size()) {
                pos = size() - 1;
            }
            const char_type* p = str_start + pos + 1;
            do {
                p--;
                if (*p == c) {
                    return (p - str_start);
                }
            } while (p != str_start);
        }
        return npos;
    }
    basic_sstring& append (const char_type* s, size_t n) {
        basic_sstring ret(initialized_later(), size() + n);
        std::copy(begin(), end(), ret.begin());
        std::copy(s, s + n, ret.begin() + size());
        *this = std::move(ret);
        return *this;
    }
    void resize(size_t n, const char_type c  = '\0') {
        if (n > size()) {
            *this += basic_sstring(n - size(), c);
        } else if (n < size()) {
            if (is_internal()) {
                u.internal.size = n;
                if (NulTerminate) {
                    u.internal.str[n] = '\0';
                }
            } else if (n + padding() <= sizeof(u.internal.str)) {
                *this = basic_sstring(u.external.str, n);
            } else {
                u.external.size = n;
                if (NulTerminate) {
                    u.external.str[n] = '\0';
                }
            }
        }
    }
    basic_sstring& replace(size_type pos, size_type n1, const char_type* s,
             size_type n2) {
        if (pos > size()) {
            throw std::out_of_range("sstring::replace out of range");
        }
        if (n1 > size() - pos) {
            n1 = size() - pos;
        }
        if (n1 == n2) {
            if (n2) {
                std::copy(s, s + n2, begin() + pos);
            }
            return *this;
        }
        basic_sstring ret(initialized_later(), size() + n2 - n1);
        char_type* p= ret.begin();
        std::copy(begin(), begin() + pos, p);
        p += pos;
        if (n2) {
            std::copy(s, s + n2, p);
        }
        p += n2;
        std::copy(begin() + pos + n1, end(), p);
        *this = std::move(ret);
        return *this;
    }
    template <class InputIterator>
    basic_sstring& replace (const_iterator i1, const_iterator i2,
            InputIterator first, InputIterator last) {
        if (i1 < begin() || i1 > end() || i2 < begin()) {
            throw std::out_of_range("sstring::replace out of range");
        }
        if (i2 > end()) {
            i2 = end();
        }
        if (i2 - i1 == last - first) {
            std::copy(first, last, const_cast<char_type*>(i1));
            return *this;
        }
        basic_sstring ret(initialized_later(), size() + (last - first) - (i2 - i1));
        char_type* p = ret.begin();
        p = std::copy(cbegin(), i1, p);
        p = std::copy(first, last, p);
        std::copy(i2, cend(), p);
        *this = std::move(ret);
        return *this;
    }
    iterator erase(iterator first, iterator last) {
        size_t pos = first - begin();
        replace(pos, last - first, nullptr, 0);
        return begin() + pos;
    }
    template <class InputIterator>
    void insert(const_iterator p, InputIterator beg, InputIterator end) {
        replace(p, p, beg, end);
    }
    reference
    back() noexcept {
        return operator[](size() - 1);
    }
    const_reference
    back() const noexcept {
        return operator[](size() - 1);
    }
    basic_sstring substr(size_t from, size_t len = npos)  const {
        if (from > size()) {
            throw std::out_of_range("sstring::substr out of range");
        }
        if (len > size() - from) {
            len = size() - from;
        }
        if (len == 0) {
            return "";
        }
        return { str() + from , len };
    }
    const char_type& at(size_t pos) const {
        if (pos >= size()) {
            throw std::out_of_range("sstring::at out of range");
        }
        return *(str() + pos);
    }
    char_type& at(size_t pos) {
        if (pos >= size()) {
            throw std::out_of_range("sstring::at out of range");
        }
        return *(str() + pos);
    }
    bool empty() const noexcept {
        return u.internal.size == 0;
    }
    void reset() noexcept {
        if (is_external()) {
            std::free(u.external.str);
        }
        u.internal.size = 0;
        if (NulTerminate) {
            u.internal.str[0] = '\0';
        }
    }
    temporary_buffer<char_type> release() && {
        if (is_external()) {
            auto ptr = u.external.str;
            auto size = u.external.size;
            u.external.str = nullptr;
            u.external.size = 0;
            return temporary_buffer<char_type>(ptr, size, make_free_deleter(ptr));
        } else {
            auto buf = temporary_buffer<char_type>(u.internal.size);
            std::copy(u.internal.str, u.internal.str + u.internal.size, buf.get_write());
            u.internal.size = 0;
            if (NulTerminate) {
                u.internal.str[0] = '\0';
            }
            return buf;
        }
    }
    int compare(const basic_sstring& x) const noexcept {
        auto n = traits_type::compare(begin(), x.begin(), std::min(size(), x.size()));
        if (n != 0) {
            return n;
        }
        if (size() < x.size()) {
            return -1;
        } else if (size() > x.size()) {
            return 1;
        } else {
            return 0;
        }
    }
    int compare(size_t pos, size_t sz, const basic_sstring& x) const {
        if (pos > size()) {
            throw std::out_of_range("pos larger than string size");
        }
        sz = std::min(size() - pos, sz);
        auto n = traits_type::compare(begin() + pos, x.begin(), std::min(sz, x.size()));
        if (n != 0) {
            return n;
        }
        if (sz < x.size()) {
            return -1;
        } else if (sz > x.size()) {
            return 1;
        } else {
            return 0;
        }
    }
    void swap(basic_sstring& x) noexcept {
        contents tmp;
        tmp = x.u;
        x.u = u;
        u = tmp;
    }
    char_type* data() {
        return str();
    }
    const char_type* data() const {
        return str();
    }
    const char_type* c_str() const {
        return str();
    }
    const char_type* begin() const { return str(); }
    const char_type* end() const { return str() + size(); }
    const char_type* cbegin() const { return str(); }
    const char_type* cend() const { return str() + size(); }
    char_type* begin() { return str(); }
    char_type* end() { return str() + size(); }
    bool operator==(const basic_sstring& x) const {
        return size() == x.size() && std::equal(begin(), end(), x.begin());
    }
    bool operator!=(const basic_sstring& x) const {
        return !operator==(x);
    }
    bool operator<(const basic_sstring& x) const {
        return compare(x) < 0;
    }
    basic_sstring operator+(const basic_sstring& x) const {
        basic_sstring ret(initialized_later(), size() + x.size());
        std::copy(begin(), end(), ret.begin());
        std::copy(x.begin(), x.end(), ret.begin() + size());
        return ret;
    }
    basic_sstring& operator+=(const basic_sstring& x) {
        return *this = *this + x;
    }
    char_type& operator[](size_type pos) {
        return str()[pos];
    }
    const char_type& operator[](size_type pos) const {
        return str()[pos];
    }
    operator compat::basic_string_view<char_type>() const {
        return compat::basic_string_view<char_type>(str(), size());
    }
    template <typename string_type, typename T>
    friend inline string_type to_sstring(T value);
};
template <typename char_type, typename Size, Size max_size, bool NulTerminate>
constexpr Size basic_sstring<char_type, Size, max_size, NulTerminate>::npos;
template <typename char_type, typename size_type, size_type Max, size_type N, bool NulTerminate>

basic_sstring<char_type, size_type, Max, NulTerminate>
operator+(const char(&s)[N], const basic_sstring<char_type, size_type, Max, NulTerminate>& t) ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;

;
 ;
 ;
}
namespace std {
template <typename char_type, typename size_type, size_type max_size, bool NulTerminate>
struct hash<seastar::basic_sstring<char_type, size_type, max_size, NulTerminate>> {
    
};
}
namespace seastar {

 ;

;
template <typename string_type, typename T>
 string_type to_sstring(T value) ;
}
namespace std {
 ;
template <typename Key, typename T, typename Hash, typename KeyEqual, typename Allocator>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<Key, T, Hash, KeyEqual, Allocator>& v) ;
}
using namespace seastar;
#define GCC6_CONCEPT(x...)
#define GCC6_NO_CONCEPT(x...) x
GCC6_CONCEPT(template <typename H> concept bool Hasher() {
  return requires(H & h, const char *ptr, size_t size) {
    { h.update(ptr, size) }
    ->void;
  };
})
template <typename T, typename Enable = void>
struct appending_hash;
namespace seastar {
template<typename T>
struct is_smart_ptr : std::false_type {};
template<typename T>
struct is_smart_ptr<std::unique_ptr<T>> : std::true_type {};
}
namespace seastar {
template<typename Pointer, typename Equal = std::equal_to<typename std::pointer_traits<Pointer>::element_type>>
struct indirect_equal_to {
    Equal _eq;
    indirect_equal_to(Equal eq = Equal()) : _eq(std::move(eq)) {}
    bool operator()(const Pointer& i1, const Pointer& i2) const {
        if (bool(i1) ^ bool(i2)) {
            return false;
        }
        return !i1 || _eq(*i1, *i2);
    }
};
template<typename Pointer, typename Less = std::less<typename std::pointer_traits<Pointer>::element_type>>
struct indirect_less {
    Less _cmp;
    indirect_less(Less cmp = Less()) : _cmp(std::move(cmp)) {}
    bool operator()(const Pointer& i1, const Pointer& i2) const {
        if (i1 && i2) {
            return _cmp(*i1, *i2);
        }
        return !i1 && i2;
    }
};
template<typename Pointer, typename Hash = std::hash<typename std::pointer_traits<Pointer>::element_type>>
struct indirect_hash {
    Hash _h;
    indirect_hash(Hash h = Hash()) : _h(std::move(h)) {}
    size_t operator()(const Pointer& p) const {
        if (p) {
            return _h(*p);
        }
        return 0;
    }
};
}
#include <boost/intrusive/parent_from_member.hpp>
namespace seastar {
using shared_ptr_counter_type = long;
template <typename T>
class lw_shared_ptr;
template <typename T>
class shared_ptr;
template <typename T>
class enable_lw_shared_from_this;
template <typename T>
class enable_shared_from_this;
template <typename T, typename... A>
lw_shared_ptr<T> make_lw_shared(A&&... a);
template <typename T>
lw_shared_ptr<T> make_lw_shared(T&& a);
template <typename T>
lw_shared_ptr<T> make_lw_shared(T& a);
;
;
;
;
;
struct lw_shared_ptr_counter_base {
    shared_ptr_counter_type _count = 0;
};
namespace internal {
template <class T, class U>
struct lw_shared_ptr_accessors;
template <class T>
struct lw_shared_ptr_accessors_esft;
template <class T>
struct lw_shared_ptr_accessors_no_esft;
}
template <typename T>
class enable_lw_shared_from_this : private lw_shared_ptr_counter_base {
    using ctor = T;
protected:
    
    
    
    
    
public:
    lw_shared_ptr<T> shared_from_this();
    lw_shared_ptr<const T> shared_from_this() const;
    template <typename X>
    friend class lw_shared_ptr;
    template <typename X>
    friend struct internal::lw_shared_ptr_accessors_esft;
    template <typename X, class Y>
    friend struct internal::lw_shared_ptr_accessors;
};
template <typename T>
struct shared_ptr_no_esft : private lw_shared_ptr_counter_base {
    T _value;
    shared_ptr_no_esft() = default;
    shared_ptr_no_esft(const T& x) : _value(x) {}
    shared_ptr_no_esft(T&& x) : _value(std::move(x)) {}
    template <typename... A>
    shared_ptr_no_esft(A&&... a) : _value(std::forward<A>(a)...) {}
    template <typename X>
    friend class lw_shared_ptr;
    template <typename X>
    friend struct internal::lw_shared_ptr_accessors_no_esft;
    template <typename X, class Y>
    friend struct internal::lw_shared_ptr_accessors;
};
template <typename T>
struct lw_shared_ptr_deleter;  
namespace internal {
template <typename T>
struct lw_shared_ptr_accessors_esft {
    using concrete_type = std::remove_const_t<T>;
    static T* to_value(lw_shared_ptr_counter_base* counter) {
        return static_cast<T*>(counter);
    }
    static void dispose(lw_shared_ptr_counter_base* counter) ;
    static void dispose(T* value_ptr) ;
    
};
template <typename T>
struct lw_shared_ptr_accessors_no_esft {
    using concrete_type = shared_ptr_no_esft<T>;
    
    
    
    
};
template <typename T, typename U = void>
struct lw_shared_ptr_accessors : std::conditional_t<
         std::is_base_of<enable_lw_shared_from_this<T>, T>::value,
         lw_shared_ptr_accessors_esft<T>,
         lw_shared_ptr_accessors_no_esft<T>> {
};
template <typename... T>
using void_t = void;
template <typename T>
struct lw_shared_ptr_accessors<T, void_t<decltype(lw_shared_ptr_deleter<T>{})>> {
    using concrete_type = T;
    
    
    
};
}
template <typename T>
class lw_shared_ptr {
    using accessors = internal::lw_shared_ptr_accessors<std::remove_const_t<T>>;
    using concrete_type = typename accessors::concrete_type;
    mutable lw_shared_ptr_counter_base* _p = nullptr;
private:
    
     ;
public:
    using element_type = T;
    
    class disposer {
    public:
        
    };
    lw_shared_ptr() noexcept = default;
    lw_shared_ptr(std::nullptr_t) noexcept : lw_shared_ptr() {}
    lw_shared_ptr(const lw_shared_ptr& x) noexcept : _p(x._p) {
        if (_p) {
            ++_p->_count;
        }
    }
    lw_shared_ptr(lw_shared_ptr&& x) noexcept  : _p(x._p) {
        x._p = nullptr;
    }
    [[gnu::always_inline]]
    ~lw_shared_ptr() {
        if (_p && !--_p->_count) {
            accessors::dispose(_p);
        }
    }
    lw_shared_ptr& operator=(const lw_shared_ptr& x) noexcept {
        if (_p != x._p) {
            this->~lw_shared_ptr();
            new (this) lw_shared_ptr(x);
        }
        return *this;
    }
    lw_shared_ptr& operator=(lw_shared_ptr&& x) noexcept {
        if (_p != x._p) {
            this->~lw_shared_ptr();
            new (this) lw_shared_ptr(std::move(x));
        }
        return *this;
    }
    lw_shared_ptr& operator=(std::nullptr_t) noexcept {
        return *this = lw_shared_ptr();
    }
    lw_shared_ptr& operator=(T&& x) noexcept {
        this->~lw_shared_ptr();
        new (this) lw_shared_ptr(make_lw_shared<T>(std::move(x)));
        return *this;
    }
    T& operator*() const noexcept { return *accessors::to_value(_p); }
    T* operator->() const noexcept { return accessors::to_value(_p); }
    T* get() const noexcept {
        if (_p) {
            return accessors::to_value(_p);
        } else {
            return nullptr;
        }
    }
    std::unique_ptr<T, disposer> release() noexcept {
        auto p = std::exchange(_p, nullptr);
        if (--p->_count) {
            return nullptr;
        } else {
            return std::unique_ptr<T, disposer>(accessors::to_value(p));
        }
    }
    long int use_count() const noexcept {
        if (_p) {
            return _p->_count;
        } else {
            return 0;
        }
    }
    operator lw_shared_ptr<const T>() const noexcept {
        return lw_shared_ptr<const T>(_p);
    }
    explicit operator bool() const noexcept {
        return _p;
    }
    bool owned() const noexcept {
        return _p->_count == 1;
    }
    bool operator==(const lw_shared_ptr<const T>& x) const {
        return _p == x._p;
    }
    bool operator!=(const lw_shared_ptr<const T>& x) const {
        return !operator==(x);
    }
    bool operator==(const lw_shared_ptr<std::remove_const_t<T>>& x) const {
        return _p == x._p;
    }
    bool operator!=(const lw_shared_ptr<std::remove_const_t<T>>& x) const {
        return !operator==(x);
    }
    bool operator<(const lw_shared_ptr<const T>& x) const {
        return _p < x._p;
    }
    bool operator<(const lw_shared_ptr<std::remove_const_t<T>>& x) const {
        return _p < x._p;
    }
    template <typename U>
    friend class lw_shared_ptr;
    ;
    ;
    ;
    template <typename U>
    friend class enable_lw_shared_from_this;
};
template <typename T, typename... A>
inline
lw_shared_ptr<T> make_lw_shared(A&&... a) {
    return lw_shared_ptr<T>::make(std::forward<A>(a)...);
}
template <typename T>
inline
lw_shared_ptr<T> make_lw_shared(T&& a) {
    return lw_shared_ptr<T>::make(std::move(a));
}
template <typename T>
inline
lw_shared_ptr<T> make_lw_shared(T& a) {
    return lw_shared_ptr<T>::make(a);
}
template <typename T>
inline
lw_shared_ptr<T>
enable_lw_shared_from_this<T>::shared_from_this() {
    return lw_shared_ptr<T>(this);
}
template <typename T>
inline
lw_shared_ptr<const T>
enable_lw_shared_from_this<T>::shared_from_this() const {
    return lw_shared_ptr<const T>(const_cast<enable_lw_shared_from_this*>(this));
}

struct shared_ptr_count_base {
    virtual ~shared_ptr_count_base() {}
    shared_ptr_counter_type count = 0;
};
template <typename T>
struct shared_ptr_count_for : shared_ptr_count_base {
    T data;
    
};
template <typename T>
class enable_shared_from_this : private shared_ptr_count_base {
public:
    
    
    template <typename U>
    friend class shared_ptr;
    template <typename U, bool esft>
    friend struct shared_ptr_make_helper;
};
template <typename T>
class shared_ptr {
    mutable shared_ptr_count_base* _b = nullptr;
    mutable T* _p = nullptr;
private:
    
    shared_ptr(shared_ptr_count_base* b, T* p)  ;
    explicit shared_ptr(enable_shared_from_this<std::remove_const_t<T>>* p) noexcept : _b(p), _p(static_cast<T*>(p)) {
        if (_b) {
            ++_b->count;
        }
    }
public:
    using element_type = T;
    shared_ptr() noexcept = default;
    shared_ptr(std::nullptr_t) noexcept : shared_ptr() {}
    shared_ptr(const shared_ptr& x) noexcept
            : _b(x._b)
            , _p(x._p) {
        if (_b) {
            ++_b->count;
        }
    }
    shared_ptr(shared_ptr&& x) noexcept
            : _b(x._b)
            , _p(x._p) {
        x._b = nullptr;
        x._p = nullptr;
    }
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    shared_ptr(const shared_ptr<U>& x) noexcept
            : _b(x._b)
            , _p(x._p) {
        if (_b) {
            ++_b->count;
        }
    }
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    shared_ptr(shared_ptr<U>&& x) noexcept
            : _b(x._b)
            , _p(x._p) {
        x._b = nullptr;
        x._p = nullptr;
    }
    ~shared_ptr() {
        if (_b && !--_b->count) {
            delete _b;
        }
    }
    shared_ptr& operator=(const shared_ptr& x) noexcept {
        if (this != &x) {
            this->~shared_ptr();
            new (this) shared_ptr(x);
        }
        return *this;
    }
    shared_ptr& operator=(shared_ptr&& x) noexcept {
        if (this != &x) {
            this->~shared_ptr();
            new (this) shared_ptr(std::move(x));
        }
        return *this;
    }
    shared_ptr& operator=(std::nullptr_t) noexcept {
        return *this = shared_ptr();
    }
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    shared_ptr& operator=(const shared_ptr<U>& x) noexcept {
        if (*this != x) {
            this->~shared_ptr();
            new (this) shared_ptr(x);
        }
        return *this;
    }
    template <typename U, typename = std::enable_if_t<std::is_base_of<T, U>::value>>
    shared_ptr& operator=(shared_ptr<U>&& x) noexcept {
        if (*this != x) {
            this->~shared_ptr();
            new (this) shared_ptr(std::move(x));
        }
        return *this;
    }
    explicit operator bool() const noexcept {
        return _p;
    }
    T& operator*() const noexcept {
        return *_p;
    }
    T* operator->() const noexcept {
        return _p;
    }
    T* get() const noexcept {
        return _p;
    }
    long use_count() const noexcept {
        if (_b) {
            return _b->count;
        } else {
            return 0;
        }
    }
    template <bool esft>
    struct make_helper;
    ;
    ;
    ;
    ;
    ;
    ;
    template <typename U>
    friend class enable_shared_from_this;
    template <typename U, bool esft>
    friend struct shared_ptr_make_helper;
    template <typename U>
    friend class shared_ptr;
};
template <typename U, bool esft>
struct shared_ptr_make_helper;
template <typename T>
struct shared_ptr_make_helper<T, false> {
     ;
};
template <typename T>
struct shared_ptr_make_helper<T, true> {
     ;
};
 ;
 ;
 ;
 ;
 ;


template <typename T, typename U>

bool
operator==(const shared_ptr<T>& x, const shared_ptr<U>& y) ;
 ;
 ;
template <typename T, typename U>

bool
operator!=(const shared_ptr<T>& x, const shared_ptr<U>& y) ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
template <typename T>

bool
operator>=(std::nullptr_t, const shared_ptr<T>& y) ;
template <typename T>
static
std::ostream& operator<<(std::ostream& out, const shared_ptr<T>& p) ;
template<typename T>
using shared_ptr_equal_by_value = indirect_equal_to<shared_ptr<T>>;
template<typename T>
using shared_ptr_value_hash = indirect_hash<shared_ptr<T>>;
}
namespace std {
template <typename T>
struct hash<seastar::lw_shared_ptr<T>> : private hash<T*> {
    size_t operator()(const seastar::lw_shared_ptr<T>& p) const {
        return hash<T*>::operator()(p.get());
    }
};
template <typename T>
struct hash<seastar::shared_ptr<T>> : private hash<T*> {
    size_t operator()(const seastar::shared_ptr<T>& p) const {
        return hash<T*>::operator()(p.get());
    }
};
}
namespace seastar {
template<typename T>
struct is_smart_ptr<shared_ptr<T>> : std::true_type {};
template<typename T>
struct is_smart_ptr<lw_shared_ptr<T>> : std::true_type {};
}
using column_count_type = uint32_t;
using column_id = column_count_type;
class schema;
using schema_ptr = seastar::lw_shared_ptr<const schema>;
GCC6_CONCEPT(template <typename T> concept bool HasTriCompare =
                 requires(const T &t) {
                   { t.compare(t) }
                   ->int;
                 } &&
                 std::is_same<std::result_of_t<decltype (&T::compare)(T, T)>,
                              int>::value;)
template <typename T>
class with_relational_operators {
private:
  template <typename U>
  GCC6_CONCEPT(requires HasTriCompare<U>)
  int do_compare(const U &t) const;
};
#include <experimental/type_traits>
namespace seastar {
namespace stdx = std::experimental;
GCC6_CONCEPT(
template<typename T>
concept bool OptimizableOptional() {
    return stdx::is_default_constructible_v<T>
        && stdx::is_nothrow_move_assignable_v<T>
        && requires(const T& obj) {
            { bool(obj) } noexcept;
        };
}
)
template<typename T>
class optimized_optional {
    T _object;
public:
    optimized_optional() = default;
    optimized_optional(compat::nullopt_t) noexcept { }
    optimized_optional(const T& obj) : _object(obj) { }
    optimized_optional(T&& obj) noexcept : _object(std::move(obj)) { }
    optimized_optional(compat::optional<T>&& obj) noexcept {
        if (obj) {
            _object = std::move(*obj);
        }
    }
    optimized_optional(const optimized_optional&) = default;
    optimized_optional(optimized_optional&&) = default;
    optimized_optional& operator=(compat::nullopt_t) noexcept ;
    template<typename U>
    std::enable_if_t<std::is_same<std::decay_t<U>, T>::value, optimized_optional&>
    operator=(U&& obj) noexcept {
        _object = std::forward<U>(obj);
        return *this;
    }
    optimized_optional& operator=(const optimized_optional&) = default;
    optimized_optional& operator=(optimized_optional&&) = default;
    explicit operator bool() const noexcept ;
    T* operator->() noexcept ;
    const T* operator->() const noexcept { return &_object; }
    T& operator*() noexcept { return _object; }
    const T& operator*() const noexcept { return _object; }
    bool operator==(const optimized_optional& other) const {
        return _object == other._object;
    }
    bool operator!=(const optimized_optional& other) const {
        return _object != other._object;
    }
    friend std::ostream& operator<<(std::ostream& out, const optimized_optional& opt) {
        if (!opt) {
            return out << "null";
        }
        return out << *opt;
    }
};
}
using bytes = basic_sstring<int8_t, uint32_t, 31, false>;
template <typename CharOutputIterator>
GCC6_CONCEPT(requires requires(CharOutputIterator it) {
  *it++ = 'a';
  *it++ = 'a';
})
inline void serialize_string(CharOutputIterator &out, const char *s) {
  auto len = strlen(s);
  out = std::copy_n(s, len, out);
}
namespace utils {
class UUID {
private:
  int64_t most_sig_bits;
  int64_t least_sig_bits;
public:
  ;
};
} 
class abstract_type;
using cql_protocol_version_type = uint8_t;
class cql_serialization_format {
  cql_protocol_version_type _version;
public:
  static constexpr cql_protocol_version_type latest_version = 4;
  explicit cql_serialization_format(cql_protocol_version_type version)
      : _version(version) {}
  static cql_serialization_format latest() {
    return cql_serialization_format{latest_version};
  }
  static cql_serialization_format internal() { return latest(); }
};
namespace utils {
template <typename T, size_t N> class small_vector {
  static_assert(N > 0);
  static_assert(std::is_nothrow_move_constructible_v<T>);
  static_assert(std::is_nothrow_move_assignable_v<T>);
  static_assert(std::is_nothrow_destructible_v<T>);
private:
  T *_begin;
  T *_end;
  T *_capacity_end;
  union internal {
    internal();
    ~internal();
    T storage[N];
  };
  internal _internal;
private:
  bool uses_internal_storage() const noexcept {
    return _begin == _internal.storage;
  }
  [[gnu::cold]] [[gnu::noinline]] void expand(size_t new_capacity) {
    auto ptr =
        static_cast<T *>(::aligned_alloc(alignof(T), new_capacity * sizeof(T)));
    if (!ptr) {
      throw std::bad_alloc();
    }
    auto n_end = std::uninitialized_move(begin(), end(), ptr);
    std::destroy(begin(), end());
    if (!uses_internal_storage()) {
      std::free(_begin);
    }
    _begin = ptr;
    _end = n_end;
    _capacity_end = ptr + new_capacity;
  }
  [[gnu::cold]] [[gnu::noinline]] void
  slow_copy_assignment(const small_vector &other) {
    auto ptr =
        static_cast<T *>(::aligned_alloc(alignof(T), other.size() * sizeof(T)));
    if (!ptr) {
      throw std::bad_alloc();
    }
    auto n_end = ptr;
    try {
      n_end = std::uninitialized_copy(other.begin(), other.end(), n_end);
    } catch (...) {
      std::free(ptr);
      throw;
    }
    std::destroy(begin(), end());
    if (!uses_internal_storage()) {
      std::free(_begin);
    }
    _begin = ptr;
    _end = n_end;
    _capacity_end = n_end;
  }
  void reserve_at_least(size_t n) {
    if (__builtin_expect(_begin + n > _capacity_end, false)) {
      expand(std::max(n, capacity() * 2));
    }
  }
  [[noreturn]] [[gnu::cold]] [[gnu::noinline]] void throw_out_of_range() {
    throw std::out_of_range("out of range small vector access");
  }
public:
  using value_type = T;
  using pointer = T *;
  using const_pointer = const T *;
  using reference = T &;
  using const_reference = const T &;
  using iterator = T *;
  using const_iterator = const T *;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  small_vector() noexcept
      : _begin(_internal.storage), _end(_begin), _capacity_end(_begin + N) {}
  template <typename InputIterator>
  small_vector(InputIterator first, InputIterator last) : small_vector() {
    if constexpr (std::is_base_of_v<std::forward_iterator_tag,
                                    typename std::iterator_traits<
                                        InputIterator>::iterator_category>) {
      reserve(std::distance(first, last));
      _end = std::uninitialized_copy(first, last, _end);
    } else {
      std::copy(first, last, std::back_inserter(*this));
    }
  }
  small_vector(std::initializer_list<T> list)
      : small_vector(list.begin(), list.end()) {}
  small_vector(small_vector &&other) noexcept {
    if (other.uses_internal_storage()) {
      _begin = _internal.storage;
      _capacity_end = _begin + N;
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(_internal.storage, other._internal.storage, N * sizeof(T));
        _end = _begin + other.size();
      } else {
        _end = _begin;
        for (auto &e : other) {
          new (_end++) T(std::move(e));
          e.~T();
        }
      }
      other._end = other._internal.storage;
    } else {
      _begin = std::exchange(other._begin, other._internal.storage);
      _end = std::exchange(other._end, other._internal.storage);
      _capacity_end =
          std::exchange(other._capacity_end, other._internal.storage + N);
    }
  }
  small_vector(const small_vector &other) noexcept : small_vector() {
    reserve(other.size());
    _end = std::uninitialized_copy(other.begin(), other.end(), _end);
  }
  small_vector &operator=(small_vector &&other) noexcept {
    clear();
    if (other.uses_internal_storage()) {
      if (__builtin_expect(!uses_internal_storage(), false)) {
        std::free(_begin);
        _begin = _internal.storage;
      }
      _capacity_end = _begin + N;
      if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(_internal.storage, other._internal.storage, N * sizeof(T));
        _end = _begin + other.size();
      } else {
        _end = _begin;
        for (auto &e : other) {
          new (_end++) T(std::move(e));
          e.~T();
        }
      }
      other._end = other._internal.storage;
    } else {
      if (__builtin_expect(!uses_internal_storage(), false)) {
        std::free(_begin);
      }
      _begin = std::exchange(other._begin, other._internal.storage);
      _end = std::exchange(other._end, other._internal.storage);
      _capacity_end =
          std::exchange(other._capacity_end, other._internal.storage + N);
    }
    return *this;
  }
  small_vector &operator=(const small_vector &other) {
    if constexpr (std::is_nothrow_copy_constructible_v<T>) {
      if (capacity() >= other.size()) {
        clear();
        _end = std::uninitialized_copy(other.begin(), other.end(), _end);
        return *this;
      }
    }
    slow_copy_assignment(other);
    return *this;
  }
  ~small_vector() {
    clear();
    if (__builtin_expect(!uses_internal_storage(), false)) {
      std::free(_begin);
    }
  }
  void reserve(size_t n) {
    if (__builtin_expect(_begin + n > _capacity_end, false)) {
      expand(n);
    }
  }
  void clear() noexcept {
    std::destroy(_begin, _end);
    _end = _begin;
  }
  iterator begin() noexcept { return _begin; }
  const_iterator begin() const noexcept { return _begin; }
  const_iterator cbegin() const noexcept { return _begin; }
  iterator end() noexcept { return _end; }
  const_iterator end() const noexcept { return _end; }
  const_iterator cend() const noexcept { return _end; }
  reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(begin());
  }
  T *data() noexcept { return _begin; }
  const T *data() const noexcept { return _begin; }
  T &front() noexcept { return *begin(); }
  const T &front() const noexcept { return *begin(); }
  T &back() noexcept { return end()[-1]; }
  const T &back() const noexcept { return end()[-1]; }
  T &operator[](size_t idx) noexcept { return data()[idx]; }
  const T &operator[](size_t idx) const noexcept { return data()[idx]; }
  T &at(size_t idx) {
    if (__builtin_expect(idx >= size(), false)) {
      throw_out_of_range();
    }
    return operator[](idx);
  }
  const T &at(size_t idx) const {
    if (__builtin_expect(idx >= size(), false)) {
      throw_out_of_range();
    }
    return operator[](idx);
  }
  bool empty() const noexcept { return _begin == _end; }
  size_t size() const noexcept { return _end - _begin; }
  size_t capacity() const noexcept { return _capacity_end - _begin; }
  template <typename... Args> T &emplace_back(Args &&... args) {
    if (__builtin_expect(_end == _capacity_end, false)) {
      expand(std::max<size_t>(capacity() * 2, 1));
    }
    auto &ref = *new (_end) T(std::forward<Args>(args)...);
    ++_end;
    return ref;
  }
  T &push_back(const T &value) { return emplace_back(value); }
  T &push_back(T &&value) { return emplace_back(std::move(value)); }
  template <typename InputIterator>
  iterator insert(const_iterator cpos, InputIterator first,
                  InputIterator last) {
    if constexpr (std::is_base_of_v<std::forward_iterator_tag,
                                    typename std::iterator_traits<
                                        InputIterator>::iterator_category>) {
      if (first == last) {
        return const_cast<iterator>(cpos);
      }
      auto idx = cpos - _begin;
      auto new_count = std::distance(first, last);
      reserve_at_least(size() + new_count);
      auto pos = _begin + idx;
      auto after = std::distance(pos, end());
      if (__builtin_expect(pos == end(), true)) {
        _end = std::uninitialized_copy(first, last, end());
        return pos;
      } else if (after > new_count) {
        std::uninitialized_move(end() - new_count, end(), end());
        std::move_backward(pos, end() - new_count, end());
        try {
          std::copy(first, last, pos);
        } catch (...) {
          std::move(pos + new_count, end() + new_count, pos);
          std::destroy(end(), end() + new_count);
          throw;
        }
      } else {
        std::uninitialized_move(pos, end(), pos + new_count);
        auto mid = std::next(first, after);
        try {
          std::uninitialized_copy(mid, last, end());
          try {
            std::copy(first, mid, pos);
          } catch (...) {
            std::destroy(end(), pos + new_count);
            throw;
          }
        } catch (...) {
          std::move(pos + new_count, end() + new_count, pos);
          std::destroy(pos + new_count, end() + new_count);
          throw;
        }
      }
      _end += new_count;
      return pos;
    } else {
      auto start = cpos - _begin;
      auto idx = start;
      while (first != last) {
        try {
          insert(begin() + idx, *first);
          ++first;
          ++idx;
        } catch (...) {
          erase(begin() + start, begin() + idx);
          throw;
        }
      }
      return begin() + idx;
    }
  }
  template <typename... Args>
  iterator emplace(const_iterator cpos, Args &&... args) {
    auto idx = cpos - _begin;
    reserve_at_least(size() + 1);
    auto pos = _begin + idx;
    if (pos != _end) {
      new (_end) T(std::move(_end[-1]));
      std::move_backward(pos, _end - 1, _end);
      pos->~T();
    }
    try {
      new (pos) T(std::forward<Args>(args)...);
    } catch (...) {
      if (pos != _end) {
        new (pos) T(std::move(pos[1]));
        std::move(pos + 2, _end + 1, pos + 1);
        _end->~T();
      }
      throw;
    }
    _end++;
    return pos;
  }
  iterator insert(const_iterator cpos, const T &obj) {
    return emplace(cpos, obj);
  }
  iterator insert(const_iterator cpos, T &&obj) {
    return emplace(cpos, std::move(obj));
  }
  void resize(size_t n) {
    if (n < size()) {
      erase(end() - (size() - n), end());
    } else if (n > size()) {
      reserve_at_least(n);
      _end = std::uninitialized_value_construct_n(_end, n - size());
    }
  }
  void resize(size_t n, const T &value) {
    if (n < size()) {
      erase(end() - (size() - n), end());
    } else if (n > size()) {
      reserve_at_least(n);
      auto nend = _begin + n;
      std::uninitialized_fill(_end, nend, value);
      _end = nend;
    }
  }
  void pop_back() noexcept { (--_end)->~T(); }
  iterator erase(const_iterator cit) noexcept { return erase(cit, cit + 1); }
  iterator erase(const_iterator cfirst, const_iterator clast) noexcept {
    auto first = const_cast<iterator>(cfirst);
    auto last = const_cast<iterator>(clast);
    std::move(last, end(), first);
    auto nend = _end - (clast - cfirst);
    std::destroy(nend, _end);
    _end = nend;
    return first;
  }
  void swap(small_vector &other) noexcept { std::swap(*this, other); }
  bool operator==(const small_vector &other) const noexcept {
    return size() == other.size() && std::equal(_begin, _end, other.begin());
  }
  bool operator!=(const small_vector &other) const noexcept {
    return !(*this == other);
  }
};
} 
#include <list>
namespace seastar {
template <typename Value, size_t Max>
class array_map {
    std::array<Value, Max> _a {};
public:
    array_map(std::initializer_list<std::pair<size_t, Value>> i) {
        for (auto kv : i) {
            _a[kv.first] = kv.second;
        }
    }
    Value& operator[](size_t key) { return _a[key]; }
    const Value& operator[](size_t key) const { return _a[key]; }
    Value& at(size_t key) {
        if (key >= Max) {
            throw std::out_of_range(std::to_string(key) + " >= " + std::to_string(Max));
        }
        return _a[key];
    }
};
}
#include <arpa/inet.h>  
namespace seastar {
template <typename T>
struct unaligned {
    T raw;
    unaligned() = default;
    unaligned(T x)  ;
    unaligned& operator=(const T& x) ;
    operator T() const ;
} __attribute__((packed));
template <typename T, typename F>
 auto unaligned_cast(F* p) ;
 ;
}
namespace seastar {
 
 
namespace net {
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
template <typename T> using packed = unaligned<T>;
 ;
 ;
 ;



;
 ;
 ;
 ;
 ;
}
}
namespace seastar {
 uint8_t cpu_to_le(uint8_t x) ;
 uint8_t le_to_cpu(uint8_t x) ;
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 ;
 ;


template <typename T>
inline
T
read_be(const char* p) {
    T datum;
    std::copy_n(p, sizeof(T), reinterpret_cast<char*>(&datum));
    return be_to_cpu(datum);
}
template <typename T>
inline
void
write_be(char* p, T datum) {
    datum = cpu_to_be(datum);
    std::copy_n(reinterpret_cast<const char*>(&datum), sizeof(T), p);
}


}
namespace seastar {
template <typename Func, typename Args, typename IndexList>
struct apply_helper;
template <typename Func, typename Tuple, size_t... I>
struct apply_helper<Func, Tuple, std::index_sequence<I...>> {
    static auto apply(Func&& func, Tuple args) {
        return func(std::get<I>(std::forward<Tuple>(args))...);
    }
};


template <typename Func, typename... T>
inline
auto apply(Func&& func, const std::tuple<T...>& args) {
    using helper = apply_helper<Func, const std::tuple<T...>&, std::index_sequence_for<T...>>;
    return helper::apply(std::forward<Func>(func), args);
}
}
#include <typeindex>
namespace seastar {
template<typename T>
struct function_traits;
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
    using return_type = Ret;
    using args_as_tuple = std::tuple<Args...>;
    using signature = Ret (Args...);
    static constexpr std::size_t arity = sizeof...(Args);
    template <std::size_t N>
    struct arg
    {
        static_assert(N < arity, "no such parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : public function_traits<Ret(Args...)>
{};
template <typename T, typename Ret, typename... Args>
struct function_traits<Ret(T::*)(Args...)> : public function_traits<Ret(Args...)>
{};
template <typename T, typename Ret, typename... Args>
struct function_traits<Ret(T::*)(Args...) const> : public function_traits<Ret(Args...)>
{};
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{};
template<typename T>
struct function_traits<T&> : public function_traits<std::remove_reference_t<T>>
{};
}
namespace seastar {
constexpr unsigned max_scheduling_groups() { return 16; }
template <typename... T>
class future;
class reactor;
class scheduling_group;
namespace internal {
unsigned scheduling_group_index(scheduling_group sg);

}



struct scheduling_group_key_config {
    
    
    size_t allocation_size;
    size_t alignment;
    std::type_index type_index;
    std::function<void (void*)> constructor;
    std::function<void (void*)> destructor;
};
class scheduling_group_key {
public:
    
private:
    
    unsigned long _id;
    
    friend class reactor;
    
    ;
    ;
};
namespace internal {

}


;
class scheduling_group {
    unsigned _id;
private:
    
public:
    constexpr scheduling_group() noexcept : _id(0) {} 
    ;
    
    bool operator==(scheduling_group x) const ;
    
    bool is_main() const ;
    template<typename T>
    T& get_specific(scheduling_group_key key) ;
    void set_shares(float shares);
    friend future<scheduling_group> create_scheduling_group(sstring name, float shares);
    
    friend future<> rename_scheduling_group(scheduling_group sg, sstring new_name);
    friend class reactor;
    friend unsigned internal::scheduling_group_index(scheduling_group sg);
    
    template<typename SpecificValType, typename Mapper, typename Reducer, typename Initial>
    GCC6_CONCEPT( requires requires(SpecificValType specific_val, Mapper mapper, Reducer reducer, Initial initial) {
        {reducer(initial, mapper(specific_val))} -> Initial;
    })
    friend future<typename function_traits<Reducer>::return_type>
    map_reduce_scheduling_group_specific(Mapper mapper, Reducer reducer, Initial initial_val, scheduling_group_key key);
    template<typename SpecificValType, typename Reducer, typename Initial>
    GCC6_CONCEPT( requires requires(SpecificValType specific_val, Reducer reducer, Initial initial) {
        {reducer(initial, specific_val)} -> Initial;
    })
    friend future<typename function_traits<Reducer>::return_type>
        reduce_scheduling_group_specific(Reducer reducer, Initial initial_val, scheduling_group_key key);
};
namespace internal {

unsigned
scheduling_group_index(scheduling_group sg) ;



scheduling_group*
current_scheduling_group_ptr() ;
}
inline
scheduling_group
current_scheduling_group() {
    return *internal::current_scheduling_group_ptr();
}
inline
scheduling_group
default_scheduling_group() {
    return scheduling_group();
}

}
namespace std {
template <>
struct hash<seastar::scheduling_group> {
    size_t operator()(seastar::scheduling_group sg) const {
        return seastar::internal::scheduling_group_index(sg);
    }
};
}
namespace seastar {
class task {
    scheduling_group _sg;
protected:
    ~task() = default;
public:
    explicit task(scheduling_group sg = current_scheduling_group()) : _sg(sg) {}
    virtual void run_and_dispose() noexcept = 0;
    scheduling_group group() const { return _sg; }
};
void schedule(task* t) noexcept;
void schedule_urgent(task* t) noexcept;
template <typename Func>
class lambda_task final : public task {
    Func _func;
public:
    lambda_task(scheduling_group sg, const Func& func) : task(sg), _func(func) {}
    lambda_task(scheduling_group sg, Func&& func) : task(sg), _func(std::move(func)) {}
    virtual void run_and_dispose() noexcept override {
        _func();
        delete this;
    }
};
template <typename Func>
inline
task*
make_task(Func&& func) noexcept {
    return new lambda_task<Func>(current_scheduling_group(), std::forward<Func>(func));
}
template <typename Func>
inline
task*
make_task(scheduling_group sg, Func&& func) noexcept {
    return new lambda_task<Func>(sg, std::forward<Func>(func));
}
}
namespace seastar {
namespace internal {
struct preemption_monitor {
    std::atomic<uint32_t> head;
    std::atomic<uint32_t> tail;
};
}
extern __thread const internal::preemption_monitor* g_need_preempt;
inline bool need_preempt() noexcept {
    std::atomic_signal_fence(std::memory_order_seq_cst);
    auto np = g_need_preempt;
    auto head = np->head.load(std::memory_order_relaxed);
    auto tail = np->tail.load(std::memory_order_relaxed);
    return __builtin_expect(head != tail, false);
}
}
#include <setjmp.h>
#include <ucontext.h>
namespace seastar {
using thread_clock = std::chrono::steady_clock;
class thread_context;
class scheduling_group;
struct jmp_buf_link {
    jmp_buf jmpbuf;
    jmp_buf_link* link;
    thread_context* thread;
public:
    void initial_switch_in(ucontext_t* initial_context, const void* stack_bottom, size_t stack_size);
    void switch_in();
    void switch_out();
    void initial_switch_in_completed();
    void final_switch_out();
};
extern thread_local jmp_buf_link* g_current_context;
namespace thread_impl {
inline thread_context* get() {
    return g_current_context->thread;
}
 bool should_yield() ;
scheduling_group sched_group(const thread_context*);
void yield();
void switch_in(thread_context* to);
void switch_out(thread_context* from);
void init();
}
}
#include <assert.h>
namespace seastar {
namespace internal {
template<typename T>
struct used_size {
    static constexpr size_t value = std::is_empty<T>::value ? 0 : sizeof(T);
};
}
}
namespace seastar {
template <typename Signature>
class noncopyable_function;
namespace internal {
class noncopyable_function_base {
private:
    noncopyable_function_base() = default;
    static constexpr size_t nr_direct = 32;
    union [[gnu::may_alias]] storage {
        char direct[nr_direct];
        void* indirect;
    };
    using move_type = void (*)(noncopyable_function_base* from, noncopyable_function_base* to);
    using destroy_type = void (*)(noncopyable_function_base* func);
    static void empty_move(noncopyable_function_base* from, noncopyable_function_base* to) ;
    static void empty_destroy(noncopyable_function_base* func) ;
    static void indirect_move(noncopyable_function_base* from, noncopyable_function_base* to) ;
    template <size_t N>
    static void trivial_direct_move(noncopyable_function_base* from, noncopyable_function_base* to) ;
    static void trivial_direct_destroy(noncopyable_function_base* func) ;
private:
    storage _storage;
    template <typename Signature>
    friend class seastar::noncopyable_function;
};
}
template <typename Ret, typename... Args>
class noncopyable_function<Ret (Args...)> : private internal::noncopyable_function_base {
    using call_type = Ret (*)(const noncopyable_function* func, Args...);
    struct vtable {
        const call_type call;
        const move_type move;
        const destroy_type destroy;
    };
private:
    const vtable* _vtable;
private:
    static Ret empty_call(const noncopyable_function* func, Args... args) ;
    static constexpr vtable _s_empty_vtable = {empty_call, empty_move, empty_destroy};
    template <typename Func>
    struct direct_vtable_for {
        static Func* access(noncopyable_function* func) { return reinterpret_cast<Func*>(func->_storage.direct); }
        static const Func* access(const noncopyable_function* func) { return reinterpret_cast<const Func*>(func->_storage.direct); }
        static Func* access(noncopyable_function_base* func) { return access(static_cast<noncopyable_function*>(func)); }
        static Ret call(const noncopyable_function* func, Args... args) {
            return (*access(const_cast<noncopyable_function*>(func)))(std::forward<Args>(args)...);
        }
        static void move(noncopyable_function_base* from, noncopyable_function_base* to) {
            new (access(to)) Func(std::move(*access(from)));
            destroy(from);
        }
        static constexpr move_type select_move_thunk() {
            bool can_trivially_move = std::is_trivially_move_constructible<Func>::value
                    && std::is_trivially_destructible<Func>::value;
            return can_trivially_move ? trivial_direct_move<internal::used_size<Func>::value> : move;
        }
        static void destroy(noncopyable_function_base* func) {
            access(func)->~Func();
        }
        static constexpr destroy_type select_destroy_thunk() {
            return std::is_trivially_destructible<Func>::value ? trivial_direct_destroy : destroy;
        }
        static void initialize(Func&& from, noncopyable_function* to) {
            new (access(to)) Func(std::move(from));
        }
        static constexpr vtable make_vtable() { return { call, select_move_thunk(), select_destroy_thunk() }; }
        static const vtable s_vtable;
    };
    template <typename Func>
    struct indirect_vtable_for {
        static Func* access(noncopyable_function* func) { return reinterpret_cast<Func*>(func->_storage.indirect); }
        static const Func* access(const noncopyable_function* func) { return reinterpret_cast<const Func*>(func->_storage.indirect); }
        static Func* access(noncopyable_function_base* func) { return access(static_cast<noncopyable_function*>(func)); }
        static Ret call(const noncopyable_function* func, Args... args) {
            return (*access(const_cast<noncopyable_function*>(func)))(std::forward<Args>(args)...);
        }
        static void destroy(noncopyable_function_base* func) {
            delete access(func);
        }
        static void initialize(Func&& from, noncopyable_function* to) {
            to->_storage.indirect = new Func(std::move(from));
        }
        static constexpr vtable make_vtable() { return { call, indirect_move, destroy }; }
        static const vtable s_vtable;
    };
    template <typename Func, bool Direct = true>
    struct select_vtable_for : direct_vtable_for<Func> {};
    template <typename Func>
    struct select_vtable_for<Func, false> : indirect_vtable_for<Func> {};
    template <typename Func>
    static constexpr bool is_direct() {
        return sizeof(Func) <= nr_direct && alignof(Func) <= alignof(storage)
                && std::is_nothrow_move_constructible<Func>::value;
    }
    template <typename Func>
    struct vtable_for : select_vtable_for<Func, is_direct<Func>()> {};
public:
    noncopyable_function() noexcept : _vtable(&_s_empty_vtable) {}
    template <typename Func>
    noncopyable_function(Func func) {
        vtable_for<Func>::initialize(std::move(func), this);
        _vtable = &vtable_for<Func>::s_vtable;
    }
    template <typename Object, typename... AllButFirstArg>
    noncopyable_function(Ret (Object::*member)(AllButFirstArg...)) : noncopyable_function(std::mem_fn(member)) {}
    
    
    
    
    
    
    
    
};
template <typename Ret, typename... Args>
constexpr typename noncopyable_function<Ret (Args...)>::vtable noncopyable_function<Ret (Args...)>::_s_empty_vtable;
template <typename Ret, typename... Args>
template <typename Func>
const typename noncopyable_function<Ret (Args...)>::vtable noncopyable_function<Ret (Args...)>::direct_vtable_for<Func>::s_vtable
        = noncopyable_function<Ret (Args...)>::direct_vtable_for<Func>::make_vtable();
template <typename Ret, typename... Args>
template <typename Func>
const typename noncopyable_function<Ret (Args...)>::vtable noncopyable_function<Ret (Args...)>::indirect_vtable_for<Func>::s_vtable
        = noncopyable_function<Ret (Args...)>::indirect_vtable_for<Func>::make_vtable();
}
namespace seastar {
namespace memory {
class alloc_failure_injector {
    uint64_t _alloc_count;
    uint64_t _fail_at = std::numeric_limits<uint64_t>::max();
    noncopyable_function<void()> _on_alloc_failure = [] { throw std::bad_alloc(); };
    bool _failed;
    uint64_t _suppressed = 0;
    friend struct disable_failure_guard;
private:
    
public:
    
    
    
    
    
    
};
extern thread_local alloc_failure_injector the_alloc_failure_injector;


struct disable_failure_guard {
    ~disable_failure_guard() ;
};


}
}
    #define SEASTAR_NODISCARD [[nodiscard]]
namespace seastar {
template <class... T>
class promise;
template <class... T>
class future;
template <typename... T>
class shared_future;
struct future_state_base;
template <typename... T, typename... A>
future<T...> make_ready_future(A&&... value);
;
 ;
 ;
void engine_exit(std::exception_ptr eptr = {});
void report_failed_future(const std::exception_ptr& ex) noexcept;
void report_failed_future(const future_state_base& state) noexcept;
struct broken_promise : std::logic_error {
    broken_promise();
};
namespace internal {
template <class... T>
class promise_base_with_type;
template <typename... T>
future<T...> current_exception_as_future() noexcept;
extern template
future<> current_exception_as_future() noexcept;
template <typename... T>
struct get0_return_type {
    using type = void;
    static type get0(std::tuple<T...> v) ;
};
template <typename T0, typename... T>
struct get0_return_type<T0, T...> {
    using type = T0;
    static type get0(std::tuple<T0, T...> v) ;
};
template <typename T, bool is_trivial_class>
struct uninitialized_wrapper_base;
template <typename T>
struct uninitialized_wrapper_base<T, false> {
    union any {
        any() ;
        ~any() ;
        T value;
    } _v;
public:
    void uninitialized_set(T&& v) ;
    T& uninitialized_get() ;
    const T& uninitialized_get() const ;
};
template <typename T> struct uninitialized_wrapper_base<T, true> : private T {
    void uninitialized_set(T&& v) ;
    T& uninitialized_get() ;
    const T& uninitialized_get() const ;
};
template <typename T>
constexpr bool can_inherit =
        std::is_same<std::tuple<>, T>::value ||
        (std::is_trivially_destructible<T>::value && std::is_trivially_constructible<T>::value &&
                std::is_class<T>::value && !std::is_final<T>::value);
template <typename T>
struct uninitialized_wrapper
    : public uninitialized_wrapper_base<T, can_inherit<T>> {};
static_assert(std::is_empty<uninitialized_wrapper<std::tuple<>>>::value, "This should still be empty");
template <typename T>
struct is_trivially_move_constructible_and_destructible {
    static constexpr bool value = std::is_trivially_move_constructible<T>::value && std::is_trivially_destructible<T>::value;
};
template <bool... v>
struct all_true : std::false_type {};
template <>
struct all_true<> : std::true_type {};
template <bool... v>
struct all_true<true, v...> : public all_true<v...> {};
}
struct future_state_base {
    static_assert(std::is_nothrow_copy_constructible<std::exception_ptr>::value,
                  "std::exception_ptr's copy constructor must not throw");
    static_assert(std::is_nothrow_move_constructible<std::exception_ptr>::value,
                  "std::exception_ptr's move constructor must not throw");
    static_assert(sizeof(std::exception_ptr) == sizeof(void*), "exception_ptr not a pointer");
    enum class state : uintptr_t {
         invalid = 0,
         future = 1,
         result_unavailable = 2,
         result = 3,
         exception_min = 4,  
    };
    union any {
        any() ;
        any(state s) ;
        void set_exception(std::exception_ptr&& e) ;
        any(std::exception_ptr&& e) {
            set_exception(std::move(e));
        }
        ~any() {}
        std::exception_ptr take_exception() {
            std::exception_ptr ret(std::move(ex));
            ex.~exception_ptr();
            st = state::invalid;
            return ret;
        }
        any(any&& x) {
            if (x.st < state::exception_min) {
                st = x.st;
                x.st = state::invalid;
            } else {
                new (&ex) std::exception_ptr(x.take_exception());
            }
        }
        bool has_result() const {
            return st == state::result || st == state::result_unavailable;
        }
        state st;
        std::exception_ptr ex;
    } _u;
    future_state_base() noexcept { }
    future_state_base(state st) noexcept : _u(st) { }
    future_state_base(std::exception_ptr&& ex) noexcept : _u(std::move(ex)) { }
    future_state_base(future_state_base&& x) noexcept : _u(std::move(x._u)) { }
protected:
    ~future_state_base() noexcept {
        if (failed()) {
            report_failed_future(_u.take_exception());
        }
    }
public:
    bool valid() const noexcept { return _u.st != state::invalid; }
    bool available() const noexcept { return _u.st == state::result || _u.st >= state::exception_min; }
    bool failed() const noexcept ;
    void set_to_broken_promise() noexcept;
    void ignore() noexcept ;
    void set_exception(std::exception_ptr&& ex) noexcept ;
    future_state_base& operator=(future_state_base&& x) noexcept ;
    void set_exception(future_state_base&& state) noexcept ;
    std::exception_ptr get_exception() && noexcept ;
    const std::exception_ptr& get_exception() const& noexcept ;
    static future_state_base current_exception();
    template <typename... U>
    friend future<U...> internal::current_exception_as_future() noexcept;
};
struct ready_future_marker {};
struct exception_future_marker {};
struct future_for_get_promise_marker {};
template <typename... T>
struct future_state :  public future_state_base, private internal::uninitialized_wrapper<std::tuple<T...>> {
    static constexpr bool copy_noexcept = std::is_nothrow_copy_constructible<std::tuple<T...>>::value;
    static constexpr bool has_trivial_move_and_destroy =
        internal::all_true<internal::is_trivially_move_constructible_and_destructible<T>::value...>::value;
    static_assert(std::is_nothrow_move_constructible<std::tuple<T...>>::value,
                  "Types must be no-throw move constructible");
    static_assert(std::is_nothrow_destructible<std::tuple<T...>>::value,
                  "Types must be no-throw destructible");
    future_state() noexcept {}
    [[gnu::always_inline]]
    future_state(future_state&& x) noexcept : future_state_base(std::move(x)) {
        if (has_trivial_move_and_destroy) {
            memcpy(reinterpret_cast<char*>(&this->uninitialized_get()),
                   &x.uninitialized_get(),
                   internal::used_size<std::tuple<T...>>::value);
        } else if (_u.has_result()) {
            this->uninitialized_set(std::move(x.uninitialized_get()));
            x.uninitialized_get().~tuple();
        }
    }
    __attribute__((always_inline))
    ~future_state() noexcept {
        if (_u.has_result()) {
            this->uninitialized_get().~tuple();
        }
    }
    future_state& operator=(future_state&& x) noexcept {
        this->~future_state();
        new (this) future_state(std::move(x));
        return *this;
    }
    template <typename... A>
    future_state(ready_future_marker, A&&... a) : future_state_base(state::result) {
        this->uninitialized_set(std::tuple<T...>(std::forward<A>(a)...));
    }
    template <typename... A>
    void set(A&&... a) {
        assert(_u.st == state::future);
        new (this) future_state(ready_future_marker(), std::forward<A>(a)...);
    }
    future_state(exception_future_marker m, std::exception_ptr&& ex) : future_state_base(std::move(ex)) { }
    future_state(exception_future_marker m, future_state_base&& state) : future_state_base(std::move(state)) { }
    std::tuple<T...>&& get_value() && noexcept {
        assert(_u.st == state::result);
        return std::move(this->uninitialized_get());
    }
    std::tuple<T...>&& take_value() && noexcept {
        assert(_u.st == state::result);
        _u.st = state::result_unavailable;
        return std::move(this->uninitialized_get());
    }
    template<typename U = std::tuple<T...>>
    const std::enable_if_t<std::is_copy_constructible<U>::value, U>& get_value() const& noexcept(copy_noexcept) {
        assert(_u.st == state::result);
        return this->uninitialized_get();
    }
    std::tuple<T...>&& take() && {
        assert(available());
        if (_u.st >= state::exception_min) {
            std::rethrow_exception(std::move(*this).get_exception());
        }
        _u.st = state::result_unavailable;
        return std::move(this->uninitialized_get());
    }
    std::tuple<T...>&& get() && {
        assert(available());
        if (_u.st >= state::exception_min) {
            std::rethrow_exception(std::move(*this).get_exception());
        }
        return std::move(this->uninitialized_get());
    }
    const std::tuple<T...>& get() const& {
        assert(available());
        if (_u.st >= state::exception_min) {
            std::rethrow_exception(_u.ex);
        }
        return this->uninitialized_get();
    }
    using get0_return_type = typename internal::get0_return_type<T...>::type;
    static get0_return_type get0(std::tuple<T...>&& x) {
        return internal::get0_return_type<T...>::get0(std::move(x));
    }
};
static_assert(sizeof(future_state<>) <= 8, "future_state<> is too large");
static_assert(sizeof(future_state<long>) <= 16, "future_state<long> is too large");
template <typename... T>
class continuation_base : public task {
protected:
    future_state<T...> _state;
    using future_type = future<T...>;
    using promise_type = promise<T...>;
public:
    continuation_base() = default;
    explicit continuation_base(future_state<T...>&& state) : _state(std::move(state)) {}
    void set_state(future_state<T...>&& state) {
        _state = std::move(state);
    }
    friend class internal::promise_base_with_type<T...>;
    friend class promise<T...>;
    friend class future<T...>;
};
template <typename Func, typename... T>
struct continuation final : continuation_base<T...> {
    continuation(Func&& func, future_state<T...>&& state) : continuation_base<T...>(std::move(state)), _func(std::move(func)) {}
    continuation(Func&& func) : _func(std::move(func)) {}
    virtual void run_and_dispose() noexcept override {
        _func(std::move(this->_state));
        delete this;
    }
    Func _func;
};
namespace internal {
template <typename... T>
future<T...> make_exception_future(future_state_base&& state) noexcept;
template <typename... T, typename U>
void set_callback(future<T...>& fut, U* callback) noexcept;
class future_base;
class promise_base {
protected:
    enum class urgent { no, yes };
    future_base* _future = nullptr;
    future_state_base* _state;
    task* _task = nullptr;
    promise_base(const promise_base&) = delete;
    promise_base(future_state_base* state) noexcept : _state(state) {}
    promise_base(future_base* future, future_state_base* state) noexcept;
    promise_base(promise_base&& x) noexcept;
    ~promise_base() noexcept;
    void operator=(const promise_base&) = delete;
    promise_base& operator=(promise_base&& x) = delete;
    template<urgent Urgent>
    void make_ready() noexcept;
    template<typename T>
    void set_exception_impl(T&& val) noexcept ;
    void set_exception(future_state_base&& state) noexcept ;
    void set_exception(std::exception_ptr&& ex) noexcept ;
    void set_exception(const std::exception_ptr& ex) noexcept ;
    template<typename Exception>
    std::enable_if_t<!std::is_same<std::remove_reference_t<Exception>, std::exception_ptr>::value, void> set_exception(Exception&& e) noexcept {
        set_exception(make_exception_ptr(std::forward<Exception>(e)));
    }
    friend class future_base;
    template <typename... U> friend class seastar::future;
};
template <typename... T>
class promise_base_with_type : protected internal::promise_base {
protected:
    future_state<T...>* get_state() ;
    static constexpr bool copy_noexcept = future_state<T...>::copy_noexcept;
public:
    promise_base_with_type(future_state_base* state)  ;
    promise_base_with_type(future<T...>* future)  ;
    promise_base_with_type(promise_base_with_type&& x) noexcept : promise_base(std::move(x)) { }
    promise_base_with_type(const promise_base_with_type&) = delete;
    promise_base_with_type& operator=(promise_base_with_type&& x) noexcept {
        this->~promise_base_with_type();
        new (this) promise_base_with_type(std::move(x));
        return *this;
    }
    void operator=(const promise_base_with_type&) = delete;
    void set_urgent_state(future_state<T...>&& state) noexcept {
        if (_state) {
            *get_state() = std::move(state);
            make_ready<urgent::yes>();
        }
    }
    template <typename... A>
    void set_value(A&&... a) {
        if (auto *s = get_state()) {
            s->set(std::forward<A>(a)...);
            make_ready<urgent::no>();
        }
    }
private:
    template <typename Func>
    void schedule(Func&& func) noexcept {
        auto tws = new continuation<Func, T...>(std::move(func));
        _state = &tws->_state;
        _task = tws;
    }
    void schedule(continuation_base<T...>* callback) noexcept {
        _state = &callback->_state;
        _task = callback;
    }
    template <typename... U>
    friend class seastar::future;
    friend struct seastar::future_state<T...>;
};
}
template <typename... T>
class promise : private internal::promise_base_with_type<T...> {
    future_state<T...> _local_state;
public:
    promise()  ;
    promise(promise&& x) noexcept;
    promise(const promise&) = delete;
    promise& operator=(promise&& x) noexcept ;
    void operator=(const promise&) = delete;
    future<T...> get_future() noexcept;
    template <typename... A>
    void set_value(A&&... a) ;
    void set_exception(std::exception_ptr&& ex) noexcept ;
    void set_exception(const std::exception_ptr& ex) noexcept ;
    template<typename Exception>
    std::enable_if_t<!std::is_same<std::remove_reference_t<Exception>, std::exception_ptr>::value, void> set_exception(Exception&& e) noexcept {
        internal::promise_base::set_exception(std::forward<Exception>(e));
    }
    using internal::promise_base_with_type<T...>::set_urgent_state;
    template <typename... U>
    friend class future;
};
template<>
class promise<void> : public promise<> {};
template <typename... T> struct is_future : std::false_type {};
template <typename... T> struct is_future<future<T...>> : std::true_type {};
template <typename T>
struct futurize;
template <typename T>
struct futurize {
    using type = future<T>;
    using promise_type = promise<T>;
    using value_type = std::tuple<T>;
    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept;
    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, FuncArgs&&... args) noexcept;
    static type convert(T&& value) ;
    static type convert(type&& value) ;
    static type from_tuple(value_type&& value);
    static type from_tuple(const value_type& value);
    template <typename Arg>
    static type make_exception_future(Arg&& arg);
};
template <>
struct futurize<void> {
    using type = future<>;
    using promise_type = promise<>;
    using value_type = std::tuple<>;
    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept;
    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, FuncArgs&&... args) noexcept;
    static inline type from_tuple(value_type&& value);
    static inline type from_tuple(const value_type& value);
    template <typename Arg>
    static type make_exception_future(Arg&& arg);
};
template <typename... Args>
struct futurize<future<Args...>> {
    using type = future<Args...>;
    using promise_type = promise<Args...>;
    using value_type = std::tuple<Args...>;
    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept;
    template<typename Func, typename... FuncArgs>
    static inline type apply(Func&& func, FuncArgs&&... args) noexcept;
    static inline type from_tuple(value_type&& value);
    static inline type from_tuple(const value_type& value);
    static inline type convert(Args&&... values) { return make_ready_future<Args...>(std::move(values)...); }
    static inline type convert(type&& value) { return std::move(value); }
    template <typename Arg>
    static type make_exception_future(Arg&& arg);
};
template <typename T>
using futurize_t = typename futurize<T>::type;
GCC6_CONCEPT(
template <typename T>
concept bool Future = is_future<T>::value;
template <typename Func, typename... T>
concept bool CanApply = requires (Func f, T... args) {
    f(std::forward<T>(args)...);
};
template <typename Func, typename Return, typename... T>
concept bool ApplyReturns = requires (Func f, T... args) {
    { f(std::forward<T>(args)...) } -> Return;
};
template <typename Func, typename... T>
concept bool ApplyReturnsAnyFuture = requires (Func f, T... args) {
    requires is_future<decltype(f(std::forward<T>(args)...))>::value;
};
)
namespace internal {
class future_base {
protected:
    promise_base* _promise;
    future_base() noexcept : _promise(nullptr) {}
    future_base(promise_base* promise, future_state_base* state)  ;
    future_base(future_base&& x, future_state_base* state)  ;
    ~future_base() noexcept ;
    promise_base* detach_promise() noexcept ;
    friend class promise_base;
};
template <bool IsVariadic>
struct warn_variadic_future {
    void check_deprecation() ;
};
template <>
struct warn_variadic_future<true> {
    [[deprecated("Variadic future<> with more than one template parmeter is deprecated, replace with future<std::tuple<...>>")]]
    void check_deprecation() {}
};
}
template <typename... T>
class SEASTAR_NODISCARD future : private internal::future_base, internal::warn_variadic_future<(sizeof...(T) > 1)> {
    future_state<T...> _state;
    static constexpr bool copy_noexcept = future_state<T...>::copy_noexcept;
private:
    future(future_for_get_promise_marker m) { }
    future(promise<T...>* pr) noexcept : future_base(pr, &_state), _state(std::move(pr->_local_state)) { }
    template <typename... A>
    future(ready_future_marker m, A&&... a) : _state(m, std::forward<A>(a)...) { }
    future(exception_future_marker m, std::exception_ptr&& ex) noexcept : _state(m, std::move(ex)) { }
    future(exception_future_marker m, future_state_base&& state) noexcept : _state(m, std::move(state)) { }
    [[gnu::always_inline]]
    explicit future(future_state<T...>&& state) noexcept
            : _state(std::move(state)) {
        this->check_deprecation();
    }
    internal::promise_base_with_type<T...> get_promise() noexcept {
        assert(!_promise);
        return internal::promise_base_with_type<T...>(this);
    }
    internal::promise_base_with_type<T...>* detach_promise() {
        return static_cast<internal::promise_base_with_type<T...>*>(future_base::detach_promise());
    }
    template <typename Func>
    void schedule(Func&& func) noexcept {
        if (_state.available() || !_promise) {
            if (__builtin_expect(!_state.available() && !_promise, false)) {
                _state.set_to_broken_promise();
            }
            ::seastar::schedule(new continuation<Func, T...>(std::move(func), std::move(_state)));
        } else {
            assert(_promise);
            detach_promise()->schedule(std::move(func));
            _state._u.st = future_state_base::state::invalid;
        }
    }
    [[gnu::always_inline]]
    future_state<T...>&& get_available_state_ref() noexcept {
        if (_promise) {
            detach_promise();
        }
        return std::move(_state);
    }
    [[gnu::noinline]]
    future<T...> rethrow_with_nested() {
        if (!failed()) {
            return internal::current_exception_as_future<T...>();
        } else {
            std::nested_exception f_ex;
            try {
                get();
            } catch (...) {
                std::throw_with_nested(f_ex);
            }
            __builtin_unreachable();
        }
    }
    template<typename... U>
    friend class shared_future;
public:
    using value_type = std::tuple<T...>;
    using promise_type = promise<T...>;
    [[gnu::always_inline]]
    future(future&& x) noexcept : future_base(std::move(x), &_state), _state(std::move(x._state)) { }
    future(const future&) = delete;
    future& operator=(future&& x) noexcept {
        this->~future();
        new (this) future(std::move(x));
        return *this;
    }
    void operator=(const future&) = delete;
    [[gnu::always_inline]]
    std::tuple<T...>&& get() {
        if (!_state.available()) {
            do_wait();
        }
        return get_available_state_ref().take();
    }
    [[gnu::always_inline]]
     std::exception_ptr get_exception() {
        return get_available_state_ref().get_exception();
    }
    typename future_state<T...>::get0_return_type get0() {
        return future_state<T...>::get0(get());
    }
    void wait() noexcept {
        if (!_state.available()) {
            do_wait();
        }
    }
private:
    class thread_wake_task final : public continuation_base<T...> {
        thread_context* _thread;
        future* _waiting_for;
    public:
        thread_wake_task(thread_context* thread, future* waiting_for)
                : _thread(thread), _waiting_for(waiting_for) {
        }
        virtual void run_and_dispose() noexcept override {
            _waiting_for->_state = std::move(this->_state);
            thread_impl::switch_in(_thread);
        }
    };
    void do_wait() noexcept {
        if (__builtin_expect(!_promise, false)) {
            _state.set_to_broken_promise();
            return;
        }
        auto thread = thread_impl::get();
        assert(thread);
        thread_wake_task wake_task{thread, this};
        detach_promise()->schedule(static_cast<continuation_base<T...>*>(&wake_task));
        thread_impl::switch_out(thread);
    }
public:
    [[gnu::always_inline]]
    bool available() const noexcept {
        return _state.available();
    }
    [[gnu::always_inline]]
    bool failed() const noexcept {
        return _state.failed();
    }
    template <typename Func, typename Result = futurize_t<std::result_of_t<Func(T&&...)>>>
    GCC6_CONCEPT( requires ::seastar::CanApply<Func, T...> )
    Result
    then(Func&& func) noexcept {
        return then_impl(std::move(func));
    }
private:
    template <typename Func, typename Result = futurize_t<std::result_of_t<Func(T&&...)>>>
    Result
    then_impl(Func&& func) noexcept {
        using futurator = futurize<std::result_of_t<Func(T&&...)>>;
        if (available() && !need_preempt()) {
            if (failed()) {
                return futurator::make_exception_future(static_cast<future_state_base&&>(get_available_state_ref()));
            } else {
                return futurator::apply(std::forward<Func>(func), get_available_state_ref().take_value());
            }
        }
        typename futurator::type fut(future_for_get_promise_marker{});
        [&] () noexcept {
            memory::disable_failure_guard dfg;
            schedule([pr = fut.get_promise(), func = std::forward<Func>(func)] (future_state<T...>&& state) mutable {
                if (state.failed()) {
                    pr.set_exception(static_cast<future_state_base&&>(std::move(state)));
                } else {
                    futurator::apply(std::forward<Func>(func), std::move(state).get_value()).forward_to(std::move(pr));
                }
            });
        } ();
        return fut;
    }
public:
    template <typename Func, typename FuncResult = std::result_of_t<Func(future)>>
    GCC6_CONCEPT( requires ::seastar::CanApply<Func, future> )
    futurize_t<FuncResult>
    then_wrapped(Func&& func) & noexcept {
        return then_wrapped_maybe_erase<false, FuncResult>(std::forward<Func>(func));
    }
    template <typename Func, typename FuncResult = std::result_of_t<Func(future&&)>>
    GCC6_CONCEPT( requires ::seastar::CanApply<Func, future&&> )
    futurize_t<FuncResult>
    then_wrapped(Func&& func) && noexcept {
        return then_wrapped_maybe_erase<true, FuncResult>(std::forward<Func>(func));
    }
private:
    template <bool AsSelf, typename FuncResult, typename Func>
    futurize_t<FuncResult>
    then_wrapped_maybe_erase(Func&& func) noexcept {
        return then_wrapped_common<AsSelf, FuncResult>(std::forward<Func>(func));
    }
    template <bool AsSelf, typename FuncResult, typename Func>
    futurize_t<FuncResult>
    then_wrapped_common(Func&& func) noexcept {
        using futurator = futurize<FuncResult>;
        if (available() && !need_preempt()) {
            if (AsSelf) {
                if (_promise) {
                    detach_promise();
                }
                return futurator::apply(std::forward<Func>(func), std::move(*this));
            } else {
                return futurator::apply(std::forward<Func>(func), future(get_available_state_ref()));
            }
        }
        typename futurator::type fut(future_for_get_promise_marker{});
        [&] () noexcept {
            memory::disable_failure_guard dfg;
            schedule([pr = fut.get_promise(), func = std::forward<Func>(func)] (future_state<T...>&& state) mutable {
                futurator::apply(std::forward<Func>(func), future(std::move(state))).forward_to(std::move(pr));
            });
        } ();
        return fut;
    }
    void forward_to(internal::promise_base_with_type<T...>&& pr) noexcept {
        if (_state.available()) {
            pr.set_urgent_state(std::move(_state));
        } else {
            *detach_promise() = std::move(pr);
        }
    }
public:
    void forward_to(promise<T...>&& pr) noexcept {
        if (_state.available()) {
            pr.set_urgent_state(std::move(_state));
        } else if (&pr._local_state != pr._state) {
            *detach_promise() = std::move(pr);
        }
    }
    template <typename Func>
    GCC6_CONCEPT( requires ::seastar::CanApply<Func> )
    future<T...> finally(Func&& func) noexcept {
        return then_wrapped(finally_body<Func, is_future<std::result_of_t<Func()>>::value>(std::forward<Func>(func)));
    }
    template <typename Func, bool FuncReturnsFuture>
    struct finally_body;
    template <typename Func>
    struct finally_body<Func, true> {
        Func _func;
        finally_body(Func&& func) : _func(std::forward<Func>(func))
        { }
        future<T...> operator()(future<T...>&& result) {
            using futurator = futurize<std::result_of_t<Func()>>;
            return futurator::apply(_func).then_wrapped([result = std::move(result)](auto f_res) mutable {
                if (!f_res.failed()) {
                    return std::move(result);
                } else {
                    try {
                        f_res.get();
                    } catch (...) {
                        return result.rethrow_with_nested();
                    }
                    __builtin_unreachable();
                }
            });
        }
    };
    template <typename Func>
    struct finally_body<Func, false> {
        Func _func;
        finally_body(Func&& func) : _func(std::forward<Func>(func))
        { }
        future<T...> operator()(future<T...>&& result) {
            try {
                _func();
                return std::move(result);
            } catch (...) {
                return result.rethrow_with_nested();
            }
        };
    };
    future<> or_terminate() noexcept {
        return then_wrapped([] (auto&& f) {
            try {
                f.get();
            } catch (...) {
                engine_exit(std::current_exception());
            }
        });
    }
    future<> discard_result() noexcept {
        return then([] (T&&...) {});
    }
    template <typename Func>
    future<T...> handle_exception(Func&& func) noexcept {
        using func_ret = std::result_of_t<Func(std::exception_ptr)>;
        return then_wrapped([func = std::forward<Func>(func)]
                             (auto&& fut) mutable -> future<T...> {
            if (!fut.failed()) {
                return make_ready_future<T...>(fut.get());
            } else {
                return futurize<func_ret>::apply(func, fut.get_exception());
            }
        });
    }
    template <typename Func>
    future<T...> handle_exception_type(Func&& func) noexcept {
        using trait = function_traits<Func>;
        static_assert(trait::arity == 1, "func can take only one parameter");
        using ex_type = typename trait::template arg<0>::type;
        using func_ret = typename trait::return_type;
        return then_wrapped([func = std::forward<Func>(func)]
                             (auto&& fut) mutable -> future<T...> {
            try {
                return make_ready_future<T...>(fut.get());
            } catch(ex_type& ex) {
                return futurize<func_ret>::apply(func, ex);
            }
        });
    }
    void ignore_ready_future() noexcept {
        _state.ignore();
    }
private:
    void set_callback(continuation_base<T...>* callback) noexcept {
        if (_state.available()) {
            callback->set_state(get_available_state_ref());
            ::seastar::schedule(callback);
        } else {
            assert(_promise);
            detach_promise()->schedule(callback);
        }
    }
    template <typename... U>
    friend class future;
    template <typename... U>
    friend class promise;
    template <typename... U>
    friend class internal::promise_base_with_type;
    template <typename... U, typename... A>
    friend future<U...> make_ready_future(A&&... value);
    template <typename... U>
    friend future<U...> make_exception_future(std::exception_ptr&& ex) noexcept;
    template <typename... U, typename Exception>
    friend future<U...> make_exception_future(Exception&& ex) noexcept;
    template <typename... U>
    friend future<U...> internal::make_exception_future(future_state_base&& state) noexcept;
    template <typename... U, typename V>
    friend void internal::set_callback(future<U...>&, V*) noexcept;
};



template <typename... T, typename... A>

future<T...> make_ready_future(A&&... value) ;
template <typename... T>

future<T...> make_exception_future(std::exception_ptr&& ex) noexcept ;
template <typename... T>

future<T...> internal::make_exception_future(future_state_base&& state) noexcept ;
template <typename... T>
future<T...> internal::current_exception_as_future() noexcept ;
void log_exception_trace() noexcept;
template <typename... T, typename Exception>

future<T...> make_exception_future(Exception&& ex) noexcept ;
template<typename T>
template<typename Func, typename... FuncArgs>
typename futurize<T>::type futurize<T>::apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept {
    try {
        return convert(::seastar::apply(std::forward<Func>(func), std::move(args)));
    } catch (...) {
        return internal::current_exception_as_future<T>();
    }
}
template<typename T>
template<typename Func, typename... FuncArgs>
typename futurize<T>::type futurize<T>::apply(Func&& func, FuncArgs&&... args) noexcept {
    try {
        return convert(func(std::forward<FuncArgs>(args)...));
    } catch (...) {
        return internal::current_exception_as_future<T>();
    }
}
template <typename Ret>  
struct do_void_futurize_helper;
template <>
struct do_void_futurize_helper<void> {
    template <typename Func, typename... FuncArgs>
    static future<> apply(Func&& func, FuncArgs&&... args) noexcept {
        try {
            func(std::forward<FuncArgs>(args)...);
            return make_ready_future<>();
        } catch (...) {
            return internal::current_exception_as_future<>();
        }
    }
    template<typename Func, typename... FuncArgs>
    static future<> apply_tuple(Func&& func, std::tuple<FuncArgs...>&& args) noexcept {
        try {
            ::seastar::apply(std::forward<Func>(func), std::move(args));
            return make_ready_future<>();
        } catch (...) {
            return internal::current_exception_as_future<>();
        }
    }
};
template <>
struct do_void_futurize_helper<future<>> {
    template <typename Func, typename... FuncArgs>
    static future<> apply(Func&& func, FuncArgs&&... args) noexcept {
        try {
            return func(std::forward<FuncArgs>(args)...);
        } catch (...) {
            return internal::current_exception_as_future<>();
        }
    }
    template<typename Func, typename... FuncArgs>
    static future<> apply_tuple(Func&& func, std::tuple<FuncArgs...>&& args) noexcept {
        try {
            return ::seastar::apply(std::forward<Func>(func), std::move(args));
        } catch (...) {
            return internal::current_exception_as_future<>();
        }
    }
};
template <typename Func, typename... FuncArgs>
using void_futurize_helper = do_void_futurize_helper<std::result_of_t<Func(FuncArgs&&...)>>;
template<typename Func, typename... FuncArgs>
typename futurize<void>::type futurize<void>::apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept {
    return void_futurize_helper<Func, FuncArgs...>::apply_tuple(std::forward<Func>(func), std::move(args));
}
template<typename Func, typename... FuncArgs>
typename futurize<void>::type futurize<void>::apply(Func&& func, FuncArgs&&... args) noexcept {
    return void_futurize_helper<Func, FuncArgs...>::apply(std::forward<Func>(func), std::forward<FuncArgs>(args)...);
}
template<typename... Args>
template<typename Func, typename... FuncArgs>
typename futurize<future<Args...>>::type futurize<future<Args...>>::apply(Func&& func, std::tuple<FuncArgs...>&& args) noexcept {
    try {
        return ::seastar::apply(std::forward<Func>(func), std::move(args));
    } catch (...) {
        return internal::current_exception_as_future<Args...>();
    }
}




template <typename T>
inline
future<T>
futurize<T>::from_tuple(std::tuple<T>&& value) {
    return make_ready_future<T>(std::move(value));
}

inline
future<>
futurize<void>::from_tuple(std::tuple<>&& value) {
    return make_ready_future<>();
}
inline
future<>
futurize<void>::from_tuple(const std::tuple<>& value) {
    return make_ready_future<>();
}
template <typename... Args>
inline
future<Args...>
futurize<future<Args...>>::from_tuple(std::tuple<Args...>&& value) {
    return make_ready_future<Args...>(std::move(value));
}
template <typename... Args>
inline
future<Args...>
futurize<future<Args...>>::from_tuple(const std::tuple<Args...>& value) {
    return make_ready_future<Args...>(value);
}
template<typename Func, typename... Args>
auto futurize_apply(Func&& func, Args&&... args) {
    using futurator = futurize<std::result_of_t<Func(Args&&...)>>;
    return futurator::apply(std::forward<Func>(func), std::forward<Args>(args)...);
}
namespace internal {
template <typename... T, typename U>
inline
void set_callback(future<T...>& fut, U* callback) noexcept {
    return fut.set_callback(callback);
}
}
}
#include <fcntl.h>
namespace seastar {
enum class open_flags {
    rw = O_RDWR,
    ro = O_RDONLY,
    wo = O_WRONLY,
    create = O_CREAT,
    truncate = O_TRUNC,
    exclusive = O_EXCL,
    dsync = O_DSYNC,
};
 open_flags operator|(open_flags a, open_flags b) ;
 void operator|=(open_flags& a, open_flags b) ;
 open_flags operator&(open_flags a, open_flags b) ;
 void operator&=(open_flags& a, open_flags b) ;
enum class directory_entry_type {
    unknown,
    block_device,
    char_device,
    directory,
    fifo,
    link,
    regular,
    socket,
};
enum class fs_type {
    other,
    xfs,
    ext2,
    ext3,
    ext4,
    btrfs,
    hfs,
    tmpfs,
};
enum class access_flags {
    exists = F_OK,
    read = R_OK,
    write = W_OK,
    execute = X_OK,
    lookup = execute,
};
 access_flags operator|(access_flags a, access_flags b) ;
 access_flags operator&(access_flags a, access_flags b) ;
enum class file_permissions {
    user_read = S_IRUSR,        
    user_write = S_IWUSR,       
    user_execute = S_IXUSR,     
    group_read = S_IRGRP,       
    group_write = S_IWGRP,      
    group_execute = S_IXGRP,    
    others_read = S_IROTH,      
    others_write = S_IWOTH,     
    others_execute = S_IXOTH,   
    user_permissions = user_read | user_write | user_execute,
    group_permissions = group_read | group_write | group_execute,
    others_permissions = others_read | others_write | others_execute,
    all_permissions = user_permissions | group_permissions | others_permissions,
    default_file_permissions = user_read | user_write | group_read | group_write | others_read | others_write, 
    default_dir_permissions = all_permissions, 
};
 constexpr file_permissions operator|(file_permissions a, file_permissions b) ;
 constexpr file_permissions operator&(file_permissions a, file_permissions b) ;
} 
namespace seastar {
template<typename Tag>
class bool_class {
    bool _value;
public:
    static const bool_class yes;
    static const bool_class no;
    constexpr bool_class()  ;
    constexpr explicit bool_class(bool v) noexcept : _value(v) { }
    
    
    
    
    
    
    
};
template<typename Tag>
const bool_class<Tag> bool_class<Tag>::yes { true };
template<typename Tag>
const bool_class<Tag> bool_class<Tag>::no { false };
}
#define SEASTAR_API_LEVEL 2
#define SEASTAR_INCLUDE_API_V2
#define SEASTAR_INCLUDE_API_V1 inline
namespace seastar {
template <class CharType> class input_stream;
template <class CharType> class output_stream;
SEASTAR_INCLUDE_API_V2 namespace api_v2 { class server_socket; }
SEASTAR_INCLUDE_API_V1 namespace api_v1 { class server_socket; }
class connected_socket;
class socket_address;
struct listen_options;
enum class transport;
class file;
struct file_open_options;
struct stat_data;














struct follow_symlink_tag { };
using follow_symlink = bool_class<follow_symlink_tag>;









}
namespace seastar {
namespace net {
enum class ip_protocol_num : uint8_t {
    icmp = 1, tcp = 6, udp = 17, unused = 255
};
enum class eth_protocol_num : uint16_t {
    ipv4 = 0x0800, arp = 0x0806, ipv6 = 0x86dd
};
const uint8_t eth_hdr_len = 14;
const uint8_t tcp_hdr_len_min = 20;
const uint8_t ipv4_hdr_len_min = 20;
const uint8_t ipv6_hdr_len_min = 40;
const uint16_t ip_packet_len_max = 65535;
}
}
namespace seastar {
namespace net {
struct fragment {
    char* base;
    size_t size;
};
struct offload_info {
    ip_protocol_num protocol = ip_protocol_num::unused;
    bool needs_csum = false;
    uint8_t ip_hdr_len = 20;
    uint8_t tcp_hdr_len = 20;
    uint8_t udp_hdr_len = 8;
    bool needs_ip_csum = false;
    bool reassembled = false;
    uint16_t tso_seg_size = 0;
    compat::optional<uint16_t> vlan_tci;
};
class packet final {
    static constexpr size_t internal_data_size = 128 - 16;
    static constexpr size_t default_nr_frags = 4;
    struct pseudo_vector {
        fragment* _start;
        fragment* _finish;
        
        
        
        
    };
    struct impl {
        deleter _deleter;
        unsigned _len = 0;
        uint16_t _nr_frags = 0;
        uint16_t _allocated_frags;
        offload_info _offload_info;
        compat::optional<uint32_t> _rss_hash;
        char _data[internal_data_size]; 
        unsigned _headroom = internal_data_size; 
        fragment _frags[];
        
        
        
        
        
        
        
        
        
        
        
        bool using_internal_data() const {
            return _nr_frags
                    && _frags[0].base >= _data
                    && _frags[0].base < _data + internal_data_size;
        }
        
        void copy_internal_fragment_to(impl* to) {
            if (!using_internal_data()) {
                return;
            }
            to->_frags[0].base = to->_data + _headroom;
            std::copy(_frags[0].base, _frags[0].base + _frags[0].size,
                    to->_frags[0].base);
        }
    };
    packet(std::unique_ptr<impl>&& impl) : _impl(std::move(impl)) {}
    std::unique_ptr<impl> _impl;
public:
    static packet from_static_data(const char* data, size_t len) {
        return {fragment{const_cast<char*>(data), len}, deleter()};
    }
    packet();
    packet(size_t nr_frags);
    packet(packet&& x) noexcept;
    packet(const char* data, size_t len);
    packet(fragment frag);
    packet(fragment frag, deleter del);
    packet(std::vector<fragment> frag, deleter del);
    template <typename Iterator>
    packet(Iterator begin, Iterator end, deleter del);
    packet(packet&& x, fragment frag);
    packet(fragment frag, packet&& x);
    packet(fragment frag, deleter del, packet&& x);
    packet(packet&& x, fragment frag, deleter d);
    packet(packet&& x, temporary_buffer<char> buf);
    packet(temporary_buffer<char> buf);
    packet(packet&& x, deleter d);
    packet& operator=(packet&& x) ;
    unsigned len() const ;
    unsigned memory() const ;
    fragment frag(unsigned idx) const ;
    fragment& frag(unsigned idx) ;
    unsigned nr_frags() const ;
    pseudo_vector fragments() const ;
    fragment* fragment_array() const ;
    packet share();
    packet share(size_t offset, size_t len);
    void append(packet&& p);
    void trim_front(size_t how_much);
    void trim_back(size_t how_much);
    template <typename Header>
    Header* get_header(size_t offset = 0);
    char* get_header(size_t offset, size_t size);
    template <typename Header>
    Header* prepend_header(size_t extra_size = 0);
    char* prepend_uninitialized_header(size_t size);
    packet free_on_cpu(unsigned cpu, std::function<void()> cb = []{});
    void linearize() ;
    void reset() ;
    void reserve(int n_frags) ;
    compat::optional<uint32_t> rss_hash() ;
    compat::optional<uint32_t> set_rss_hash(uint32_t hash) ;
    template <typename Func>
    void release_into(Func&& func) ;
    std::vector<temporary_buffer<char>> release() ;
    explicit operator bool() ;
    static packet make_null_packet() ;
private:
    void linearize(size_t at_frag, size_t desired_size);
    bool allocate_headroom(size_t size);
public:
    struct offload_info offload_info() const ;
    struct offload_info& offload_info_ref() ;
    void set_offload_info(struct offload_info oi) ;
};



























}
}
namespace seastar {
template <typename CharType>
class scattered_message {
private:
    using fragment = net::fragment;
    using packet = net::packet;
    using char_type = CharType;
    packet _p;
public:
    
    
    
    
     ;
    
     ;
    
     ;
    template <typename size_type, size_type max_size, typename Callback>
    void append(const basic_sstring<char_type, size_type, max_size>& s, Callback callback) ;
    void reserve(int n_frags) ;
    packet release() && ;
    template <typename Callback>
    void on_delete(Callback callback) ;
    operator bool() const ;
    size_t size() ;
};
}
namespace seastar {
namespace net { class packet; }
class data_source_impl {
public:
    virtual ~data_source_impl() ;
    virtual future<temporary_buffer<char>> get() = 0;
    virtual future<temporary_buffer<char>> skip(uint64_t n);
    
};
class data_source {
    std::unique_ptr<data_source_impl> _dsi;
protected:
    
public:
    
    
    
    
    
    
    
};
class data_sink_impl {
public:
    virtual ~data_sink_impl() {}
    virtual temporary_buffer<char> allocate_buffer(size_t size) {
        return temporary_buffer<char>(size);
    }
    virtual future<> put(net::packet data) = 0;
    virtual future<> put(std::vector<temporary_buffer<char>> data) {
        net::packet p;
        p.reserve(data.size());
        for (auto& buf : data) {
            p = net::packet(std::move(p), net::fragment{buf.get_write(), buf.size()}, buf.release());
        }
        return put(std::move(p));
    }
    virtual future<> put(temporary_buffer<char> buf) {
        return put(net::packet(net::fragment{buf.get_write(), buf.size()}, buf.release()));
    }
    virtual future<> flush() {
        return make_ready_future<>();
    }
    
};
class data_sink {
    std::unique_ptr<data_sink_impl> _dsi;
public:
    data_sink() = default;
    explicit data_sink(std::unique_ptr<data_sink_impl> dsi) : _dsi(std::move(dsi)) {}
    data_sink(data_sink&& x) = default;
    data_sink& operator=(data_sink&& x) = default;
    temporary_buffer<char> allocate_buffer(size_t size) {
        return _dsi->allocate_buffer(size);
    }
    future<> put(std::vector<temporary_buffer<char>> data) {
        return _dsi->put(std::move(data));
    }
    future<> put(temporary_buffer<char> data) {
        return _dsi->put(std::move(data));
    }
    future<> put(net::packet p) {
        return _dsi->put(std::move(p));
    }
    future<> flush() {
        return _dsi->flush();
    }
    
};
struct continue_consuming {};
template <typename CharType>
class stop_consuming {
public:
    using tmp_buf = temporary_buffer<CharType>;
    
    
    
private:
    tmp_buf _buf;
};
class skip_bytes {
public:
    
    
private:
    uint64_t _value;
};
template <typename CharType>
class consumption_result {
public:
    using stop_consuming_type = stop_consuming<CharType>;
    using consumption_variant = compat::variant<continue_consuming, stop_consuming_type, skip_bytes>;
    using tmp_buf = typename stop_consuming_type::tmp_buf;
     
    
    
    
    
    
private:
    consumption_variant _result;
};
GCC6_CONCEPT(
template <typename Consumer, typename CharType>
concept bool InputStreamConsumer = requires (Consumer c) {
    { c(temporary_buffer<CharType>{}) } -> future<consumption_result<CharType>>;
};
template <typename Consumer, typename CharType>
concept bool ObsoleteInputStreamConsumer = requires (Consumer c) {
    { c(temporary_buffer<CharType>{}) } -> future<compat::optional<temporary_buffer<CharType>>>;
};
)
template <typename CharType>
class input_stream final {
    static_assert(sizeof(CharType) == 1, "must buffer stream of bytes");
    data_source _fd;
    temporary_buffer<CharType> _buf;
    bool _eof = false;
private:
    using tmp_buf = temporary_buffer<CharType>;
    
protected:
    
    
public:
    using consumption_result_type = consumption_result<CharType>;
    using unconsumed_remainder = compat::optional<tmp_buf>;
    using char_type = CharType;
    
    
    
    
    
    ;
    ;
    
    
    
    
    
    
private:
    
};
template <typename CharType>
class output_stream final {
    static_assert(sizeof(CharType) == 1, "must buffer stream of bytes");
    data_sink _fd;
    temporary_buffer<CharType> _buf;
    net::packet _zc_bufs = net::packet::make_null_packet(); 
    size_t _size = 0;
    size_t _begin = 0;
    size_t _end = 0;
    bool _trim_to_size = false;
    bool _batch_flushes = false;
    compat::optional<promise<>> _in_batch;
    bool _flush = false;
    bool _flushing = false;
    std::exception_ptr _ex;
private:
    
    
    
    
    void poll_flush();
    
    
    [[gnu::noinline]]
    future<> slow_write(const CharType* buf, size_t n);
public:
    using char_type = CharType;
    
    
    
    
    
    
    
    ;
    
    
    
    future<> write(temporary_buffer<char_type>);
    future<> flush();
    future<> close();
    data_sink detach() &&;
private:
    friend class reactor;
};
template <typename CharType>
future<> copy(input_stream<CharType>&, output_stream<CharType>&);
}
namespace seastar {
namespace internal {
template <typename Future>
struct continuation_base_from_future;
template <typename... T>
struct continuation_base_from_future<future<T...>> {
    using type = continuation_base<T...>;
};
template <typename HeldState, typename Future>
class do_with_state final : public continuation_base_from_future<Future>::type {
    HeldState _held;
    typename Future::promise_type _pr;
public:
    explicit do_with_state(HeldState&& held) : _held(std::move(held)) {}
    virtual void run_and_dispose() noexcept override {
        _pr.set_urgent_state(std::move(this->_state));
        delete this;
    }
    HeldState& data() {
        return _held;
    }
    Future get_future() {
        return _pr.get_future();
    }
};
}
template<typename T, typename F>
inline
auto do_with(T&& rvalue, F&& f) {
    auto task = std::make_unique<internal::do_with_state<T, std::result_of_t<F(T&)>>>(std::forward<T>(rvalue));
    auto fut = f(task->data());
    if (fut.available()) {
        return fut;
    }
    auto ret = task->get_future();
    internal::set_callback(fut, task.release());
    return ret;
}
template <typename Tuple, size_t... Idx>
inline
auto
cherry_pick_tuple(std::index_sequence<Idx...>, Tuple&& tuple) {
    return std::make_tuple(std::get<Idx>(std::forward<Tuple>(tuple))...);
}
template<typename Lock, typename Func>
inline
auto with_lock(Lock& lock, Func&& func) {
    return lock.lock().then([func = std::forward<Func>(func)] () mutable {
        return func();
    }).then_wrapped([&lock] (auto&& fut) {
        lock.unlock();
        return std::move(fut);
    });
}
template <typename T1, typename T2, typename T3_or_F, typename... More>
inline
auto
do_with(T1&& rv1, T2&& rv2, T3_or_F&& rv3, More&&... more) {
    auto all = std::forward_as_tuple(
            std::forward<T1>(rv1),
            std::forward<T2>(rv2),
            std::forward<T3_or_F>(rv3),
            std::forward<More>(more)...);
    constexpr size_t nr = std::tuple_size<decltype(all)>::value - 1;
    using idx = std::make_index_sequence<nr>;
    auto&& just_values = cherry_pick_tuple(idx(), std::move(all));
    auto&& just_func = std::move(std::get<nr>(std::move(all)));
    using value_tuple = std::remove_reference_t<decltype(just_values)>;
    using ret_type = decltype(apply(just_func, just_values));
    auto task = std::make_unique<internal::do_with_state<value_tuple, ret_type>>(std::move(just_values));
    auto fut = apply(just_func, task->data());
    if (fut.available()) {
        return fut;
    }
    auto ret = task->get_future();
    internal::set_callback(fut, task.release());
    return ret;
}
}
#include <boost/intrusive/list.hpp>
#include <bitset>
namespace seastar {
namespace bitsets {
static constexpr int ulong_bits = std::numeric_limits<unsigned long>::digits;
template<typename T>
inline size_t count_leading_zeros(T value);
template<typename T>
static inline size_t count_trailing_zeros(T value);
template<>
inline size_t count_leading_zeros<unsigned long>(unsigned long value)
{
    return __builtin_clzl(value);
}
template<>
inline size_t count_leading_zeros<long>(long value)
{
    return __builtin_clzl((unsigned long)value) - 1;
}
template<>
inline size_t count_leading_zeros<long long>(long long value)
{
    return __builtin_clzll((unsigned long long)value) - 1;
}
template<>
inline
size_t count_trailing_zeros<unsigned long>(unsigned long value)
{
    return __builtin_ctzl(value);
}
template<>
size_t count_trailing_zeros<long>(long value)
;
template<size_t N>
static size_t get_first_set(const std::bitset<N>& bitset)
;

;
template<size_t N>
class set_iterator : public std::iterator<std::input_iterator_tag, int>
{
private:
    
public:
    
    
    
    
    
private:
    std::bitset<N> _bitset;
    int _index;
};
template<size_t N>
class set_range
{
public:
    using iterator = set_iterator<N>;
    using value_type = int;
    
    
    
private:
    std::bitset<N> _bitset;
    int _offset;
};
template<size_t N>
static inline set_range<N> for_each_set(std::bitset<N> bitset, int offset = 0)
{
    return set_range<N>(bitset, offset);
}
}
}
namespace seastar {
template<typename Timer, boost::intrusive::list_member_hook<> Timer::*link>
class timer_set {
public:
    using time_point = typename Timer::time_point;
    using timer_list_t = boost::intrusive::list<Timer, boost::intrusive::member_hook<Timer, boost::intrusive::list_member_hook<>, link>>;
private:
    using duration = typename Timer::duration;
    using timestamp_t = typename Timer::duration::rep;
    static constexpr timestamp_t max_timestamp = std::numeric_limits<timestamp_t>::max();
    static constexpr int timestamp_bits = std::numeric_limits<timestamp_t>::digits;
    static constexpr int n_buckets = timestamp_bits + 1;
    std::array<timer_list_t, n_buckets> _buckets;
    timestamp_t _last;
    timestamp_t _next;
    std::bitset<n_buckets> _non_empty_buckets;
private:
    static timestamp_t get_timestamp(time_point _time_point)
    ;
    
    
    
    
public:
    
    
    
    
    
    
    
    
    
    
};
};
namespace seastar {
using steady_clock_type = std::chrono::steady_clock;
template <typename Clock = steady_clock_type>
class timer {
public:
    typedef typename Clock::time_point time_point;
    typedef typename Clock::duration duration;
    typedef Clock clock;
private:
    using callback_t = noncopyable_function<void()>;
    boost::intrusive::list_member_hook<> _link;
    callback_t _callback;
    time_point _expiry;
    compat::optional<duration> _period;
    bool _armed = false;
    bool _queued = false;
    bool _expired = false;
    
    
public:
    
    
    
    
    
    
    
    void arm(duration delta) {
        return arm(Clock::now() + delta);
    }
    
    bool armed() const ;
    bool cancel();
    time_point get_timeout() ;
    friend class reactor;
    friend class timer_set<timer, &timer::_link>;
};
extern template class timer<steady_clock_type>;
}
#include <iterator>
#include <vector>
#include <tuple>
#include <utility>
namespace seastar {
namespace internal {
template<typename Tuple>
Tuple untuple(Tuple t) ;
template<typename T>
T untuple(std::tuple<T> t) {
    return std::get<0>(std::move(t));
}
template<typename Tuple, typename Function, size_t... I>
void tuple_for_each_helper(Tuple&& t, Function&& f, std::index_sequence<I...>&&) ;
template<typename Tuple, typename MapFunction, size_t... I>
auto tuple_map_helper(Tuple&& t, MapFunction&& f, std::index_sequence<I...>&&) ;
template<size_t I, typename IndexSequence>
struct prepend;
template<size_t I, size_t... Is>
struct prepend<I, std::index_sequence<Is...>> {
    using type = std::index_sequence<I, Is...>;
};
template<template<typename> class Filter, typename Tuple, typename IndexSequence>
struct tuple_filter;
template<template<typename> class Filter, typename T, typename... Ts, size_t I, size_t... Is>
struct tuple_filter<Filter, std::tuple<T, Ts...>, std::index_sequence<I, Is...>> {
    using tail = typename tuple_filter<Filter, std::tuple<Ts...>, std::index_sequence<Is...>>::type;
    using type = std::conditional_t<Filter<T>::value, typename prepend<I, tail>::type, tail>;
};
template<template<typename> class Filter>
struct tuple_filter<Filter, std::tuple<>, std::index_sequence<>> {
    using type = std::index_sequence<>;
};
template<typename Tuple, size_t... I>
auto tuple_filter_helper(Tuple&& t, std::index_sequence<I...>&&) ;
}
template<template<typename> class MapClass, typename Tuple>
struct tuple_map_types;
template<template<typename> class MapClass, typename... Elements>
struct tuple_map_types<MapClass, std::tuple<Elements...>> {
    using type = std::tuple<typename MapClass<Elements>::type...>;
};
template<template<typename> class FilterClass, typename... Elements>
auto tuple_filter_by_type(const std::tuple<Elements...>& t) ;
template<template<typename> class FilterClass, typename... Elements>
auto tuple_filter_by_type(std::tuple<Elements...>&& t) {
    using sequence = typename internal::tuple_filter<FilterClass, std::tuple<Elements...>,
                                                     std::index_sequence_for<Elements...>>::type;
    return internal::tuple_filter_helper(std::move(t), sequence());
}
template<typename Function, typename... Elements>
auto tuple_map(const std::tuple<Elements...>& t, Function&& f) ;
template<typename Function, typename... Elements>
auto tuple_map(std::tuple<Elements...>&& t, Function&& f) {
    return internal::tuple_map_helper(std::move(t), std::forward<Function>(f),
                                      std::index_sequence_for<Elements...>());
}
template<typename Function, typename... Elements>
void tuple_for_each(const std::tuple<Elements...>& t, Function&& f) ;
template<typename Function, typename... Elements>
void tuple_for_each(std::tuple<Elements...>& t, Function&& f) {
    return internal::tuple_for_each_helper(t, std::forward<Function>(f),
                                           std::index_sequence_for<Elements...>());
}
template<typename Function, typename... Elements>
void tuple_for_each(std::tuple<Elements...>&& t, Function&& f) {
    return internal::tuple_for_each_helper(std::move(t), std::forward<Function>(f),
                                           std::index_sequence_for<Elements...>());
}
}
namespace seastar {
extern __thread size_t task_quota;
namespace internal {
template <typename Func>
void
schedule_in_group(scheduling_group sg, Func func) ;
}
template <typename Func, typename... Args>

auto
with_scheduling_group(scheduling_group sg, Func func, Args&&... args) ;
namespace internal {
template <typename Iterator, typename IteratorCategory>

size_t
iterator_range_estimate_vector_capacity(Iterator begin, Iterator end, IteratorCategory category) ;
template <typename Iterator>

size_t
iterator_range_estimate_vector_capacity(Iterator begin, Iterator end, std::forward_iterator_tag category) ;
}
class parallel_for_each_state final : private continuation_base<> {
    std::vector<future<>> _incomplete;
    promise<> _result;
    std::exception_ptr _ex;
private:
    void wait_for_one() noexcept;
    virtual void run_and_dispose() noexcept override;
public:
    parallel_for_each_state(size_t n);
    void add_future(future<>&& f);
    future<> get_future();
};
template <typename Iterator, typename Func>
GCC6_CONCEPT( requires requires (Func f, Iterator i) { { f(*i++) } -> future<>; } )
inline
future<>
parallel_for_each(Iterator begin, Iterator end, Func&& func) noexcept {
    parallel_for_each_state* s = nullptr;
    while (begin != end) {
        auto f = futurize_apply(std::forward<Func>(func), *begin++);
        if (!f.available() || f.failed()) {
            if (!s) {
                using itraits = std::iterator_traits<Iterator>;
                auto n = (internal::iterator_range_estimate_vector_capacity(begin, end, typename itraits::iterator_category()) + 1);
                s = new parallel_for_each_state(n);
            }
            s->add_future(std::move(f));
        }
    }
    if (s) {
        return s->get_future();
    }
    return make_ready_future<>();
}
template <typename Range, typename Func>
GCC6_CONCEPT( requires requires (Func f, Range r) { { f(*r.begin()) } -> future<>; } )
inline
future<>
parallel_for_each(Range&& range, Func&& func) {
    return parallel_for_each(std::begin(range), std::end(range),
            std::forward<Func>(func));
}
struct stop_iteration_tag { };
using stop_iteration = bool_class<stop_iteration_tag>;
namespace internal {
template <typename AsyncAction>
class repeater final : public continuation_base<stop_iteration> {
    using futurator = futurize<std::result_of_t<AsyncAction()>>;
    promise<> _promise;
    AsyncAction _action;
public:
    explicit repeater(AsyncAction action) : _action(std::move(action)) {}
    repeater(stop_iteration si, AsyncAction action) : repeater(std::move(action)) {
        _state.set(std::make_tuple(si));
    }
    
    
};
}
template<typename AsyncAction>
GCC6_CONCEPT( requires seastar::ApplyReturns<AsyncAction, stop_iteration> || seastar::ApplyReturns<AsyncAction, future<stop_iteration>> )
inline
future<> repeat(AsyncAction action) noexcept {
    using futurator = futurize<std::result_of_t<AsyncAction()>>;
    static_assert(std::is_same<future<stop_iteration>, typename futurator::type>::value, "bad AsyncAction signature");
    try {
        do {
            auto f = futurator::apply(action);
            if (!f.available()) {
              return [&] () noexcept {
                memory::disable_failure_guard dfg;
                auto repeater = new internal::repeater<AsyncAction>(std::move(action));
                auto ret = repeater->get_future();
                internal::set_callback(f, repeater);
                return ret;
              }();
            }
            if (f.get0() == stop_iteration::yes) {
                return make_ready_future<>();
            }
        } while (!need_preempt());
        auto repeater = new internal::repeater<AsyncAction>(stop_iteration::no, std::move(action));
        auto ret = repeater->get_future();
        schedule(repeater);
        return ret;
    } catch (...) {
        return make_exception_future(std::current_exception());
    }
}
template <typename T>
struct repeat_until_value_type_helper;
template <typename T>
struct repeat_until_value_type_helper<future<compat::optional<T>>> {
    using value_type = T;
    using optional_type = compat::optional<T>;
    using future_type = future<value_type>;
};
template <typename AsyncAction>
using repeat_until_value_return_type
        = typename repeat_until_value_type_helper<typename futurize<std::result_of_t<AsyncAction()>>::type>::future_type;
namespace internal {
template <typename AsyncAction, typename T>
class repeat_until_value_state final : public continuation_base<compat::optional<T>> {
    using futurator = futurize<std::result_of_t<AsyncAction()>>;
    promise<T> _promise;
    AsyncAction _action;
public:
    explicit repeat_until_value_state(AsyncAction action) : _action(std::move(action)) {}
    repeat_until_value_state(compat::optional<T> st, AsyncAction action) : repeat_until_value_state(std::move(action)) {
        this->_state.set(std::make_tuple(std::move(st)));
    }
    future<T> get_future() { return _promise.get_future(); }
    virtual void run_and_dispose() noexcept override {
        if (this->_state.failed()) {
            _promise.set_exception(std::move(this->_state).get_exception());
            delete this;
            return;
        } else {
            auto v = std::get<0>(std::move(this->_state).get());
            if (v) {
                _promise.set_value(std::move(*v));
                delete this;
                return;
            }
            this->_state = {};
        }
        try {
            do {
                auto f = futurator::apply(_action);
                if (!f.available()) {
                    internal::set_callback(f, this);
                    return;
                }
                auto ret = f.get0();
                if (ret) {
                    _promise.set_value(std::make_tuple(std::move(*ret)));
                    delete this;
                    return;
                }
            } while (!need_preempt());
        } catch (...) {
            _promise.set_exception(std::current_exception());
            delete this;
            return;
        }
        this->_state.set(compat::nullopt);
        schedule(this);
    }
};
}

namespace internal {
template <typename StopCondition, typename AsyncAction>
class do_until_state final : public continuation_base<> {
    promise<> _promise;
    StopCondition _stop;
    AsyncAction _action;
public:
    
    
    
};
}


namespace internal {
template <typename Iterator, typename AsyncAction>
class do_for_each_state final : public continuation_base<> {
    Iterator _begin;
    Iterator _end;
    AsyncAction _action;
    promise<> _pr;
public:
    
    
    
};
}
 ;
 ;
namespace internal {
template<typename... Futures>
struct identity_futures_tuple {
    using future_type = future<std::tuple<Futures...>>;
    using promise_type = typename future_type::promise_type;
    
    
};
template <typename Future>
struct continuation_base_for_future;
template <typename... T>
struct continuation_base_for_future<future<T...>> {
    using type = continuation_base<T...>;
};
template <typename Future>
using continuation_base_for_future_t = typename continuation_base_for_future<Future>::type;
class when_all_state_base;
using when_all_process_element_func = bool (*)(void* future, void* continuation, when_all_state_base* wasb);
struct when_all_process_element {
    when_all_process_element_func func;
    void* future;
};
class when_all_state_base {
    size_t _nr_remain;
    const when_all_process_element* _processors;
    void* _continuation;
public:
    
    
    
    
    
};
template <typename Future>
class when_all_state_component final : public continuation_base_for_future_t<Future> {
    when_all_state_base* _base;
    Future* _final_resting_place;
public:
    
    
    
};
template<typename ResolvedTupleTransform, typename... Futures>
class when_all_state : public when_all_state_base {
    static constexpr size_t nr = sizeof...(Futures);
    using type = std::tuple<Futures...>;
    type tuple;
    std::aligned_union_t<1, when_all_state_component<Futures>...> _cont;
    when_all_process_element _processors[nr];
public:
    typename ResolvedTupleTransform::promise_type p;
    
    
private:
     ;
public:
    
};
}
GCC6_CONCEPT(
namespace impl {
template <typename T>
struct is_tuple_of_futures : std::false_type {
};
template <>
struct is_tuple_of_futures<std::tuple<>> : std::true_type {
};
template <typename... T, typename... Rest>
struct is_tuple_of_futures<std::tuple<future<T...>, Rest...>> : is_tuple_of_futures<std::tuple<Rest...>> {
};
}
template <typename... Futs>
concept bool AllAreFutures = impl::is_tuple_of_futures<std::tuple<Futs...>>::value;
)
 ;
 ;
template <typename... Futs>
GCC6_CONCEPT( requires seastar::AllAreFutures<Futs...> )
inline
future<std::tuple<Futs...>>
when_all_impl(Futs&&... futs) {
    namespace si = internal;
    using state = si::when_all_state<si::identity_futures_tuple<Futs...>, Futs...>;
    return state::wait_all(std::forward<Futs>(futs)...);
}
template <typename... FutOrFuncs>
inline auto when_all(FutOrFuncs&&... fut_or_funcs) {
    return when_all_impl(futurize_apply_if_func(std::forward<FutOrFuncs>(fut_or_funcs))...);
}
namespace internal {
template<typename Future>
struct identity_futures_vector {
    using future_type = future<std::vector<Future>>;
    static future_type run(std::vector<Future> futures) {
        return make_ready_future<std::vector<Future>>(std::move(futures));
    }
};
template <typename ResolvedVectorTransform, typename Future>
inline
typename ResolvedVectorTransform::future_type
complete_when_all(std::vector<Future>&& futures, typename std::vector<Future>::iterator pos) {
    while (pos != futures.end() && pos->available()) {
        ++pos;
    }
    if (pos == futures.end()) {
        return ResolvedVectorTransform::run(std::move(futures));
    }
    return pos->then_wrapped([futures = std::move(futures), pos] (auto fut) mutable {
        *pos++ = std::move(fut);
        return complete_when_all<ResolvedVectorTransform>(std::move(futures), pos);
    });
}
template<typename ResolvedVectorTransform, typename FutureIterator>
inline auto
do_when_all(FutureIterator begin, FutureIterator end) {
    using itraits = std::iterator_traits<FutureIterator>;
    std::vector<typename itraits::value_type> ret;
    ret.reserve(iterator_range_estimate_vector_capacity(begin, end, typename itraits::iterator_category()));
    std::move(begin, end, std::back_inserter(ret));
    return complete_when_all<ResolvedVectorTransform>(std::move(ret), ret.begin());
}
}
template <typename FutureIterator>
GCC6_CONCEPT( requires requires (FutureIterator i) { { *i++ }; requires is_future<std::remove_reference_t<decltype(*i)>>::value; } )
inline
future<std::vector<typename std::iterator_traits<FutureIterator>::value_type>>
when_all(FutureIterator begin, FutureIterator end) {
    namespace si = internal;
    using itraits = std::iterator_traits<FutureIterator>;
    using result_transform = si::identity_futures_vector<typename itraits::value_type>;
    return si::do_when_all<result_transform>(std::move(begin), std::move(end));
}
template <typename T, bool IsFuture>
struct reducer_with_get_traits;
template <typename T>
struct reducer_with_get_traits<T, false> {
    using result_type = decltype(std::declval<T>().get());
    using future_type = future<result_type>;
    
};
template <typename T>
struct reducer_with_get_traits<T, true> {
    using future_type = decltype(std::declval<T>().get());
    static future_type maybe_call_get(future<> f, lw_shared_ptr<T> r) {
        return f.then([r = std::move(r)] () mutable {
            return r->get();
        }).then_wrapped([r] (future_type f) {
            return f;
        });
    }
};
template <typename T, typename V = void>
struct reducer_traits {
    using future_type = future<>;
    
};
template <typename T>
struct reducer_traits<T, decltype(std::declval<T>().get(), void())> : public reducer_with_get_traits<T, is_future<std::result_of_t<decltype(&T::get)(T)>>::value> {};

;
 ;
 ;
template <typename Result, typename Addend = Result>
class adder {
private:
    Result _result;
public:
    
    
};



class timed_out_error : public std::exception {
public:
    
};
struct default_timeout_exception_factory {
    
};
template<typename ExceptionFactory = default_timeout_exception_factory, typename Clock, typename Duration, typename... T>
future<T...> with_timeout(std::chrono::time_point<Clock, Duration> timeout, future<T...> f) {
    if (f.available()) {
        return f;
    }
    auto pr = std::make_unique<promise<T...>>();
    auto result = pr->get_future();
    timer<Clock> timer([&pr = *pr] {
        pr.set_exception(std::make_exception_ptr(ExceptionFactory::timeout()));
    });
    timer.arm(timeout);
    (void)f.then_wrapped([pr = std::move(pr), timer = std::move(timer)] (auto&& f) mutable {
        if (timer.cancel()) {
            f.forward_to(std::move(*pr));
        } else {
            f.ignore_ready_future();
        }
    });
    return result;
}
namespace internal {
template<typename Future>
struct future_has_value {
    enum {
        value = !std::is_same<std::decay_t<Future>, future<>>::value
    };
};
template<typename Tuple>
struct tuple_to_future;
template<typename... Elements>
struct tuple_to_future<std::tuple<Elements...>> {
    using type = future<Elements...>;
    using promise_type = promise<Elements...>;
    static auto make_ready(std::tuple<Elements...> t) {
        auto create_future = [] (auto&&... args) {
            return make_ready_future<Elements...>(std::move(args)...);
        };
        return apply(create_future, std::move(t));
    }
    static auto make_failed(std::exception_ptr excp) {
        return seastar::make_exception_future<Elements...>(std::move(excp));
    }
};
template<typename... Futures>
class extract_values_from_futures_tuple {
    
public:
    using future_type = decltype(transform(std::declval<std::tuple<Futures...>>()));
    using promise_type = typename future_type::promise_type;
    
    
};
template<typename Future>
struct extract_values_from_futures_vector {
    using value_type = decltype(untuple(std::declval<typename Future::value_type>()));
    using future_type = future<std::vector<value_type>>;
    
};
template<>
struct extract_values_from_futures_vector<future<>> {
    using future_type = future<>;
    
};
}
 ;
 ;
template <typename FutureIterator, typename = typename std::iterator_traits<FutureIterator>::value_type>
GCC6_CONCEPT( requires requires (FutureIterator i) {
     *i++;
     { i != i } -> bool;
     requires is_future<std::remove_reference_t<decltype(*i)>>::value;
} )
inline auto
when_all_succeed(FutureIterator begin, FutureIterator end) {
    using itraits = std::iterator_traits<FutureIterator>;
    using result_transform = internal::extract_values_from_futures_vector<typename itraits::value_type>;
    return internal::do_when_all<result_transform>(std::move(begin), std::move(end));
}
}
#include <boost/version.hpp>
#if (BOOST_VERSION < 105800)
#error "Boost version >= 1.58 is required for using variant visitation helpers."
#error "Earlier versions lack support for return value deduction and move-only return values"
#endif
namespace seastar {
namespace internal {
#if __cplusplus >= 201703L 
template<typename... Args>
struct variant_visitor : Args... {
    variant_visitor(Args&&... a) : Args(std::move(a))... {}
    using Args::operator()...;
};
template<typename... Args> variant_visitor(Args&&...) -> variant_visitor<Args...>;
#else
template <typename... Args>
struct variant_visitor;
template <typename FuncObj, typename... Args>
struct variant_visitor<FuncObj, Args...> : FuncObj, variant_visitor<Args...>
{
    
    using FuncObj::operator();
    using variant_visitor<Args...>::operator();
};
template <typename FuncObj>
struct variant_visitor<FuncObj> : FuncObj
{
    
    using FuncObj::operator();
};
#endif
}

;

;
#ifdef SEASTAR_USE_STD_OPTIONAL_VARIANT_STRINGVIEW
namespace internal {
template<typename... Args>
struct castable_variant {
    compat::variant<Args...> var;
    template<typename... SuperArgs>
    operator compat::variant<SuperArgs...>() && {
        return std::visit([] (auto&& x) {
            return std::variant<SuperArgs...>(std::move(x));
        }, var);
    }
};
}
template<typename... Args>
internal::castable_variant<Args...> variant_cast(compat::variant<Args...>&& var) {
    return {std::move(var)};
}
template<typename... Args>
internal::castable_variant<Args...> variant_cast(const compat::variant<Args...>& var) {
    return {var};
}
#else
 ;
#endif
}
namespace seastar {























template <typename CharType>
void
output_stream<CharType>::poll_flush() {
    if (!_flush) {
        _flushing = false;
        _in_batch.value().set_value();
        _in_batch = compat::nullopt;
        return;
    }
    auto f = make_ready_future();
    _flush = false;
    _flushing = true; 
    if (_end) {
        _buf.trim(_end);
        _end = 0;
        f = _fd.put(std::move(_buf));
    } else if(_zc_bufs) {
        f = _fd.put(std::move(_zc_bufs));
    }
    (void)f.then([this] {
        return _fd.flush();
    }).then_wrapped([this] (future<> f) {
        try {
            f.get();
        } catch (...) {
            _ex = std::current_exception();
        }
        poll_flush();
    });
}


namespace internal {
template <typename CharType>
struct stream_copy_consumer {
private:
    output_stream<CharType>& _os;
    using unconsumed_remainder = compat::optional<temporary_buffer<CharType>>;
public:
    stream_copy_consumer(output_stream<CharType>& os)  ;
    future<unconsumed_remainder> operator()(temporary_buffer<CharType> data) ;
};
}
extern template struct internal::stream_copy_consumer<char>;
template <typename CharType>
future<> copy(input_stream<CharType>& in, output_stream<CharType>& out) ;
extern template future<> copy<char>(input_stream<char>&, output_stream<char>&);
}
#include <stdlib.h>
#include <memory>
#include <stdexcept>
namespace seastar {
namespace internal {
void* allocate_aligned_buffer_impl(size_t size, size_t align);
}
struct free_deleter {
    
};
 ;
}
#include <cstddef>
namespace seastar {
static constexpr size_t cache_line_size =
    64;
}
#include <type_traits>
namespace seastar {
template <typename T, size_t Capacity>
class circular_buffer_fixed_capacity {
    size_t _begin = 0;
    size_t _end = 0;
    union maybe_storage {
        T data;
        
        
    };
    maybe_storage _storage[Capacity];
private:
    
    
    
public:
    static_assert((Capacity & (Capacity - 1)) == 0, "capacity must be a power of two");
    static_assert(std::is_nothrow_move_constructible<T>::value && std::is_nothrow_move_assignable<T>::value,
            "circular_buffer_fixed_capacity only supports nothrow-move value types");
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
    using difference_type = ssize_t;
public:
    template <typename ValueType>
    class cbiterator {
        using holder = std::conditional_t<std::is_const<ValueType>::value, const maybe_storage, maybe_storage>;
        holder* _start;
        size_t _idx;
    private:
        
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = ValueType;
        using difference_type = ssize_t;
        using pointer = ValueType*;
        using reference = ValueType&;
    public:
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        friend class circular_buffer_fixed_capacity;
    };
public:
    using iterator = cbiterator<T>;
    using const_iterator = cbiterator<const T>;
public:
    
    
    
    
    
    
    ;
    
    
    ;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
};



















}
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unordered_map>
#include <cassert>
#include <unistd.h>
#include <queue>
#include <thread>
#include <system_error>
#include <chrono>
#include <ratio>
#include <stack>
#include <boost/next_prior.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/barrier.hpp>
#include <boost/container/static_vector.hpp>
#include <set>
namespace seastar {
struct reactor_config {
    std::chrono::duration<double> task_quota{0.5e-3}; 
    bool auto_handle_sigint_sigterm = true;  
};
}
#include <endian.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <signal.h>
namespace seastar {
namespace internal {
namespace linux_abi {
using aio_context_t = unsigned long;
enum class iocb_cmd : uint16_t {
    PREAD = 0,
    PWRITE = 1,
    FSYNC = 2,
    FDSYNC = 3,
    POLL = 5,
    NOOP = 6,
    PREADV = 7,
    PWRITEV = 8,
};
struct io_event {
    uint64_t data;
    uint64_t obj;
    int64_t res;
    int64_t res2;
};
constexpr int IOCB_FLAG_RESFD = 1;
struct iocb {
        uint64_t   aio_data;
#if __BYTE_ORDER == __LITTLE_ENDIAN
        uint32_t   aio_key;
        int32_t aio_rw_flags;
#elif __BYTE_ORDER == __BIG_ENDIAN
        int32_t aio_rw_flags;
        uint32_t   aio_key;
#else
#error bad byteorder
#endif
        iocb_cmd   aio_lio_opcode;
        int16_t   aio_reqprio;
        uint32_t   aio_fildes;
        uint64_t   aio_buf;
        uint64_t   aio_nbytes;
        int64_t   aio_offset;
        uint64_t   aio_reserved2;
        uint32_t   aio_flags;
        uint32_t   aio_resfd;
};
struct aio_sigset {
    const sigset_t *sigmask;
    size_t sigsetsize;
};
}

















}
namespace internal {






















}
}
namespace seastar {


}
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <sys/mman.h>
#include <pthread.h>
#include <iosfwd>
#include <array>
#include <sys/un.h>
#include <string>
namespace seastar {
/*!
    A helper struct for creating/manipulating UNIX-domain sockets.
    A UNIX-domain socket is either named or unnamed. If named, the name is either
    a path in the filesystem namespace, or an abstract-domain identifier. Abstract-domain
    names start with a null byte, and may contain non-printable characters.
    std::string() can hold a sequence of arbitrary bytes, and has a length() attribute
    that does not rely on using strlen(). Thus it is used here to hold the address.
 */
struct unix_domain_addr {
    const std::string name;
    const int path_count;  
    
    
    
    
    
    
    
};

} 
namespace seastar {
namespace net {
class inet_address;
}
struct ipv4_addr;
struct ipv6_addr;
class socket_address {
public:
    socklen_t addr_length; 
    union {
        ::sockaddr_storage sas;
        ::sockaddr sa;
        ::sockaddr_in in;
        ::sockaddr_in6 in6;
        ::sockaddr_un un;
    } u;
    
    
    
    
    
    
    
    
    /** creates an uninitialized socket_address. this can be written into, or used as
     *  "unspecified" for such addresses as bind(addr) or local address in socket::connect
     *  (i.e. system picks)
     */
    socket_address();
    
    
    
    
    
    
    
    ;
    
    
    
    
    
    
    
    
};

enum class transport {
    TCP = IPPROTO_TCP,
    SCTP = IPPROTO_SCTP
};
struct ipv4_addr {
    uint32_t ip;
    uint16_t port;
    
    
    
    
    
    
    
    
    
    
};
struct ipv6_addr {
    using ipv6_bytes = std::array<uint8_t, 16>;
    ipv6_bytes ip;
    uint16_t port;
    
    
    
    
    
    
    
    
    
    
};


 
}
namespace std {
template<>
struct hash<seastar::socket_address> {
    size_t operator()(const seastar::socket_address&) const;
};
template<>
struct hash<seastar::ipv4_addr> {
    size_t operator()(const seastar::ipv4_addr&) const;
};
template<>
struct hash<seastar::unix_domain_addr> {
    size_t operator()(const seastar::unix_domain_addr&) const;
};
template<>
struct hash<::sockaddr_un> {
    size_t operator()(const ::sockaddr_un&) const;
};
template <>
struct hash<seastar::transport> {
    size_t operator()(seastar::transport tr) const {
        return static_cast<size_t>(tr);
    }
};
}
namespace seastar {
 
;
struct mmap_deleter {
    size_t _size;
    
};
using mmap_area = std::unique_ptr<char[], mmap_deleter>;

class file_desc {
    int _fd;
public:
    
    
    
    
    
    
    
    int get() const ;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
     ;
     ;
     ;
    
     ;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
private:
    
 };
namespace posix {
template <typename Rep, typename Period>
struct timespec
to_timespec(std::chrono::duration<Rep, Period> d) {
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    struct timespec ts {};
    ts.tv_sec = ns / 1000000000;
    ts.tv_nsec = ns % 1000000000;
    return ts;
}
template <typename Rep1, typename Period1, typename Rep2, typename Period2>
struct itimerspec
to_relative_itimerspec(std::chrono::duration<Rep1, Period1> base, std::chrono::duration<Rep2, Period2> interval) {
    struct itimerspec its {};
    its.it_interval = to_timespec(interval);
    its.it_value = to_timespec(base);
    return its;
}
template <typename Clock, class Duration, class Rep, class Period>
struct itimerspec
to_absolute_itimerspec(std::chrono::time_point<Clock, Duration> base, std::chrono::duration<Rep, Period> interval) {
    return to_relative_itimerspec(base.time_since_epoch(), interval);
}
}
class posix_thread {
public:
    class attr;
private:
    std::unique_ptr<std::function<void ()>> _func;
    pthread_t _pthread;
    bool _valid = true;
    mmap_area _stack;
private:
    
public:
    
    
    
    
    
public:
    class attr {
    public:
        struct stack_size { size_t size = 0; };
        
         ;
        
         ;
        
    private:
        stack_size _stack_size;
        friend class posix_thread;
    };
};


 ;
 ;








}
namespace seastar {





namespace net {
struct tcp_keepalive_params {
    std::chrono::seconds idle; 
    std::chrono::seconds interval; 
    unsigned count; 
};
struct sctp_keepalive_params {
    std::chrono::seconds interval; 
    unsigned count; 
};
using keepalive_params = compat::variant<tcp_keepalive_params, sctp_keepalive_params>;
class connected_socket_impl;
class socket_impl;
SEASTAR_INCLUDE_API_V1 namespace api_v1 { class server_socket_impl; }
SEASTAR_INCLUDE_API_V2 namespace api_v2 { class server_socket_impl; }
class udp_channel_impl;
class get_impl;
class udp_datagram_impl {
public:
    virtual ~udp_datagram_impl() {};
    
    
    
    
};
class udp_datagram final {
private:
    std::unique_ptr<udp_datagram_impl> _impl;
public:
    ;
    
    
    
    
};
class udp_channel {
private:
    std::unique_ptr<udp_channel_impl> _impl;
public:
    
    
    
    
    
    
    
    
    
    
    
    
    
};
class network_interface_impl;
} 
class connected_socket {
    friend class net::get_impl;
    std::unique_ptr<net::connected_socket_impl> _csi;
public:
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
};
class socket {
    std::unique_ptr<net::socket_impl> _si;
public:
    
    
    
    
    
    
    
    
};
struct accept_result {
    connected_socket connection;  
    socket_address remote_address;  
};
SEASTAR_INCLUDE_API_V2 namespace api_v2 {
class server_socket {
    std::unique_ptr<net::api_v2::server_socket_impl> _ssi;
    bool _aborted = false;
public:
    enum class load_balancing_algorithm {
        connection_distribution,
        port,
        fixed,
        default_ = connection_distribution
    };
    
    
    
    
    
    
    
    
};
}
SEASTAR_INCLUDE_API_V1 namespace api_v1 {
class server_socket {
    api_v2::server_socket _impl;
private:
    
public:
    using load_balancing_algorithm = api_v2::server_socket::load_balancing_algorithm;
    
    
    
    
    
    
    
    
    
    
    
};
}
struct listen_options {
    bool reuse_address = false;
    server_socket::load_balancing_algorithm lba = server_socket::load_balancing_algorithm::default_;
    transport proto = transport::TCP;
    int listen_backlog = 100;
    unsigned fixed_cpu = 0u;
    
};
class network_interface {
private:
    shared_ptr<net::network_interface_impl> _impl;
public:
    
    
    
    
    
    
    
    
    
    
    
    
    
};
class network_stack {
public:
    
    
    
    
    
    
    
    
    
};
}
namespace seastar {
 ;


template <typename T, typename Alloc>
inline
void
transfer_pass2(Alloc& a, T* from, T* to,
        typename std::enable_if<!std::is_nothrow_move_constructible<T>::value>::type* = nullptr) {
    a.destroy(from);
}
}
#include <limits>
namespace seastar {
inline
constexpr unsigned count_leading_zeros(unsigned x) {
    return __builtin_clz(x);
}
inline
constexpr unsigned count_leading_zeros(unsigned long x) {
    return __builtin_clzl(x);
}
inline
constexpr unsigned count_leading_zeros(unsigned long long x) {
    return __builtin_clzll(x);
}
inline
constexpr unsigned count_trailing_zeros(unsigned x) {
    return __builtin_ctz(x);
}
inline
constexpr unsigned count_trailing_zeros(unsigned long x) {
    return __builtin_ctzl(x);
}
inline
constexpr unsigned count_trailing_zeros(unsigned long long x) {
    return __builtin_ctzll(x);
}
template<typename T>
inline constexpr unsigned log2ceil(T n) {
    if (n == 1) {
        return 0;
    }
    return std::numeric_limits<T>::digits - count_leading_zeros(n - 1);
}
 ;
}
namespace seastar {
template <typename T, typename Alloc = std::allocator<T>>
class circular_buffer {
    struct impl : Alloc {
        T* storage = nullptr;
        size_t begin = 0;
        size_t end = 0;
        size_t capacity = 0;
    };
    impl _impl;
public:
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
public:
    
    
    
    
    
    
    
    
    ;
    void push_back(const T& data);
    void push_back(T&& data);
    template <typename... A>
    void emplace_back(A&&... args);
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    void pop_front();
    void pop_back();
    bool empty() const;
    size_t size() const;
    size_t capacity() const;
    void reserve(size_t);
    void clear();
    T& operator[](size_t idx);
    const T& operator[](size_t idx) const;
    template <typename Func>
    void for_each(Func func);
    T& access_element_unsafe(size_t idx);
private:
    
    
    
    
    template<typename CB, typename ValueType>
    struct cbiterator : std::iterator<std::random_access_iterator_tag, ValueType> {
        typedef std::iterator<std::random_access_iterator_tag, ValueType> super_t;
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
       typename super_t::difference_type operator-(const cbiterator<CB, ValueType>& rhs) const {
            return idx - rhs.idx;
        }
    private:
        CB* cb;
        size_t idx;
        
        friend class circular_buffer;
    };
    friend class iterator;
public:
    typedef cbiterator<circular_buffer, T> iterator;
    typedef cbiterator<const circular_buffer, const T> const_iterator;
    
    
    
    
    
    
    
};





























}
#include <functional>
namespace seastar {
template <typename... T>
class stream;
template <typename... T>
class subscription;
template <typename... T>
class stream {
public:
    using next_fn = noncopyable_function<future<> (T...)>;
private:
    subscription<T...>* _sub = nullptr;
    promise<> _done;
    promise<> _ready;
    next_fn _next;
    
public:
    
    
    
    
    
    
    
    
    
    ;
    
    
    friend class subscription<T...>;
};
template <typename... T>
class subscription {
    stream<T...>* _stream;
    future<> _done;
    
public:
    using next_fn = typename stream<T...>::next_fn;
    
    
    
    
    friend class stream<T...>;
};

}
#include <cstdlib>
namespace seastar {
template <typename T>
inline constexpr
T align_up(T v, T align) {
    return (v + align - 1) & ~(align - 1);
}
template <typename T>
inline constexpr
T* align_up(T* v, size_t align) {
    static_assert(sizeof(T) == 1, "align byte pointers only");
    return reinterpret_cast<T*>(align_up(reinterpret_cast<uintptr_t>(v), align));
}


}
#include <unordered_set>
namespace seastar {
struct fair_queue_request_descriptor {
    unsigned weight = 1; 
    unsigned size = 1;        
};
class priority_class {
    struct request {
        noncopyable_function<void()> func;
        fair_queue_request_descriptor desc;
    };
    friend class fair_queue;
    uint32_t _shares = 0;
    float _accumulated = 0;
    circular_buffer<request> _queue;
    bool _queued = false;
    friend struct shared_ptr_no_esft<priority_class>;
    
    
public:
    
};
using priority_class_ptr = lw_shared_ptr<priority_class>;
class fair_queue {
public:
    struct config {
        unsigned capacity = std::numeric_limits<unsigned>::max();
        std::chrono::microseconds tau = std::chrono::milliseconds(100);
        unsigned max_req_count = std::numeric_limits<unsigned>::max();
        unsigned max_bytes_count = std::numeric_limits<unsigned>::max();
    };
private:
    friend priority_class;
    struct class_compare {
        
    };
    config _config;
    unsigned _requests_executing = 0;
    unsigned _req_count_executing = 0;
    unsigned _bytes_count_executing = 0;
    unsigned _requests_queued = 0;
    using clock_type = std::chrono::steady_clock::time_point;
    clock_type _base;
    using prioq = std::priority_queue<priority_class_ptr, std::vector<priority_class_ptr>, class_compare>;
    prioq _handles;
    std::unordered_set<priority_class_ptr> _all_classes;
    
    
    
    
    
public:
    
    
    
    
    
    
    
    
    
    
};
}
#include <sys/statvfs.h>
#include <linux/fs.h>
namespace seastar {
struct directory_entry {
    sstring name;
    compat::optional<directory_entry_type> type;
};
struct stat_data {
    uint64_t  device_id;      
    uint64_t  inode_number;   
    uint64_t  mode;           
    directory_entry_type type;
    uint64_t  number_of_links;
    uint64_t  uid;            
    uint64_t  gid;            
    uint64_t  rdev;           
    uint64_t  size;           
    uint64_t  block_size;     
    uint64_t  allocated_size; 
    std::chrono::system_clock::time_point time_accessed;  
    std::chrono::system_clock::time_point time_modified;  
    std::chrono::system_clock::time_point time_changed;   
};
struct file_open_options {
    uint64_t extent_allocation_size_hint = 1 << 20; 
    bool sloppy_size = false; 
    uint64_t sloppy_size_hint = 1 << 20; 
    file_permissions create_permissions = file_permissions::default_file_permissions; 
};
class io_queue;
using io_priority_class_id = unsigned;
class io_priority_class {
    io_priority_class_id _id;
    friend io_queue;
    
public:
    
};
const io_priority_class& default_priority_class();
class file;
class file_impl;
class file_handle;
class file_handle_impl {
public:
    
    
    
};
class file_impl {
protected:
    
public:
    unsigned _memory_dma_alignment = 4096;
    unsigned _disk_read_dma_alignment = 4096;
    unsigned _disk_write_dma_alignment = 4096;
public:
    virtual ~file_impl() {}
    
    
    
    
    
    
    
    virtual future<> discard(uint64_t offset, uint64_t length) = 0;
    virtual future<> allocate(uint64_t position, uint64_t length) = 0;
    virtual future<uint64_t> size(void) = 0;
    virtual future<> close() = 0;
    virtual std::unique_ptr<file_handle_impl> dup();
    virtual subscription<directory_entry> list_directory(std::function<future<> (directory_entry de)> next) = 0;
    virtual future<temporary_buffer<uint8_t>> dma_read_bulk(uint64_t offset, size_t range_size, const io_priority_class& pc) = 0;
    friend class reactor;
};
class file {
    shared_ptr<file_impl> _file_impl;
private:
    explicit file(int fd, file_open_options options);
public:
    
    
    
    
    
    
    
    
    
    
    uint64_t memory_dma_alignment() const ;
    template <typename CharType>
    future<size_t>
    dma_read(uint64_t aligned_pos, CharType* aligned_buffer, size_t aligned_len, const io_priority_class& pc = default_priority_class()) ;
    template <typename CharType>
    future<temporary_buffer<CharType>> dma_read(uint64_t pos, size_t len, const io_priority_class& pc = default_priority_class()) ;
    class eof_error : public std::exception {};
    template <typename CharType>
    future<temporary_buffer<CharType>>
    dma_read_exactly(uint64_t pos, size_t len, const io_priority_class& pc = default_priority_class()) ;
    future<size_t> dma_read(uint64_t pos, std::vector<iovec> iov, const io_priority_class& pc = default_priority_class()) ;
    template <typename CharType>
    future<size_t> dma_write(uint64_t pos, const CharType* buffer, size_t len, const io_priority_class& pc = default_priority_class()) ;
    future<size_t> dma_write(uint64_t pos, std::vector<iovec> iov, const io_priority_class& pc = default_priority_class()) ;
    future<> flush() ;
    future<struct stat> stat() ;
    future<> truncate(uint64_t length) ;
    future<> allocate(uint64_t position, uint64_t length) ;
    future<> discard(uint64_t offset, uint64_t length) ;
    future<uint64_t> size() const ;
    future<> close() {
        return _file_impl->close();
    }
    subscription<directory_entry> list_directory(std::function<future<> (directory_entry de)> next) {
        return _file_impl->list_directory(std::move(next));
    }
    template <typename CharType>
    future<temporary_buffer<CharType>>
    dma_read_bulk(uint64_t offset, size_t range_size, const io_priority_class& pc = default_priority_class()) {
        return _file_impl->dma_read_bulk(offset, range_size, pc).then([] (temporary_buffer<uint8_t> t) {
            return temporary_buffer<CharType>(reinterpret_cast<CharType*>(t.get_write()), t.size(), t.release());
        });
    }
    
    template <typename CharType>
    struct read_state;
private:
    friend class reactor;
    friend class file_impl;
};
class file_handle {
    std::unique_ptr<file_handle_impl> _impl;
private:
    
public:
    
    
    
    
    
    
    friend class file;
};
template <typename CharType>
struct file::read_state {
    typedef temporary_buffer<CharType> tmp_buf_type;
    
    
    
    
    
    
    
    
public:
    bool         eof      = false;
    tmp_buf_type buf;
    size_t       pos      = 0;
private:
    uint64_t     _offset;
    size_t       _to_read;
    uint64_t     _front;
};
}
namespace seastar {
template <typename T, size_t items_per_chunk = 128>
class chunked_fifo {
    static_assert((items_per_chunk & (items_per_chunk - 1)) == 0,
            "chunked_fifo chunk size must be power of two");
    union maybe_item {
        
        
        T data;
    };
    struct chunk {
        maybe_item items[items_per_chunk];
        struct chunk* next;
        unsigned begin;
        unsigned end;
    };
    chunk* _front_chunk = nullptr; 
    chunk* _back_chunk = nullptr; 
    size_t _nchunks = 0;
    chunk* _free_chunks = nullptr;
    size_t _nfree_chunks = 0;
public:
    using value_type = T;
    using size_type = size_t;
    using reference = T&;
    using pointer = T*;
    using const_reference = const T&;
    using const_pointer = const T*;
private:
    template <typename U>
    class basic_iterator {
        friend class chunked_fifo;
    public:
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = U;
        using pointer = U*;
        using reference = U&;
    protected:
        chunk* _chunk = nullptr;
        size_t _item_index = 0;
    protected:
        
        
    public:
        
        
        
        
        
        
    };
public:
    class iterator : public basic_iterator<T> {
        using basic_iterator<T>::basic_iterator;
    public:
        
    };
    class const_iterator : public basic_iterator<const T> {
        using basic_iterator<T>::basic_iterator;
    public:
        
        
    };
public:
    
    
    
    ;
    
    
    ;
    ;
    ;
    ;
    ;
    inline T& front() const noexcept;
    inline void pop_front() noexcept;
    inline bool empty() const noexcept;
    
    ;
    
    ;
    ;
    ;
    ;
    ;
    
    
private:
    void back_chunk_new();
    void front_chunk_delete() noexcept;
    ;
    ;
    
};












template <typename T, size_t items_per_chunk>
inline bool
chunked_fifo<T, items_per_chunk>::empty() const noexcept {
    return _front_chunk == nullptr;
}




template <typename T, size_t items_per_chunk>
void
chunked_fifo<T, items_per_chunk>::back_chunk_new() {
    chunk *old = _back_chunk;
    if (_free_chunks) {
        _back_chunk = _free_chunks;
        _free_chunks = _free_chunks->next;
        --_nfree_chunks;
    } else {
        _back_chunk = new chunk;
    }
    _back_chunk->next = nullptr;
    _back_chunk->begin = 0;
    _back_chunk->end = 0;
    if (old) {
        old->next = _back_chunk;
    }
    if (_front_chunk == nullptr) {
        _front_chunk = _back_chunk;
    }
    _nchunks++;
}







template <typename T, size_t items_per_chunk>
inline T&
chunked_fifo<T, items_per_chunk>::front() const noexcept {
    return _front_chunk->items[mask(_front_chunk->begin)].data;
}
template <typename T, size_t items_per_chunk>
inline void
chunked_fifo<T, items_per_chunk>::front_chunk_delete() noexcept {
    chunk *next = _front_chunk->next;
    static constexpr int save_free_chunks = 1;
    if (_nfree_chunks < save_free_chunks) {
        _front_chunk->next = _free_chunks;
        _free_chunks = _front_chunk;
        ++_nfree_chunks;
    } else {
        delete _front_chunk;
    }
    if (_back_chunk == _front_chunk) {
        _back_chunk = nullptr;
    }
    _front_chunk = next;
    --_nchunks;
}
template <typename T, size_t items_per_chunk>
inline void
chunked_fifo<T, items_per_chunk>::pop_front() noexcept {
    front().~T();
    if (++_front_chunk->begin == _front_chunk->end) {
        front_chunk_delete();
    }
}







}
namespace seastar {
class lowres_clock;
class lowres_system_clock;
class lowres_clock_impl final {
public:
    using base_steady_clock = std::chrono::steady_clock;
    using base_system_clock = std::chrono::system_clock;
    using period = std::ratio<1, 1000>;
    using steady_rep = base_steady_clock::rep;
    using steady_duration = std::chrono::duration<steady_rep, period>;
    using steady_time_point = std::chrono::time_point<lowres_clock, steady_duration>;
    using system_rep = base_system_clock::rep;
    using system_duration = std::chrono::duration<system_rep, period>;
    using system_time_point = std::chrono::time_point<lowres_system_clock, system_duration>;
    
    
    friend class smp;
private:
    struct alignas(seastar::cache_line_size) counters final {
        static std::atomic<steady_rep> _steady_now;
        static std::atomic<system_rep> _system_now;
    };
    static constexpr std::chrono::milliseconds _granularity{10};
    timer<> _timer{};
    
    
};
class lowres_clock final {
public:
    using rep = lowres_clock_impl::steady_rep;
    using period = lowres_clock_impl::period;
    using duration = lowres_clock_impl::steady_duration;
    using time_point = lowres_clock_impl::steady_time_point;
    static constexpr bool is_steady = true;
    
};
class lowres_system_clock final {
public:
    using rep = lowres_clock_impl::system_rep;
    using period = lowres_clock_impl::period;
    using duration = lowres_clock_impl::system_duration;
    using time_point = lowres_clock_impl::system_time_point;
    static constexpr bool is_steady = lowres_clock_impl::base_system_clock::is_steady;
    
    
    
};
extern template class timer<lowres_clock>;
}
namespace seastar {
template<typename T>
struct dummy_expiry {
    ;
};
template<typename... T>
struct promise_expiry {
    ;
};
template <typename T, typename OnExpiry = dummy_expiry<T>, typename Clock = lowres_clock>
class expiring_fifo {
public:
    using clock = Clock;
    using time_point = typename Clock::time_point;
private:
    struct entry {
        compat::optional<T> payload; 
        timer<Clock> tr;
        
        
        
        
        
    };
    std::unique_ptr<entry> _front;
    chunked_fifo<entry> _list;
    OnExpiry _on_expiry;
    size_t _size = 0;
    
public:
    
    
    
    bool empty() const {
        return _size == 0;
    }
    
    
    
    size_t size() const ;
    void reserve(size_t size) ;
    void push_back(const T& payload) ;
    void push_back(T&& payload) ;
    void push_back(T payload, time_point timeout) ;
    void pop_front() ;
};
}
namespace seastar {
class broken_semaphore : public std::exception {
public:
    virtual const char* what() const noexcept ;
};
class semaphore_timed_out : public std::exception {
public:
    virtual const char* what() const noexcept ;
};
struct semaphore_default_exception_factory {
    static semaphore_timed_out timeout() ;
    static broken_semaphore broken() ;
};
class named_semaphore_timed_out : public semaphore_timed_out {
    sstring _msg;
public:
    named_semaphore_timed_out(compat::string_view msg);
    virtual const char* what() const noexcept ;
};
class broken_named_semaphore : public broken_semaphore {
    sstring _msg;
public:
    broken_named_semaphore(compat::string_view msg);
    virtual const char* what() const noexcept ;
};
struct named_semaphore_exception_factory {
    sstring name;
    named_semaphore_timed_out timeout() ;
    broken_named_semaphore broken() {
        return broken_named_semaphore(name);
    }
};
template<typename ExceptionFactory, typename Clock = typename timer<>::clock>
class basic_semaphore : private ExceptionFactory {
public:
    using duration = typename timer<Clock>::duration;
    using clock = typename timer<Clock>::clock;
    using time_point = typename timer<Clock>::time_point;
    using exception_factory = ExceptionFactory;
private:
    ssize_t _count;
    std::exception_ptr _ex;
    struct entry {
        promise<> pr;
        size_t nr;
        entry(promise<>&& pr_, size_t nr_) : pr(std::move(pr_)), nr(nr_) {}
    };
    struct expiry_handler : public exception_factory {
        expiry_handler() = default;
        expiry_handler(exception_factory&& f) : exception_factory(std::move(f)) { }
        void operator()(entry& e) noexcept {
            e.pr.set_exception(exception_factory::timeout());
        }
    };
    expiring_fifo<entry, expiry_handler, clock> _wait_list;
    bool has_available_units(size_t nr) const {
        return _count >= 0 && (static_cast<size_t>(_count) >= nr);
    }
    bool may_proceed(size_t nr) const {
        return has_available_units(nr) && _wait_list.empty();
    }
public:
    static constexpr size_t max_counter() {
        return std::numeric_limits<decltype(_count)>::max();
    }
    basic_semaphore(size_t count) : _count(count) {}
    basic_semaphore(size_t count, exception_factory&& factory) : exception_factory(factory), _count(count), _wait_list(expiry_handler(std::move(factory))) {}
    future<> wait(size_t nr = 1) {
        return wait(time_point::max(), nr);
    }
    future<> wait(time_point timeout, size_t nr = 1) {
        if (may_proceed(nr)) {
            _count -= nr;
            return make_ready_future<>();
        }
        if (_ex) {
            return make_exception_future(_ex);
        }
        promise<> pr;
        auto fut = pr.get_future();
        _wait_list.push_back(entry(std::move(pr), nr), timeout);
        return fut;
    }
    future<> wait(duration timeout, size_t nr = 1) {
        return wait(clock::now() + timeout, nr);
    }
    void signal(size_t nr = 1) ;
    void consume(size_t nr = 1) ;
    bool try_wait(size_t nr = 1) ;
    size_t current() const ;
    ssize_t available_units() const ;
    size_t waiters() const ;
    
    
    
    
};

template<typename ExceptionFactory = semaphore_default_exception_factory, typename Clock = typename timer<>::clock>
class semaphore_units {
    basic_semaphore<ExceptionFactory, Clock>* _sem;
    size_t _n;
    
public:
    
    
    
    
    
    
    
    
    
};
 ;
 ;
 ;
 ;
template <typename ExceptionFactory, typename Func, typename Clock = typename timer<>::clock>
inline
futurize_t<std::result_of_t<Func()>>
with_semaphore(basic_semaphore<ExceptionFactory, Clock>& sem, size_t units, Func&& func) {
    return get_units(sem, units).then([func = std::forward<Func>(func)] (auto units) mutable {
        return futurize_apply(std::forward<Func>(func)).finally([units = std::move(units)] {});
    });
}
template <typename ExceptionFactory, typename Clock, typename Func>
inline
futurize_t<std::result_of_t<Func()>>
with_semaphore(basic_semaphore<ExceptionFactory, Clock>& sem, size_t units, typename basic_semaphore<ExceptionFactory, Clock>::duration timeout, Func&& func) {
    return get_units(sem, units, timeout).then([func = std::forward<Func>(func)] (auto units) mutable {
        return futurize_apply(std::forward<Func>(func)).finally([units = std::move(units)] {});
    });
}
using semaphore = basic_semaphore<semaphore_default_exception_factory>;
using named_semaphore = basic_semaphore<named_semaphore_exception_factory>;
}
namespace seastar {
template <typename T>
class enum_hash {
    static_assert(std::is_enum<T>::value, "must be an enum");
public:
    
};
}
#include <sched.h>
#include <boost/any.hpp>
namespace seastar {

namespace resource {
using compat::optional;
using cpuset = std::set<unsigned>;
struct configuration {
    optional<size_t> total_memory;
    optional<size_t> reserve_memory;  
    optional<size_t> cpus;
    optional<cpuset> cpu_set;
    std::unordered_map<dev_t, unsigned> num_io_queues;
};
struct memory {
    size_t bytes;
    unsigned nodeid;
};
struct io_queue_topology {
    std::vector<unsigned> shard_to_coordinator;
    std::vector<unsigned> coordinators;
    std::vector<unsigned> coordinator_to_idx;
    std::vector<bool> coordinator_to_idx_valid; 
};
struct cpu {
    unsigned cpu_id;
    std::vector<memory> mem;
};
struct resources {
    std::vector<cpu> cpus;
    std::unordered_map<dev_t, io_queue_topology> ioq_topology;
};


}
struct cpuset_bpo_wrapper {
    resource::cpuset value;
};

}
#include <new>
namespace seastar {
namespace memory {
#define SEASTAR_INTERNAL_ALLOCATOR_PAGE_SIZE 4096
static constexpr size_t page_size = SEASTAR_INTERNAL_ALLOCATOR_PAGE_SIZE;
static constexpr size_t page_bits = log2ceil(page_size);
static constexpr size_t huge_page_size =
    1 << 21; 
void configure(std::vector<resource::memory> m, bool mbind,
        compat::optional<std::string> hugetlbfs_path = {});

class disable_abort_on_alloc_failure_temporarily {
public:
    
    
};
enum class reclaiming_result {
    reclaimed_nothing,
    reclaimed_something
};
enum class reclaimer_scope {
    async,
    sync
};
class reclaimer {
public:
    struct request {
        size_t bytes_to_reclaim;
    };
    using reclaim_fn = std::function<reclaiming_result ()>;
private:
    std::function<reclaiming_result (request)> _reclaim;
    reclaimer_scope _scope;
public:
    
    reclaimer(std::function<reclaiming_result (request)> reclaim, reclaimer_scope scope = reclaimer_scope::async);
    ~reclaimer();
    reclaiming_result do_reclaim(size_t bytes_to_reclaim) ;
    reclaimer_scope scope() const ;
};
extern compat::polymorphic_allocator<char>* malloc_allocator;
bool drain_cross_cpu_freelist();
void set_reclaim_hook(
        std::function<void (std::function<void ()>)> hook);
class statistics;
statistics stats();
class statistics {
    uint64_t _mallocs;
    uint64_t _frees;
    uint64_t _cross_cpu_frees;
    size_t _total_memory;
    size_t _free_memory;
    uint64_t _reclaims;
    uint64_t _large_allocs;
private:
    statistics(uint64_t mallocs, uint64_t frees, uint64_t cross_cpu_frees,
            uint64_t total_memory, uint64_t free_memory, uint64_t reclaims, uint64_t large_allocs)
        : _mallocs(mallocs), _frees(frees), _cross_cpu_frees(cross_cpu_frees)
        , _total_memory(total_memory), _free_memory(free_memory), _reclaims(reclaims), _large_allocs(large_allocs) {}
public:
    uint64_t mallocs() const { return _mallocs; }
    uint64_t frees() const { return _frees; }
    
    
    
    
    
    
    
    friend statistics stats();
};
struct memory_layout {
    uintptr_t start;
    uintptr_t end;
};






class scoped_large_allocation_warning_threshold {
    size_t _old_threshold;
public:
    
    
    
    
    
    
};
class scoped_large_allocation_warning_disable {
    size_t _old_threshold;
public:
    
    
    
    
    
    
};

class scoped_heap_profiling {
public:
    
    
};
}
}
#include <time.h>
namespace seastar {
class thread_cputime_clock {
public:
    using rep = int64_t;
    using period = std::chrono::nanoseconds::period;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<thread_cputime_clock, duration>;
public:
    
};
}
#include <boost/range/irange.hpp>
namespace seastar {
class broken_condition_variable : public std::exception {
public:
    
};
class condition_variable_timed_out : public std::exception {
public:
    
};
class condition_variable {
    using duration = semaphore::duration;
    using clock = semaphore::clock;
    using time_point = semaphore::time_point;
    struct condition_variable_exception_factory {
        
        
    };
    basic_semaphore<condition_variable_exception_factory> _sem;
public:
    
    
    
    
    
    
    
    
    
    
};
}
#include <mutex>
#include <boost/lexical_cast.hpp>
namespace seastar {
enum class log_level {
    error,
    warn,
    info,
    debug,
    trace,
};


}
namespace boost {

}
namespace seastar {
class logger;
class logger_registry;
class logger {
    sstring _name;
    std::atomic<log_level> _level = { log_level::info };
    static std::ostream* _out;
    static std::atomic<bool> _ostream;
    static std::atomic<bool> _syslog;
private:
    struct stringer {
        void (*append)(std::ostream& os, const void* object);
        const void* object;
    };
     ;;
    ;
    
    
public:
    
    
    
    
    
     ;
     ;
     ;
     ;
     ;
     ;
     ;
    
    
    
    
    
    [[deprecated("Use set_ostream_enabled instead")]]
    static void set_stdout_enabled(bool enabled);
    
};
class logger_registry {
    mutable std::mutex _mutex;
    std::unordered_map<sstring, logger*> _loggers;
public:
    
    
    
    
    
    
    
};

enum class logger_timestamp_style {
    none,
    boot,
    real,
};
enum class logger_ostream_type {
    none,
    stdout,
    stderr,
};
struct logging_settings final {
    std::unordered_map<sstring, log_level> logger_levels;
    log_level default_level;
    bool stdout_enabled;
    bool syslog_enabled;
    logger_timestamp_style stdout_timestamp_style = logger_timestamp_style::real;
    logger_ostream_type logger_ostream = logger_ostream_type::stderr;
};

extern thread_local uint64_t logging_failures;


template <typename T>
class logger_for : public logger {
public:
    
};

} 
namespace std {



}
namespace seastar {
class manual_clock {
public:
    using rep = int64_t;
    using period = std::chrono::nanoseconds::period;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<manual_clock, duration>;
private:
    static std::atomic<rep> _now;
    
public:
    
    
    
};
extern template class timer<manual_clock>;
}
/*!
 * \file metrics_registration.hh
 * \brief holds the metric_groups definition needed by class that reports metrics
 *
 * If class A needs to report metrics,
 * typically you include metrics_registration.hh, in A header file and add to A:
 * * metric_groups _metrics as a member
 * * set_metrics() method that would be called in the constructor.
 * \code
 * class A {
 *   metric_groups _metrics
 *
 *   void setup_metrics();
 *
 * };
 * \endcode
 * To define the metrics, include in your source file metircs.hh
 * @see metrics.hh for the definition for adding a metric.
 */
namespace seastar {
namespace metrics {
namespace impl {
class metric_groups_def;
struct metric_definition_impl;
class metric_groups_impl;
}
using group_name_type = sstring; /*!< A group of logically related metrics */
class metric_groups;
class metric_definition {
    std::unique_ptr<impl::metric_definition_impl> _impl;
public:
    
    
    
    friend metric_groups;
    friend impl::metric_groups_impl;
};
class metric_group_definition {
public:
    group_name_type name;
    std::initializer_list<metric_definition> metrics;
    
    
    
};
/*!
 * metric_groups
 * \brief holds the metric definition.
 *
 * Add multiple metric groups definitions.
 * Initialization can be done in the constructor or with a call to add_group
 * @see metrics.hh for example and supported metrics
 */
class metric_groups {
    std::unique_ptr<impl::metric_groups_def> _impl;
public:
    
    
    
    
    /*!
     * \brief add metrics belong to the same group in the constructor.
     *
     * combine the constructor with the add_group functionality.
     */
    
    /*!
     * \brief Add metrics belonging to the same group.
     *
     * Use the metrics creation functions to add metrics.
     *
     * For example:
     *  _metrics.add_group("my_group", {
     *      make_counter("my_counter_name1", counter, description("my counter description")),
     *      make_counter("my_counter_name2", counter, description("my second counter description")),
     *      make_gauge("my_gauge_name1", gauge, description("my gauge description")),
     *  });
     *
     * Metric name should be unique inside the group.
     * You can chain add_group calls like:
     * _metrics.add_group("my group1", {...}).add_group("my group2", {...});
     *
     * This overload (with initializer_list) is needed because metric_definition
     * has no copy constructor, so the other overload (with vector) cannot be
     * invoked on a braced-init-list.
     */
    
    /*!
     * \brief Add metrics belonging to the same group.
     *
     * Use the metrics creation functions to add metrics.
     *
     * For example:
     *  vector<metric_definition> v;
     *  v.push_back(make_counter("my_counter_name1", counter, description("my counter description")));
     *  v.push_back(make_counter("my_counter_name2", counter, description("my second counter description")));
     *  v.push_back(make_gauge("my_gauge_name1", gauge, description("my gauge description")));
     *  _metrics.add_group("my_group", v);
     *
     * Metric name should be unique inside the group.
     * You can chain add_group calls like:
     * _metrics.add_group("my group1", vec1).add_group("my group2", vec2);
     */
    
    /*!
     * \brief clear all metrics groups registrations.
     */
    
};
/*!
 * \brief hold a single metric group
 * Initialization is done in the constructor or
 * with a call to add_group
 */
class metric_group : public metric_groups {
public:
    
    
    
    
    
    /*!
     * \brief add metrics belong to the same group in the constructor.
     *
     *
     */
    
};
}
}
#include <map>
namespace seastar {
namespace metrics {
/*!
 * \brief Histogram bucket type
 *
 * A histogram bucket contains an upper bound and the number
 * of events in the buckets.
 */
struct histogram_bucket {
    uint64_t count = 0; 
    double upper_bound = 0;      
};
/*!
 * \brief Histogram data type
 *
 * The histogram struct is a container for histogram values.
 * It is not a histogram implementation but it will be used by histogram
 * implementation to return its internal values.
 */
struct histogram {
    uint64_t sample_count = 0;
    double sample_sum = 0;
    std::vector<histogram_bucket> buckets; 
    /*!
     * \brief Addition assigning a historgram
     *
     * The histogram must match the buckets upper bounds
     * or an exception will be thrown
     */
    
    /*!
     * \brief Addition historgrams
     *
     * Add two histograms and return the result as a new histogram
     * The histogram must match the buckets upper bounds
     * or an exception will be thrown
     */
    
    /*!
     * \brief Addition historgrams
     *
     * Add two histograms and return the result as a new histogram
     * The histogram must match the buckets upper bounds
     * or an exception will be thrown
     */
    
};
}
}
/*! \file metrics.hh
 *  \brief header for metrics creation.
 *
 *  This header file contains the metrics creation method with their helper function.
 *  Include this file when need to create metrics.
 *  Typically this will be in your source file.
 *
 *  Code that is under the impl namespace should not be used directly.
 *
 */
namespace seastar {
/*!
 * \namespace seastar::metrics
 * \brief metrics creation and registration
 *
 * the metrics namespace holds the relevant method and classes to generate metrics.
 *
 * The metrics layer support registering metrics, that later will be
 * exported via different API protocols.
 *
 * To be able to support multiple protocols the following simplifications where made:
 * 1. The id of the metrics is based on the collectd id
 * 2. A metric could be a single value either a reference or a function
 *
 * To add metrics definition to class A do the following:
 * * Add a metrics_group memeber to A
 * * Add a a set_metrics() method that would be called in the constructor.
 *
 *
 * In A header file
 * \code
 * #include "core/metrics_registration.hh"
 * class A {
 *   metric_groups _metrics
 *
 *   void setup_metrics();
 *
 * };
 * \endcode
 *
 * In A source file:
 *
 * \code
 * include "core/metrics.hh"
 *
 * void A::setup_metrics() {
 *   namespace sm = seastar::metrics;
 *   _metrics = sm::create_metric_group();
 *   _metrics->add_group("cache", {sm::make_gauge("bytes", "used", [this] { return _region.occupancy().used_space(); })});
 * }
 * \endcode
 */
namespace metrics {
class double_registration : public std::runtime_error {
public:
    
};
/*!
 * \defgroup metrics_types metrics type definitions
 * The following are for the metric layer use, do not use them directly
 * Instead use the make_counter, make_gauge, make_absolute and make_derived
 *
 */
using metric_type_def = sstring; /*!< Used to hold an inherit type (like bytes)*/
using metric_name_type = sstring; /*!<  The metric name'*/
using instance_id_type = sstring; 
class description {
public:
    
    
private:
    sstring _s;
};
class label_instance {
    sstring _key;
    sstring _value;
public:
    
    
    
    
    
    
};
class label {
    sstring key;
public:
    using instance = label_instance;
    
    
    
};
namespace impl {
enum class data_type : uint8_t {
    COUNTER, 
    GAUGE, 
    DERIVE, 
    ABSOLUTE, 
    HISTOGRAM,
};
struct metric_value {
    compat::variant<double, histogram> u;
    data_type _type;
    
    
    
    
    
    
    
    
    
    
    
    
};
using metric_function = std::function<metric_value()>;
struct metric_type {
    data_type base_type;
    metric_type_def type_name;
};
struct metric_definition_impl {
    metric_name_type name;
    metric_type type;
    metric_function f;
    description d;
    bool enabled = true;
    std::map<sstring, sstring> labels;
    
    
    
};
class metric_groups_def {
public:
    
    
    
    
    
    virtual metric_groups_def& add_group(group_name_type name, const std::initializer_list<metric_definition>& l) = 0;
    virtual metric_groups_def& add_group(group_name_type name, const std::vector<metric_definition>& l) = 0;
};
instance_id_type shard();
template<typename T, typename En = std::true_type>
struct is_callable;
template<typename T>
struct is_callable<T, typename std::integral_constant<bool, !std::is_void<typename std::result_of<T()>::type>::value>::type> : public std::true_type {
};
template<typename T>
struct is_callable<T, typename std::enable_if<std::is_fundamental<T>::value, std::true_type>::type> : public std::false_type {
};
template<typename T, typename = std::enable_if_t<is_callable<T>::value>>
metric_function make_function(T val, data_type dt) {
    return [dt, val] {
        return metric_value(val(), dt);
    };
}
template<typename T, typename = std::enable_if_t<!is_callable<T>::value>>
metric_function make_function(T& val, data_type dt) {
    return [dt, &val] {
        return metric_value(val, dt);
    };
}
}
extern const bool metric_disabled;
extern label shard_label;
extern label type_label;
template<typename T>
impl::metric_definition_impl make_gauge(metric_name_type name,
        T&& val, description d=description(), std::vector<label_instance> labels = {}) ;
template<typename T>
impl::metric_definition_impl make_gauge(metric_name_type name,
        description d, T&& val) ;
template<typename T>
impl::metric_definition_impl make_gauge(metric_name_type name,
        description d, std::vector<label_instance> labels, T&& val) ;
template<typename T>
impl::metric_definition_impl make_derive(metric_name_type name,
        T&& val, description d=description(), std::vector<label_instance> labels = {}) ;
template<typename T>
impl::metric_definition_impl make_derive(metric_name_type name, description d,
        T&& val) ;
template<typename T>
impl::metric_definition_impl make_derive(metric_name_type name, description d, std::vector<label_instance> labels,
        T&& val) ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
 ;
}
}
#include <deque>
namespace seastar {
using shard_id = unsigned;
class smp_service_group;
class reactor_backend_selector;
namespace internal {
;
}
shard_id this_shard_id();
struct smp_service_group_config {
    unsigned max_nonlocal_requests = 0;
};
class smp_service_group {
    unsigned _id;
private:
    explicit smp_service_group(unsigned id)  ;
    ;
    friend smp_service_group default_smp_service_group();
    friend future<smp_service_group> create_smp_service_group(smp_service_group_config ssgc);
};

smp_service_group default_smp_service_group();
future<smp_service_group> create_smp_service_group(smp_service_group_config ssgc);
future<> destroy_smp_service_group(smp_service_group ssg);
inline
smp_service_group default_smp_service_group() {
    return smp_service_group(0);
}
using smp_timeout_clock = lowres_clock;
using smp_service_group_semaphore = basic_semaphore<named_semaphore_exception_factory, smp_timeout_clock>;
using smp_service_group_semaphore_units = semaphore_units<named_semaphore_exception_factory, smp_timeout_clock>;
static constexpr smp_timeout_clock::time_point smp_no_timeout = smp_timeout_clock::time_point::max();
struct smp_submit_to_options {
    smp_service_group service_group = default_smp_service_group();
    smp_timeout_clock::time_point timeout = smp_no_timeout;
    smp_submit_to_options(smp_service_group service_group = default_smp_service_group(), smp_timeout_clock::time_point timeout = smp_no_timeout)
        : service_group(service_group)
        , timeout(timeout) {
    }
};


class smp_message_queue {
    static constexpr size_t queue_length = 128;
    static constexpr size_t batch_size = 16;
    static constexpr size_t prefetch_cnt = 2;
    struct work_item;
    struct lf_queue_remote {
        reactor* remote;
    };
    using lf_queue_base = boost::lockfree::spsc_queue<work_item*,
                            boost::lockfree::capacity<queue_length>>;
    struct lf_queue : lf_queue_remote, lf_queue_base {
        
        
        
    };
    lf_queue _pending;
    lf_queue _completed;
    struct alignas(seastar::cache_line_size) {
        size_t _sent = 0;
        size_t _compl = 0;
        size_t _last_snt_batch = 0;
        size_t _last_cmpl_batch = 0;
        size_t _current_queue_length = 0;
    };
    metrics::metric_groups _metrics;
    struct alignas(seastar::cache_line_size) {
        size_t _received = 0;
        size_t _last_rcv_batch = 0;
    };
    struct work_item : public task {
        explicit work_item(smp_service_group ssg) : task(current_scheduling_group()), ssg(ssg) {}
        smp_service_group ssg;
        virtual ~work_item() {}
        
        
        
    };
    template <typename Func>
    struct async_work_item : work_item {
        smp_message_queue& _queue;
        Func _func;
        using futurator = futurize<std::result_of_t<Func()>>;
        using future_type = typename futurator::type;
        using value_type = typename future_type::value_type;
        compat::optional<value_type> _result;
        std::exception_ptr _ex; 
        typename futurator::promise_type _promise; 
        
        
        
        
        
    };
    union tx_side {
        
        
        
        struct aa {
            std::deque<work_item*> pending_fifo;
        } a;
    } _tx;
    std::vector<work_item*> _completed_fifo;
public:
    
    
    template <typename Func>
    futurize_t<std::result_of_t<Func()>> submit(shard_id t, smp_submit_to_options options, Func&& func) {
        auto wi = std::make_unique<async_work_item<Func>>(*this, options.service_group, std::forward<Func>(func));
        auto fut = wi->get_future();
        submit_item(t, options.timeout, std::move(wi));
        return fut;
    }
    void start(unsigned cpuid);
    ;
    
    
    
private:
    
    
    void respond(work_item* wi);
    
    
    
    
    
    
    friend class smp;
};
class smp {
    static std::vector<posix_thread> _threads;
    static std::vector<std::function<void ()>> _thread_loops; 
    static compat::optional<boost::barrier> _all_event_loops_done;
    static std::vector<reactor*> _reactors;
    struct qs_deleter {
      
    };
    static std::unique_ptr<smp_message_queue*[], qs_deleter> _qs;
    static std::thread::id _tmain;
    static bool _using_dpdk;
    template <typename Func>
    using returns_future = is_future<std::result_of_t<Func()>>;
    template <typename Func>
    using returns_void = std::is_same<std::result_of_t<Func()>, void>;
public:
    
    
    
    
    
    
    
    
    template <typename Func>
    static futurize_t<std::result_of_t<Func()>> submit_to(unsigned t, smp_submit_to_options options, Func&& func) {
        using ret_type = std::result_of_t<Func()>;
        if (t == this_shard_id()) {
            try {
                if (!is_future<ret_type>::value) {
                    return futurize<ret_type>::apply(std::forward<Func>(func));
                } else if (std::is_lvalue_reference<Func>::value) {
                    return futurize<ret_type>::apply(func);
                } else {
                    auto w = std::make_unique<std::decay_t<Func>>(std::move(func));
                    auto ret = futurize<ret_type>::apply(*w);
                    return ret.finally([w = std::move(w)] {});
                }
            } catch (...) {
                return futurize<std::result_of_t<Func()>>::make_exception_future(std::current_exception());
            }
        } else {
            return _qs[t][this_shard_id()].submit(t, options, std::forward<Func>(func));
        }
    }
    template <typename Func>
    static futurize_t<std::result_of_t<Func()>> submit_to(unsigned t, Func&& func) {
        return submit_to(t, default_smp_service_group(), std::forward<Func>(func));
    }
    static bool poll_queues();
    
    
     ;
     ;
     ;
     ;
private:
    
    
    
    
public:
    static unsigned count;
};
}
namespace seastar {
class kernel_completion {
protected:
    
public:
    
};
}
namespace seastar {
namespace internal {
class io_request {
public:
    enum class operation { read, readv, write, writev, fdatasync, recv, recvmsg, send, sendmsg, accept, connect, poll_add, poll_remove, cancel };
private:
    operation _op;
    int _fd;
    union {
        uint64_t pos;
        int flags;
        int events;
    } _attr;
    union {
        char* addr;
        ::iovec* iovec;
        ::msghdr* msghdr;
        ::sockaddr* sockaddr;
    } _ptr;
    union {
        size_t len;
        socklen_t* socklen_ptr;
        socklen_t socklen;
    } _size;
    kernel_completion* _kernel_completion;
    
    
    
    
    
    
    
    
public:
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    static io_request make_accept(int fd, struct sockaddr* addr, socklen_t* addrlen, int flags) ;
    static io_request make_connect(int fd, struct sockaddr* addr, socklen_t addrlen) ;
    static io_request make_poll_add(int fd, int events) ;
    static io_request make_poll_remove(int fd, void *addr) ;
    static io_request make_cancel(int fd, void *addr) ;
};
}
}
#include <boost/intrusive_ptr.hpp>
namespace seastar {
class reactor;
class pollable_fd;
class pollable_fd_state;
class socket_address;
namespace internal {
class buffer_allocator;
}
namespace net {
class packet;
}
class pollable_fd_state;
using pollable_fd_state_ptr = boost::intrusive_ptr<pollable_fd_state>;
class pollable_fd_state {
    unsigned _refs = 0;
public:
    virtual ~pollable_fd_state() ;
    struct speculation {
        int events = 0;
        explicit speculation(int epoll_events_guessed = 0)  ;
    };
    pollable_fd_state(const pollable_fd_state&) = delete;
    void operator=(const pollable_fd_state&) = delete;
    void speculate_epoll(int events) ;
    file_desc fd;
    bool events_rw = false;   
    bool no_more_recv = false; 
    bool no_more_send = false; 
    int events_requested = 0; 
    int events_epoll = 0;     
    int events_known = 0;     
    friend class reactor;
    friend class pollable_fd;
    future<size_t> read_some(char* buffer, size_t size);
    future<size_t> read_some(uint8_t* buffer, size_t size);
    future<size_t> read_some(const std::vector<iovec>& iov);
    future<temporary_buffer<char>> read_some(internal::buffer_allocator* ba);
    future<> write_all(const char* buffer, size_t size);
    future<> write_all(const uint8_t* buffer, size_t size);
    future<size_t> write_some(net::packet& p);
    future<> write_all(net::packet& p);
    future<> readable();
    future<> writeable();
    future<> readable_or_writeable();
    void abort_reader();
    void abort_writer();
    future<std::tuple<pollable_fd, socket_address>> accept();
    future<> connect(socket_address& sa);
    future<size_t> sendmsg(struct msghdr *msg);
    future<size_t> recvmsg(struct msghdr *msg);
    future<size_t> sendto(socket_address addr, const void* buf, size_t len);
protected:
    explicit pollable_fd_state(file_desc fd, speculation speculate = speculation())  ;
private:
    void maybe_no_more_recv();
    void maybe_no_more_send();
    void forget(); 
    friend void intrusive_ptr_add_ref(pollable_fd_state* fd) ;
    friend void intrusive_ptr_release(pollable_fd_state* fd);
};
class pollable_fd {
public:
    using speculation = pollable_fd_state::speculation;
    pollable_fd(file_desc fd, speculation speculate = speculation());
public:
    future<size_t> read_some(char* buffer, size_t size) ;
    future<size_t> read_some(uint8_t* buffer, size_t size) ;
    future<size_t> read_some(const std::vector<iovec>& iov) ;
    future<temporary_buffer<char>> read_some(internal::buffer_allocator* ba) ;
    future<> write_all(const char* buffer, size_t size) ;
    future<> write_all(const uint8_t* buffer, size_t size) ;
    future<size_t> write_some(net::packet& p) ;
    future<> write_all(net::packet& p) ;
    future<> readable() ;
    future<> writeable() ;
    future<> readable_or_writeable() ;
    void abort_reader() ;
    void abort_writer() ;
    future<std::tuple<pollable_fd, socket_address>> accept() {
        return _s->accept();
    }
    future<> connect(socket_address& sa) ;
    future<size_t> sendmsg(struct msghdr *msg) ;
    future<size_t> recvmsg(struct msghdr *msg) ;
    future<size_t> sendto(socket_address addr, const void* buf, size_t len) ;
    file_desc& get_file_desc() const ;
    void shutdown(int how);
    void close() ;
protected:
    int get_fd() const ;
    void maybe_no_more_recv() ;
    void maybe_no_more_send() ;
    friend class reactor;
    friend class readable_eventfd;
    friend class writeable_eventfd;
    friend class aio_storage_context;
private:
    pollable_fd_state_ptr _s;
};
class writeable_eventfd;
class readable_eventfd {
    pollable_fd _fd;
public:
    explicit readable_eventfd(size_t initial = 0) : _fd(try_create_eventfd(initial)) {}
    readable_eventfd(readable_eventfd&&) = default;
    writeable_eventfd write_side();
    future<size_t> wait();
    int get_write_fd() { return _fd.get_fd(); }
private:
    explicit readable_eventfd(file_desc&& fd) : _fd(std::move(fd)) {}
    static file_desc try_create_eventfd(size_t initial);
    friend class writeable_eventfd;
};
class writeable_eventfd {
    file_desc _fd;
public:
    explicit writeable_eventfd(size_t initial = 0) : _fd(try_create_eventfd(initial)) {}
    writeable_eventfd(writeable_eventfd&&) = default;
    readable_eventfd read_side();
    void signal(size_t nr);
    int get_read_fd() { return _fd.get(); }
private:
    explicit writeable_eventfd(file_desc&& fd) : _fd(std::move(fd)) {}
    static file_desc try_create_eventfd(size_t initial);
    friend class readable_eventfd;
};
}
namespace seastar {
struct pollfn {
    virtual ~pollfn() {}
    virtual bool poll() = 0;
    virtual bool pure_poll() = 0;
    virtual bool try_enter_interrupt_mode() { return false; }
    virtual void exit_interrupt_mode() {}
};
}
struct _Unwind_Exception;
extern "C" int _Unwind_RaiseException(struct _Unwind_Exception *h);
namespace seastar {
using shard_id = unsigned;
namespace alien {
class message_queue;
}
class reactor;
inline
size_t iovec_len(const std::vector<iovec>& iov)
{
    size_t ret = 0;
    for (auto&& e : iov) {
        ret += e.iov_len;
    }
    return ret;
}
}
namespace std {
template <>
struct hash<::sockaddr_in> {
    size_t operator()(::sockaddr_in a) const {
        return a.sin_port ^ a.sin_addr.s_addr;
    }
};
}
bool operator==(const ::sockaddr_in a, const ::sockaddr_in b);
namespace seastar {

class thread_pool;
class smp;
class reactor_backend_selector;
class reactor_backend;
namespace internal {
class reactor_stall_sampler;
class cpu_stall_detector;
class buffer_allocator;
}
class kernel_completion;
class io_queue;
class disk_config_params;
class reactor {
    using sched_clock = std::chrono::steady_clock;
private:
    struct task_queue;
    using task_queue_list = circular_buffer_fixed_capacity<task_queue*, max_scheduling_groups()>;
    using pollfn = seastar::pollfn;
    class signal_pollfn;
    class batch_flush_pollfn;
    class smp_pollfn;
    class drain_cross_cpu_freelist_pollfn;
    class lowres_timer_pollfn;
    class manual_timer_pollfn;
    class epoll_pollfn;
    class reap_kernel_completions_pollfn;
    class kernel_submit_work_pollfn;
    class io_queue_submission_pollfn;
    class syscall_pollfn;
    class execution_stage_pollfn;
    friend signal_pollfn;
    friend batch_flush_pollfn;
    friend smp_pollfn;
    friend drain_cross_cpu_freelist_pollfn;
    friend lowres_timer_pollfn;
    friend class manual_clock;
    friend class epoll_pollfn;
    friend class reap_kernel_completions_pollfn;
    friend class kernel_submit_work_pollfn;
    friend class io_queue_submission_pollfn;
    friend class syscall_pollfn;
    friend class execution_stage_pollfn;
    friend class file_data_source_impl; 
    friend class internal::reactor_stall_sampler;
    friend class preempt_io_context;
    friend struct hrtimer_aio_completion;
    friend struct task_quota_aio_completion;
    friend class reactor_backend_epoll;
    friend class reactor_backend_aio;
    friend class reactor_backend_selector;
    friend class aio_storage_context;
public:
    class poller {
        std::unique_ptr<pollfn> _pollfn;
        class registration_task;
        class deregistration_task;
        registration_task* _registration_task = nullptr;
    public:
        
        
        
        
        
        void do_register() noexcept;
        friend class reactor;
    };
    enum class idle_cpu_handler_result {
        no_more_work,
        interrupted_by_higher_priority_task
    };
    using work_waiting_on_reactor = const noncopyable_function<bool()>&;
    using idle_cpu_handler = noncopyable_function<idle_cpu_handler_result(work_waiting_on_reactor)>;
    struct io_stats {
        uint64_t aio_reads = 0;
        uint64_t aio_read_bytes = 0;
        uint64_t aio_writes = 0;
        uint64_t aio_write_bytes = 0;
        uint64_t aio_errors = 0;
        uint64_t fstream_reads = 0;
        uint64_t fstream_read_bytes = 0;
        uint64_t fstream_reads_blocked = 0;
        uint64_t fstream_read_bytes_blocked = 0;
        uint64_t fstream_read_aheads_discarded = 0;
        uint64_t fstream_read_ahead_discarded_bytes = 0;
    };
private:
    reactor_config _cfg;
    file_desc _notify_eventfd;
    file_desc _task_quota_timer;
    std::unique_ptr<reactor_backend> _backend;
    sigset_t _active_sigmask; 
    std::vector<pollfn*> _pollers;
    static constexpr unsigned max_aio_per_queue = 128;
    static constexpr unsigned max_queues = 8;
    static constexpr unsigned max_aio = max_aio_per_queue * max_queues;
    friend disk_config_params;
    std::vector<std::unique_ptr<io_queue>> my_io_queues;
    std::unordered_map<dev_t, io_queue*> _io_queues;
    friend io_queue;
    std::vector<noncopyable_function<future<> ()>> _exit_funcs;
    unsigned _id = 0;
    bool _stopping = false;
    bool _stopped = false;
    bool _finished_running_tasks = false;
    condition_variable _stop_requested;
    bool _handle_sigint = true;
    compat::optional<future<std::unique_ptr<network_stack>>> _network_stack_ready;
    int _return = 0;
    promise<> _start_promise;
    semaphore _cpu_started;
    internal::preemption_monitor _preemption_monitor{};
    uint64_t _global_tasks_processed = 0;
    uint64_t _polls = 0;
    std::unique_ptr<internal::cpu_stall_detector> _cpu_stall_detector;
    unsigned _max_task_backlog = 1000;
    timer_set<timer<>, &timer<>::_link> _timers;
    timer_set<timer<>, &timer<>::_link>::timer_list_t _expired_timers;
    timer_set<timer<lowres_clock>, &timer<lowres_clock>::_link> _lowres_timers;
    timer_set<timer<lowres_clock>, &timer<lowres_clock>::_link>::timer_list_t _expired_lowres_timers;
    timer_set<timer<manual_clock>, &timer<manual_clock>::_link> _manual_timers;
    timer_set<timer<manual_clock>, &timer<manual_clock>::_link>::timer_list_t _expired_manual_timers;
    io_stats _io_stats;
    uint64_t _fsyncs = 0;
    uint64_t _cxx_exceptions = 0;
    uint64_t _abandoned_failed_futures = 0;
    struct task_queue {
        
        int64_t _vruntime = 0;
        float _shares;
        int64_t _reciprocal_shares_times_2_power_32;
        bool _current = false;
        bool _active = false;
        uint8_t _id;
        sched_clock::duration _runtime = {};
        uint64_t _tasks_processed = 0;
        circular_buffer<task*> _q;
        sstring _name;
        /**
         * This array holds pointers to the scheduling group specific
         * data. The pointer is not use as is but is cast to a reference
         * to the appropriate type that is actually pointed to.
         */
        std::vector<void*> _scheduling_group_specific_vals;
        
        
        struct indirect_compare;
        sched_clock::duration _time_spent_on_task_quota_violations = {};
        seastar::metrics::metric_groups _metrics;
        
    private:
        void register_stats();
    };
    circular_buffer<internal::io_request> _pending_io;
    boost::container::static_vector<std::unique_ptr<task_queue>, max_scheduling_groups()> _task_queues;
    std::vector<scheduling_group_key_config> _scheduling_group_key_configs;
    int64_t _last_vruntime = 0;
    task_queue_list _active_task_queues;
    task_queue_list _activating_task_queues;
    task_queue* _at_destroy_tasks;
    sched_clock::duration _task_quota;
    idle_cpu_handler _idle_cpu_handler{ [] (work_waiting_on_reactor) {return idle_cpu_handler_result::no_more_work;} };
    std::unique_ptr<network_stack> _network_stack;
    std::unique_ptr<lowres_clock_impl> _lowres_clock_impl;
    lowres_clock::time_point _lowres_next_timeout;
    compat::optional<poller> _epoll_poller;
    compat::optional<pollable_fd> _aio_eventfd;
    const bool _reuseport;
    circular_buffer<double> _loads;
    double _load = 0;
    sched_clock::duration _total_idle;
    sched_clock::duration _total_sleep;
    sched_clock::time_point _start_time = sched_clock::now();
    std::chrono::nanoseconds _max_poll_time = calculate_poll_time();
    circular_buffer<output_stream<char>* > _flush_batching;
    std::atomic<bool> _sleeping alignas(seastar::cache_line_size);
    pthread_t _thread_id alignas(seastar::cache_line_size) = pthread_self();
    bool _strict_o_direct = true;
    bool _force_io_getevents_syscall = false;
    bool _bypass_fsync = false;
    bool _have_aio_fsync = false;
    std::atomic<bool> _dying{false};
private:
    static std::chrono::nanoseconds calculate_poll_time();
    static void block_notifier(int);
    void wakeup();
    size_t handle_aio_error(internal::linux_abi::iocb* iocb, int ec);
    bool flush_pending_aio();
    bool flush_tcp_batches();
    bool do_expire_lowres_timers();
    bool do_check_lowres_timers() const;
    void expire_manual_timers();
    void abort_on_error(int ret);
    void start_aio_eventfd_loop();
    
    ;
    /**
     * Returns TRUE if all pollers allow blocking.
     *
     * @return FALSE if at least one of the blockers requires a non-blocking
     *         execution.
     */
    
    
    ;
public:
    
private:
    class signals {
    public:
        
        
        
        
        
        
        
        
    private:
        struct signal_handler {
            
            noncopyable_function<void ()> _handler;
        };
        std::atomic<uint64_t> _pending_signals;
        std::unordered_map<int, signal_handler> _signal_handlers;
        
    };
    signals _signals;
    std::unique_ptr<thread_pool> _thread_pool;
    friend class thread_pool;
    friend class internal::cpu_stall_detector;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    [[noreturn]] void no_such_scheduling_group(scheduling_group sg);
    
    
    
    
    void request_preemption();
    void start_handling_signal();
    void reset_preemption_monitor();
    void service_highres_timer();
    future<std::tuple<pollable_fd, socket_address>>
    do_accept(pollable_fd_state& listen_fd);
    future<> do_connect(pollable_fd_state& pfd, socket_address& sa);
    future<size_t>
    do_read_some(pollable_fd_state& fd, void* buffer, size_t size);
    future<size_t>
    do_read_some(pollable_fd_state& fd, const std::vector<iovec>& iov);
    future<temporary_buffer<char>>
    do_read_some(pollable_fd_state& fd, internal::buffer_allocator* ba);
    future<size_t>
    do_write_some(pollable_fd_state& fd, const void* buffer, size_t size);
    future<size_t>
    do_write_some(pollable_fd_state& fd, net::packet& p);
public:
    static boost::program_options::options_description get_options_description(reactor_config cfg);
    explicit reactor(unsigned id, reactor_backend_selector rbs, reactor_config cfg);
    reactor(const reactor&) = delete;
    ~reactor();
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    future<compat::optional<directory_entry_type>>  file_type(sstring name, follow_symlink = follow_symlink::yes);
    
    future<uint64_t> file_size(sstring pathname);
    future<bool> file_accessible(sstring pathname, access_flags flags);
    future<bool> file_exists(sstring pathname) ;
    future<fs_type> file_system_at(sstring pathname);
    future<struct statvfs> statvfs(sstring pathname);
    future<> remove_file(sstring pathname);
    future<> rename_file(sstring old_pathname, sstring new_pathname);
    future<> link_file(sstring oldpath, sstring newpath);
    future<> chmod(sstring name, file_permissions permissions);
    void submit_io(kernel_completion* desc, internal::io_request req);
    future<size_t> submit_io_read(io_queue* ioq,
            const io_priority_class& priority_class,
            size_t len,
            internal::io_request req);
    future<size_t> submit_io_write(io_queue* ioq,
            const io_priority_class& priority_class,
            size_t len,
            internal::io_request req);
     void handle_io_result(ssize_t res) ;
    int run();
    void exit(int ret);
    future<> when_started() ;
    template <typename Rep, typename Period>
    future<> wait_for_stop(std::chrono::duration<Rep, Period> timeout) ;
    void at_exit(noncopyable_function<future<> ()> func);
     ;
    
    
    
    
    
    
    
    
    
    steady_clock_type::duration total_busy_time();
    std::chrono::nanoseconds total_steal_time();
    const io_stats& get_io_stats() const ;
    uint64_t abandoned_failed_futures() const ;
private:
    /**
     * Add a new "poller" - a non-blocking function returning a boolean, that
     * will be called every iteration of a main loop.
     * If it returns FALSE then reactor's main loop is forbidden to block in the
     * current iteration.
     *
     * @param fn a new "poller" function to register
     */
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    friend class alien::message_queue;
    friend class pollable_fd;
    friend class pollable_fd_state;
    friend struct pollable_fd_state_deleter;
    friend class posix_file_impl;
    friend class blockdev_file_impl;
    friend class readable_eventfd;
    friend class timer<>;
    friend class timer<lowres_clock>;
    friend class timer<manual_clock>;
    friend class smp;
    friend class smp_message_queue;
    friend class poller;
    friend class scheduling_group;
    
    friend int ::_Unwind_RaiseException(struct _Unwind_Exception *h);
    
    metrics::metric_groups _metric_groups;
    
    
    
    
    ;
    ;
    template<typename SpecificValType, typename Mapper, typename Reducer, typename Initial>
        GCC6_CONCEPT( requires requires(SpecificValType specific_val, Mapper mapper, Reducer reducer, Initial initial) {
            {reducer(initial, mapper(specific_val))} -> Initial;
        })
    friend future<typename function_traits<Reducer>::return_type>
    map_reduce_scheduling_group_specific(Mapper mapper, Reducer reducer, Initial initial_val, scheduling_group_key key);
    template<typename SpecificValType, typename Reducer, typename Initial>
    GCC6_CONCEPT( requires requires(SpecificValType specific_val, Reducer reducer, Initial initial) {
        {reducer(initial, specific_val)} -> Initial;
    })
    friend future<typename function_traits<Reducer>::return_type>
        reduce_scheduling_group_specific(Reducer reducer, Initial initial_val, scheduling_group_key key);
public:
    
    
    
    
    
    
    
    
    
    
    
    
};

extern __thread reactor* local_engine;
extern __thread size_t task_quota;
 
 

size_t iovec_len(const iovec* begin, size_t len)
;
 int hrtimer_signal() ;
extern logger seastar_logger;
}
namespace seastar {
template <typename T>
class queue {
    std::queue<T, circular_buffer<T>> _q;
    size_t _max;
    compat::optional<promise<>> _not_empty;
    compat::optional<promise<>> _not_full;
    std::exception_ptr _ex = nullptr;
private:
    void notify_not_empty();
    void notify_not_full();
public:
    explicit queue(size_t size);
    bool push(T&& a);
    
    ;
    bool empty() const;
    bool full() const;
    ;
    ;
    
    
    
    
    
    
    
};








template <typename T>
inline
bool queue<T>::empty() const {
    return _q.empty();
}
template <typename T>
inline
bool queue<T>::full() const {
    return _q.size() >= _max;
}


}
namespace seastar {
using rss_key_type = compat::basic_string_view<uint8_t>;
static constexpr uint8_t default_rsskey_40bytes_v[] = {
    0xd1, 0x81, 0xc6, 0x2c, 0xf7, 0xf4, 0xdb, 0x5b,
    0x19, 0x83, 0xa2, 0xfc, 0x94, 0x3e, 0x1a, 0xdb,
    0xd9, 0x38, 0x9e, 0x6b, 0xd1, 0x03, 0x9c, 0x2c,
    0xa7, 0x44, 0x99, 0xad, 0x59, 0x3d, 0x56, 0xd9,
    0xf3, 0x25, 0x3c, 0x06, 0x2a, 0xdc, 0x1f, 0xfc
};
static constexpr rss_key_type default_rsskey_40bytes{default_rsskey_40bytes_v, sizeof(default_rsskey_40bytes_v)};
static constexpr uint8_t default_rsskey_52bytes_v[] = {
    0x44, 0x39, 0x79, 0x6b, 0xb5, 0x4c, 0x50, 0x23,
    0xb6, 0x75, 0xea, 0x5b, 0x12, 0x4f, 0x9f, 0x30,
    0xb8, 0xa2, 0xc0, 0x3d, 0xdf, 0xdc, 0x4d, 0x02,
    0xa0, 0x8c, 0x9b, 0x33, 0x4a, 0xf6, 0x4a, 0x4c,
    0x05, 0xc6, 0xfa, 0x34, 0x39, 0x58, 0xd8, 0x55,
    0x7d, 0x99, 0x58, 0x3a, 0xe1, 0x38, 0xc9, 0x2e,
    0x81, 0x15, 0x03, 0x66
};
static constexpr rss_key_type default_rsskey_52bytes{default_rsskey_52bytes_v, sizeof(default_rsskey_52bytes_v)};

}
namespace seastar {
namespace net {
struct ethernet_address {
    
    
    
    std::array<uint8_t, 6> mac;
     ;
    
    
    void write(char* p) const ;
    void produce(char*& p) const ;
    static constexpr size_t size() ;
} __attribute__((packed));
std::ostream& operator<<(std::ostream& os, ethernet_address ea);
struct ethernet {
    using address = ethernet_address;
    static address broadcast_address() ;
    static constexpr uint16_t arp_hardware_type() ;
};
struct eth_hdr {
    ethernet_address dst_mac;
    ethernet_address src_mac;
    packed<uint16_t> eth_proto;
    template <typename Adjuster>
    auto adjust_endianness(Adjuster a) ;
} __attribute__((packed));

}
}
namespace seastar {
namespace net {
class packet;
class interface;
class device;
class qp;
class l3_protocol;
class forward_hash {
    uint8_t data[64];
    size_t end_idx = 0;
public:
    
    
    
    
    
};
struct hw_features {
    bool tx_csum_ip_offload = false;
    bool tx_csum_l4_offload = false;
    bool rx_csum_offload = false;
    bool rx_lro = false;
    bool tx_tso = false;
    bool tx_ufo = false;
    uint16_t mtu = 1500;
    uint16_t max_packet_len = ip_packet_len_max - eth_hdr_len;
};
class l3_protocol {
public:
    struct l3packet {
        eth_protocol_num proto_num;
        ethernet_address to;
        packet p;
    };
    using packet_provider_type = std::function<compat::optional<l3packet> ()>;
private:
    interface* _netif;
    eth_protocol_num _proto_num;
public:
    
    
private:
    friend class interface;
};
class interface {
    struct l3_rx_stream {
        stream<packet, ethernet_address> packet_stream;
        future<> ready;
        std::function<bool (forward_hash&, packet&, size_t)> forward;
        
    };
    std::unordered_map<uint16_t, l3_rx_stream> _proto_map;
    std::shared_ptr<device> _dev;
    ethernet_address _hw_address;
    net::hw_features _hw_features;
    std::vector<l3_protocol::packet_provider_type> _pkt_providers;
private:
    
public:
    
    
    
    
    
    
    
    
    
    friend class l3_protocol;
};
struct qp_stats_good {
    
    
    
    uint64_t bytes;      
    uint64_t nr_frags;   
    uint64_t copy_frags; 
    uint64_t copy_bytes; 
    uint64_t packets;    
    uint64_t last_bunch; 
};
struct qp_stats {
    
    struct {
        struct qp_stats_good good;
        struct {
            
            
            uint64_t no_mem;       
            uint64_t total;        
            uint64_t csum;         
        } bad;
    } rx;
    struct {
        struct qp_stats_good good;
        uint64_t linearized;       
    } tx;
};
class qp {
    using packet_provider_type = std::function<compat::optional<packet> ()>;
    std::vector<packet_provider_type> _pkt_providers;
    compat::optional<std::array<uint8_t, 128>> _sw_reta;
    circular_buffer<packet> _proxy_packetq;
    stream<packet> _rx_stream;
    reactor::poller _tx_poller;
    circular_buffer<packet> _tx_packetq;
protected:
    const std::string _stats_plugin_name;
    const std::string _queue_name;
    metrics::metric_groups _metrics;
    qp_stats _stats;
public:
    
    
    
    
    ;
    
    
    
    
    
    friend class device;
};
class device {
protected:
    std::unique_ptr<qp*[]> _queues;
    size_t _rss_table_bits = 0;
public:
    
    ;
    
    
    
    
    
    
    
    virtual uint16_t hw_queues_count() ;
    virtual future<> link_ready() ;
    virtual std::unique_ptr<qp> init_local_queue(boost::program_options::variables_map opts, uint16_t qid) = 0;
    virtual unsigned hash2qid(uint32_t hash) ;
    void set_local_queue(std::unique_ptr<qp> dev);
    template <typename Func>
    unsigned forward_dst(unsigned src_cpuid, Func&& hashfn) ;
    virtual unsigned hash2cpu(uint32_t hash) ;
};
}
}
namespace seastar {
namespace net {
class arp;
class arp_for_protocol;
template <typename L3>
class arp_for;
class arp_for_protocol {
protected:
    arp& _arp;
    uint16_t _proto_num;
public:
    arp_for_protocol(arp& a, uint16_t proto_num);
    virtual ~arp_for_protocol();
    virtual future<> received(packet p) = 0;
    virtual bool forward(forward_hash& out_hash_data, packet& p, size_t off) ;
};
class arp {
    interface* _netif;
    l3_protocol _proto;
    std::unordered_map<uint16_t, arp_for_protocol*> _arp_for_protocol;
    circular_buffer<l3_protocol::l3packet> _packetq;
private:
    struct arp_hdr {
        uint16_t htype;
        uint16_t ptype;
        static arp_hdr read(const char* p) ;
        static constexpr size_t size() ;
    };
public:
    explicit arp(interface* netif);
    void add(uint16_t proto_num, arp_for_protocol* afp);
    void del(uint16_t proto_num);
private:
    ethernet_address l2self() ;
    future<> process_packet(packet p, ethernet_address from);
    bool forward(forward_hash& out_hash_data, packet& p, size_t off);
    compat::optional<l3_protocol::l3packet> get_packet();
    template <class l3_proto>
    friend class arp_for;
};
template <typename L3>
class arp_for : public arp_for_protocol {
public:
    using l2addr = ethernet_address;
    using l3addr = typename L3::address_type;
private:
    static constexpr auto max_waiters = 512;
    enum oper {
        op_request = 1,
        op_reply = 2,
    };
    struct arp_hdr {
        uint16_t htype;
        uint16_t ptype;
        uint8_t hlen;
        uint8_t plen;
        uint16_t oper;
        l2addr sender_hwaddr;
        l3addr sender_paddr;
        l2addr target_hwaddr;
        l3addr target_paddr;
        static arp_hdr read(const char* p) ;
        void write(char* p) const ;
        static constexpr size_t size() ;
    };
    struct resolution {
        std::vector<promise<l2addr>> _waiters;
        timer<> _timeout_timer;
    };
private:
    l3addr _l3self = L3::broadcast_address();
    std::unordered_map<l3addr, l2addr> _table;
    std::unordered_map<l3addr, resolution> _in_progress;
private:
    packet make_query_packet(l3addr paddr);
    virtual future<> received(packet p) override;
    future<> handle_request(arp_hdr* ah);
    l2addr l2self() { return _arp.l2self(); }
    void send(l2addr to, packet p);
public:
    future<> send_query(const l3addr& paddr);
    explicit arp_for(arp& a) : arp_for_protocol(a, L3::arp_protocol_type()) {
        _table[L3::broadcast_address()] = ethernet::broadcast_address();
    }
    future<ethernet_address> lookup(const l3addr& addr);
    void learn(l2addr l2, l3addr l3);
    void run();
    void set_self_addr(l3addr addr) {
        _table.erase(_l3self);
        _table[addr] = l2self();
        _l3self = addr;
    }
    friend class arp;
};
template <typename L3>
packet
arp_for<L3>::make_query_packet(l3addr paddr) {
    arp_hdr hdr;
    hdr.htype = ethernet::arp_hardware_type();
    hdr.ptype = L3::arp_protocol_type();
    hdr.hlen = sizeof(l2addr);
    hdr.plen = sizeof(l3addr);
    hdr.oper = op_request;
    hdr.sender_hwaddr = l2self();
    hdr.sender_paddr = _l3self;
    hdr.target_hwaddr = ethernet::broadcast_address();
    hdr.target_paddr = paddr;
    auto p = packet();
    p.prepend_uninitialized_header(hdr.size());
    hdr.write(p.get_header(0, hdr.size()));
    return p;
}
template <typename L3>
void arp_for<L3>::send(l2addr to, packet p) {
    _arp._packetq.push_back(l3_protocol::l3packet{eth_protocol_num::arp, to, std::move(p)});
}
template <typename L3>
future<>
arp_for<L3>::send_query(const l3addr& paddr) {
    send(ethernet::broadcast_address(), make_query_packet(paddr));
    return make_ready_future<>();
}
class arp_error : public std::runtime_error {
public:
    arp_error(const std::string& msg) : std::runtime_error(msg) {}
};
class arp_timeout_error : public arp_error {
public:
    arp_timeout_error() : arp_error("ARP timeout") {}
};
class arp_queue_full_error : public arp_error {
public:
    arp_queue_full_error() : arp_error("ARP waiter's queue is full") {}
};
template <typename L3>
future<ethernet_address>
arp_for<L3>::lookup(const l3addr& paddr) {
    auto i = _table.find(paddr);
    if (i != _table.end()) {
        return make_ready_future<ethernet_address>(i->second);
    }
    auto j = _in_progress.find(paddr);
    auto first_request = j == _in_progress.end();
    auto& res = first_request ? _in_progress[paddr] : j->second;
    if (first_request) {
        res._timeout_timer.set_callback([paddr, this, &res] {
            (void)send_query(paddr);
            for (auto& w : res._waiters) {
                w.set_exception(arp_timeout_error());
            }
            res._waiters.clear();
        });
        res._timeout_timer.arm_periodic(std::chrono::seconds(1));
        (void)send_query(paddr);
    }
    if (res._waiters.size() >= max_waiters) {
        return make_exception_future<ethernet_address>(arp_queue_full_error());
    }
    res._waiters.emplace_back();
    return res._waiters.back().get_future();
}
template <typename L3>
void
arp_for<L3>::learn(l2addr hwaddr, l3addr paddr) {
    _table[paddr] = hwaddr;
    auto i = _in_progress.find(paddr);
    if (i != _in_progress.end()) {
        auto& res = i->second;
        res._timeout_timer.cancel();
        for (auto &&pr : res._waiters) {
            pr.set_value(hwaddr);
        }
        _in_progress.erase(i);
    }
}
template <typename L3>
future<>
arp_for<L3>::received(packet p) {
    auto ah = p.get_header(0, arp_hdr::size());
    if (!ah) {
        return make_ready_future<>();
    }
    auto h = arp_hdr::read(ah);
    if (h.hlen != sizeof(l2addr) || h.plen != sizeof(l3addr)) {
        return make_ready_future<>();
    }
    switch (h.oper) {
    case op_request:
        return handle_request(&h);
    case op_reply:
        arp_learn(h.sender_hwaddr, h.sender_paddr);
        return make_ready_future<>();
    default:
        return make_ready_future<>();
    }
}

}
}
#include <arpa/inet.h>
namespace seastar {
namespace net {
uint16_t ip_checksum(const void* data, size_t len);
struct checksummer {
    __int128 csum = 0;
    bool odd = false;
    void sum(const char* data, size_t len);
    void sum(const packet& p);
    void sum(uint8_t data) ;
    void sum(uint16_t data) ;
    void sum(uint32_t data) ;
    void sum_many() ;
    template <typename T0, typename... T>
    void sum_many(T0 data, T... rest) ;
    uint16_t get() const;
};
}
}
#include <iostream>
namespace seastar {
namespace net {
template <typename Offset, typename Tag>
class packet_merger {
private:
    static uint64_t& linearizations_ref() ;
public:
    std::map<Offset, packet> map;
    static uint64_t linearizations() {
        return linearizations_ref();
    }
    void merge(Offset offset, packet p) {
        bool insert = true;
        auto beg = offset;
        auto end = beg + p.len();
        for (auto it = map.begin(); it != map.end();) {
            auto& seg_pkt = it->second;
            auto seg_beg = it->first;
            auto seg_end = seg_beg + seg_pkt.len();
            if (seg_beg <= beg && end <= seg_end) {
                return;
            } else if (beg <= seg_beg && seg_end <= end) {
                it = map.erase(it);
                insert = true;
                break;
            } else if (beg < seg_beg && seg_beg <= end && end <= seg_end) {
                auto trim = end - seg_beg;
                seg_pkt.trim_front(trim);
                p.append(std::move(seg_pkt));
                it = map.erase(it);
                insert = true;
                break;
            } else if (seg_beg <= beg && beg <= seg_end && seg_end < end) {
                auto trim = seg_end - beg;
                p.trim_front(trim);
                seg_pkt.append(std::move(p));
                seg_pkt.linearize();
                ++linearizations_ref();
                insert = false;
                break;
            } else {
                it++;
                insert = true;
            }
        }
        if (insert) {
            p.linearize();
            ++linearizations_ref();
            map.emplace(beg, std::move(p));
        }
        for (auto it = map.begin(); it != map.end();) {
            auto& seg_pkt = it->second;
            auto seg_beg = it->first;
            auto seg_end = seg_beg + seg_pkt.len();
            auto it_next = it;
            it_next++;
            if (it_next == map.end()) {
                break;
            }
            auto& p = it_next->second;
            auto beg = it_next->first;
            auto end = beg + p.len();
            if (seg_beg <= beg && beg <= seg_end && seg_end < end) {
                auto trim = seg_end - beg;
                p.trim_front(trim);
                seg_pkt.append(std::move(p));
                map.erase(it_next);
                continue;
            } else if (end <= seg_end) {
                map.erase(it_next);
                continue;
            } else if (seg_end < beg) {
                it = it_next;
                continue;
            } else {
                std::cerr << "packet_merger: merge error\n";
                abort();
                break;
            }
        }
    }
};
}
}
namespace seastar {
namespace net {
struct udp_hdr {
    packed<uint16_t> src_port;
    packed<uint16_t> dst_port;
    packed<uint16_t> len;
    packed<uint16_t> cksum;
    template<typename Adjuster>
    auto adjust_endianness(Adjuster a) {
        return a(src_port, dst_port, len, cksum);
    }
} __attribute__((packed));
struct udp_channel_state {
    queue<udp_datagram> _queue;
    semaphore _user_queue_space = {212992};
    udp_channel_state(size_t queue_size) : _queue(queue_size) {}
    future<> wait_for_send_buffer(size_t len) { return _user_queue_space.wait(len); }
    
};
}
}
namespace seastar {
struct ipv6_addr;
namespace net {
class ipv4;
template <ip_protocol_num ProtoNum>
class ipv4_l4;
struct ipv4_address;
template <typename InetTraits>
class tcp;
struct ipv4_address {
    
    
    
    
    packed<uint32_t> ip;
    
    
    
    
    
    
    
    
} __attribute__((packed));


struct ipv6_address {
    using ipv6_bytes = std::array<uint8_t, 16>;
    static_assert(alignof(ipv6_bytes) == 1, "ipv6_bytes should be byte-aligned");
    static_assert(sizeof(ipv6_bytes) == 16, "ipv6_bytes should be 16 bytes");
    
    
    
    
    
    ipv6_bytes ip;
     ;
    
    
    
    
    
    
    
    void produce(char*& p) const;
    static constexpr size_t size() ;
} __attribute__((packed));
std::ostream& operator<<(std::ostream&, const ipv6_address&);
}
}
namespace std {
template <>
struct hash<seastar::net::ipv4_address> {
    size_t operator()(seastar::net::ipv4_address a) const { return a.ip; }
};
template <>
struct hash<seastar::net::ipv6_address> {
    size_t operator()(const seastar::net::ipv6_address&) const;
};
}
namespace seastar {
namespace net {
struct ipv4_traits {
    using address_type = ipv4_address;
    using inet_type = ipv4_l4<ip_protocol_num::tcp>;
    struct l4packet {
        ipv4_address to;
        packet p;
        ethernet_address e_dst;
        ip_protocol_num proto_num;
    };
    using packet_provider_type = std::function<compat::optional<l4packet> ()>;
    static void tcp_pseudo_header_checksum(checksummer& csum, ipv4_address src, ipv4_address dst, uint16_t len) ;
    static void udp_pseudo_header_checksum(checksummer& csum, ipv4_address src, ipv4_address dst, uint16_t len) {
        csum.sum_many(src.ip.raw, dst.ip.raw, uint8_t(0), uint8_t(ip_protocol_num::udp), len);
    }
    static constexpr uint8_t ip_hdr_len_min = ipv4_hdr_len_min;
};
template <ip_protocol_num ProtoNum>
class ipv4_l4 {
public:
    ipv4& _inet;
public:
    ipv4_l4(ipv4& inet)  ;
    void register_packet_provider(ipv4_traits::packet_provider_type func);
    future<ethernet_address> get_l2_dst_address(ipv4_address to);
    const ipv4& inet() const ;
};
class ip_protocol {
public:
    virtual ~ip_protocol() ;
    virtual void received(packet p, ipv4_address from, ipv4_address to) = 0;
    virtual bool forward(forward_hash& out_hash_data, packet& p, size_t off) ;
};
template <typename InetTraits>
struct l4connid {
    using ipaddr = typename InetTraits::address_type;
    using inet_type = typename InetTraits::inet_type;
    struct connid_hash;
    ipaddr local_ip;
    ipaddr foreign_ip;
    uint16_t local_port;
    uint16_t foreign_port;
    bool operator==(const l4connid& x) const ;
    uint32_t hash(rss_key_type rss_key) ;
};
class ipv4_tcp final : public ip_protocol {
    ipv4_l4<ip_protocol_num::tcp> _inet_l4;
    std::unique_ptr<tcp<ipv4_traits>> _tcp;
public:
    ipv4_tcp(ipv4& inet);
    ~ipv4_tcp();
    virtual void received(packet p, ipv4_address from, ipv4_address to) override;
    virtual bool forward(forward_hash& out_hash_data, packet& p, size_t off) override;
    friend class ipv4;
};
struct icmp_hdr {
    enum class msg_type : uint8_t {
        echo_reply = 0,
        echo_request = 8,
    };
    msg_type type;
    uint8_t code;
    packed<uint16_t> csum;
    packed<uint32_t> rest;
    template <typename Adjuster>
    auto adjust_endianness(Adjuster a) ;
} __attribute__((packed));
class icmp {
public:
    using ipaddr = ipv4_address;
    using inet_type = ipv4_l4<ip_protocol_num::icmp>;
    explicit icmp(inet_type& inet) : _inet(inet) {
        _inet.register_packet_provider([this] {
            compat::optional<ipv4_traits::l4packet> l4p;
            if (!_packetq.empty()) {
                l4p = std::move(_packetq.front());
                _packetq.pop_front();
                _queue_space.signal(l4p.value().p.len());
            }
            return l4p;
        });
    }
    void received(packet p, ipaddr from, ipaddr to);
private:
    inet_type& _inet;
    circular_buffer<ipv4_traits::l4packet> _packetq;
    semaphore _queue_space = {212992};
};
class ipv4_icmp final : public ip_protocol {
    ipv4_l4<ip_protocol_num::icmp> _inet_l4;
    icmp _icmp;
public:
    ipv4_icmp(ipv4& inet) : _inet_l4(inet), _icmp(_inet_l4) {}
    virtual void received(packet p, ipv4_address from, ipv4_address to) {
        _icmp.received(std::move(p), from, to);
    }
    friend class ipv4;
};
class ipv4_udp : public ip_protocol {
    using connid = l4connid<ipv4_traits>;
    using connid_hash = typename connid::connid_hash;
public:
    static const int default_queue_size;
private:
    static const uint16_t min_anonymous_port = 32768;
    ipv4 &_inet;
    std::unordered_map<uint16_t, lw_shared_ptr<udp_channel_state>> _channels;
    int _queue_size = default_queue_size;
    uint16_t _next_anonymous_port = min_anonymous_port;
    circular_buffer<ipv4_traits::l4packet> _packetq;
private:
    uint16_t next_port(uint16_t port);
public:
    class registration {
    private:
        ipv4_udp &_proto;
        uint16_t _port;
    public:
        registration(ipv4_udp &proto, uint16_t port) : _proto(proto), _port(port) {};
        void unregister() ;
        uint16_t port() const ;
    };
    ipv4_udp(ipv4& inet);
    udp_channel make_channel(ipv4_addr addr);
    virtual void received(packet p, ipv4_address from, ipv4_address to) override;
    void send(uint16_t src_port, ipv4_addr dst, packet &&p);
    bool forward(forward_hash& out_hash_data, packet& p, size_t off) override;
    void set_queue_size(int size) ;
    const ipv4& inet() const ;
};
struct ip_hdr;
struct ip_packet_filter {
    virtual ~ip_packet_filter() ;;
    virtual future<> handle(packet& p, ip_hdr* iph, ethernet_address from, bool & handled) = 0;
};
struct ipv4_frag_id {
    struct hash;
    ipv4_address src_ip;
    ipv4_address dst_ip;
    uint16_t identification;
    uint8_t protocol;
    bool operator==(const ipv4_frag_id& x) const ;
};
struct ipv4_frag_id::hash : private std::hash<ipv4_address>,
    private std::hash<uint16_t>, private std::hash<uint8_t> {
    size_t operator()(const ipv4_frag_id& id) const noexcept ;
};
struct ipv4_tag {};
using ipv4_packet_merger = packet_merger<uint32_t, ipv4_tag>;
class ipv4 {
public:
    using clock_type = lowres_clock;
    using address_type = ipv4_address;
    using proto_type = uint16_t;
    static address_type broadcast_address() ;
    
private:
    interface* _netif;
    std::vector<ipv4_traits::packet_provider_type> _pkt_providers;
    arp _global_arp;
    arp_for<ipv4> _arp;
    ipv4_address _host_address;
    ipv4_address _gw_address;
    ipv4_address _netmask;
    l3_protocol _l3;
    ipv4_tcp _tcp;
    ipv4_icmp _icmp;
    ipv4_udp _udp;
    array_map<ip_protocol*, 256> _l4;
    ip_packet_filter * _packet_filter = nullptr;
    struct frag {
        packet header;
        ipv4_packet_merger data;
        clock_type::time_point rx_time;
        uint32_t mem_size = 0;
        bool last_frag_received = false;
        
        
        
    };
    std::unordered_map<ipv4_frag_id, frag, ipv4_frag_id::hash> _frags;
    std::list<ipv4_frag_id> _frags_age;
    static constexpr std::chrono::seconds _frag_timeout{30};
    static constexpr uint32_t _frag_low_thresh{3 * 1024 * 1024};
    static constexpr uint32_t _frag_high_thresh{4 * 1024 * 1024};
    uint32_t _frag_mem{0};
    timer<lowres_clock> _frag_timer;
    circular_buffer<l3_protocol::l3packet> _packetq;
    unsigned _pkt_provider_idx = 0;
    metrics::metric_groups _metrics;
private:
    
    
    
    
    
    
    
    
    
public:
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
};


struct ip_hdr {
    uint8_t ihl : 4;
    uint8_t ver : 4;
    uint8_t dscp : 6;
    uint8_t ecn : 2;
    packed<uint16_t> len;
    packed<uint16_t> id;
    packed<uint16_t> frag;
    enum class frag_bits : uint8_t { mf = 13, df = 14, reserved = 15, offset_shift = 3 };
    uint8_t ttl;
    uint8_t ip_proto;
    packed<uint16_t> csum;
    ipv4_address src_ip;
    ipv4_address dst_ip;
    uint8_t options[0];
     ;
    
    
    
} __attribute__((packed));
template <typename InetTraits>
struct l4connid<InetTraits>::connid_hash : private std::hash<ipaddr>, private std::hash<uint16_t> {
    
};

}
}
using data_type = shared_ptr<const abstract_type>;
class column_set {};
class schema final : public enable_lw_shared_from_this<schema> {
  struct column {
    bytes name;
    data_type type;
  };
public:
  schema(std::optional<utils::UUID> id, std::string_view ks_name,
         std::string_view cf_name, std::vector<column> partition_key,
         std::vector<column> clustering_key,
         std::vector<column> regular_columns,
         std::vector<column> static_columns, data_type regular_column_name_type,
         std::string_view comment = {});
  ~schema();
public:
};
struct blob_storage {
  struct [[gnu::packed]] ref_type {
    blob_storage *ptr;
    ref_type() {}
    ref_type(blob_storage * ptr) : ptr(ptr) {}
    operator blob_storage *() const { return ptr; }
    blob_storage *operator->() const { return ptr; }
    blob_storage &operator*() const { return *ptr; }
  };
} __attribute__((packed));
class table;
using column_family = table;
class clustering_key_prefix;
template <typename T> class wrapping_range {};
template <typename T> class nonwrapping_range {};
GCC6_CONCEPT(template <template <typename> typename T, typename U>
             concept bool Range =
                 std::is_same<T<U>, wrapping_range<U>>::value ||
                 std::is_same<T<U>, nonwrapping_range<U>>::value;)
namespace std {}
namespace dht {
class decorated_key;
}
template <typename EnumType, EnumType... Items> struct super_enum {};
template <typename Enum> class enum_set {};
namespace tracing {
class trace_state_ptr;
}
namespace query {
using column_id_vector = utils::small_vector<column_id, 8>;
using clustering_range = nonwrapping_range<clustering_key_prefix>;
typedef std::vector<clustering_range> clustering_row_ranges;
class specific_ranges {};
constexpr auto max_rows = std::numeric_limits<uint32_t>::max();
class partition_slice {
public:
  enum class option {
    send_clustering_key,
    send_partition_key,
    send_timestamp,
    send_expiry,
    reversed,
    distinct,
    collections_as_maps,
    send_ttl,
    allow_short_read,
    with_digest,
    bypass_cache,
    always_return_static_content,
  };
  using option_set = enum_set<super_enum<
      option, option::send_clustering_key, option::send_partition_key,
      option::send_timestamp, option::send_expiry, option::reversed,
      option::distinct, option::collections_as_maps, option::send_ttl,
      option::allow_short_read, option::with_digest, option::bypass_cache,
      option::always_return_static_content>>;
public:
  partition_slice(
      clustering_row_ranges row_ranges, column_id_vector static_columns,
      column_id_vector regular_columns, option_set options,
      std::unique_ptr<specific_ranges> specific_ranges = nullptr,
      cql_serialization_format = cql_serialization_format::internal(),
      uint32_t partition_row_limit = max_rows);
  partition_slice(clustering_row_ranges ranges, const schema &schema,
                  const column_set &mask, option_set options);
  partition_slice(const partition_slice &);
};
} 
class range_tombstone final {};
namespace db {
using timeout_clock = seastar::lowres_clock;
}
class clustering_row {};
class static_row {};
class partition_start final {
public:
};
class partition_end final {
public:
};
GCC6_CONCEPT(template <typename T, typename ReturnType>
             concept bool MutationFragmentConsumer() {
               return requires(T t, static_row sr, clustering_row cr,
                               range_tombstone rt, partition_start ph,
                               partition_end pe) {
                 { t.consume(std::move(sr)) }
                 ->ReturnType;
                 { t.consume(std::move(cr)) }
                 ->ReturnType;
                 { t.consume(std::move(rt)) }
                 ->ReturnType;
                 { t.consume(std::move(ph)) }
                 ->ReturnType;
                 { t.consume(std::move(pe)) }
                 ->ReturnType;
               };
             })
class mutation_fragment {};
GCC6_CONCEPT(template <typename F> concept bool StreamedMutationTranformer() {
  return requires(F f, mutation_fragment mf, schema_ptr s) {
    { f(std::move(mf)) }
    ->mutation_fragment;
    { f(s) }
    ->schema_ptr;
  };
})
class mutation final {
  mutation() = default;
public:
  const dht::decorated_key &decorated_key() const;
};
using mutation_opt = optimized_optional<mutation>;
class flat_mutation_reader;
future<mutation_opt>
read_mutation_from_flat_mutation_reader(flat_mutation_reader &reader,
                                        db::timeout_clock::time_point timeout);
class locked_cell;
class frozen_mutation;
class table {
public:
  future<std::vector<locked_cell>>
  lock_counter_cells(const mutation &m, db::timeout_clock::time_point timeout);
};
class database {
private:
  future<mutation>
  do_apply_counter_update(column_family &cf, const frozen_mutation &fm,
                          schema_ptr m_schema,
                          db::timeout_clock::time_point timeout,
                          tracing::trace_state_ptr trace_state);
public:
};
namespace tracing {
class trace_state_ptr final {
public:
  trace_state_ptr();
  trace_state_ptr(nullptr_t);
};
} 
template <typename Consumer>
inline future<> consume_partitions(flat_mutation_reader &reader,
                                   Consumer consumer,
                                   db::timeout_clock::time_point timeout) {
  using futurator = futurize<std::result_of_t<Consumer(mutation &&)>>;
  return do_with(
      std::move(consumer), [&reader, timeout](Consumer &c) -> future<> {
        return repeat([&reader, &c, timeout]() {
          return read_mutation_from_flat_mutation_reader(reader, timeout)
              .then([&c](mutation_opt &&mo) -> future<stop_iteration> {
                if (!mo) {
                  return make_ready_future<stop_iteration>(stop_iteration::yes);
                }
                return futurator::apply(c, std::move(*mo));
              });
        });
      });
}
class frozen_mutation final {
public:
  frozen_mutation(const mutation &m);
  mutation unfreeze(schema_ptr s) const;
};
class mutation_source {};
future<mutation_opt> counter_write_query(schema_ptr, const mutation_source &,
                                         const dht::decorated_key &dk,
                                         const query::partition_slice &slice,
                                         tracing::trace_state_ptr trace_ptr);
class locked_cell {};
future<mutation>
database::do_apply_counter_update(column_family &cf, const frozen_mutation &fm,
                                  schema_ptr m_schema,
                                  db::timeout_clock::time_point timeout,
                                  tracing::trace_state_ptr trace_state) {
  auto m = fm.unfreeze(m_schema);
  query::column_id_vector static_columns;
  query::clustering_row_ranges cr_ranges;
  query::column_id_vector regular_columns;
  auto slice = query::partition_slice(
      std::move(cr_ranges), std::move(static_columns),
      std::move(regular_columns), {}, {}, cql_serialization_format::internal(),
      query::max_rows);
  return do_with(
      std::move(slice), std::move(m), std::vector<locked_cell>(),
      [this, &cf, timeout](const query::partition_slice &slice, mutation &m,
                           std::vector<locked_cell> &locks) mutable {
        return cf.lock_counter_cells(m, timeout)
            .then([&, timeout, this](std::vector<locked_cell> lcs) mutable {
              locks = std::move(lcs);
              return counter_write_query(schema_ptr(), mutation_source(),
                                         m.decorated_key(), slice, nullptr)
                  .then([this, &cf, &m, timeout](auto mopt) {
                    return std::move(m);
                  });
            });
      });
}

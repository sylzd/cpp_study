// Copyright 2023 lzd
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SALES_DATA_HPP
#define SALES_DATA_HPP

#include "cpplang.hpp"
#include <string>
#include <msgpack.hpp> // brew install msgpack-cxx

#if MSGPACK_VERSION_MAJOR < 2
#   error "msgpack version is too old"
#endif

namespace cpp_study {

class SalesData final { // final: forbid inheritance 禁止继承
public:
    using this_t = SalesData; // 自己的类型别名 type alias
public:
    using string_t = std::string; // 外部类型别名 type alias
    using string_view_t = const std::string&;
    using uint_t = unsigned int;
    using currency_t = double;

    STATIC_ASSERT(sizeof(uint_t) >= 4); // static_assert 静态断言
    STATIC_ASSERT(sizeof(currency_t) >= 4);
public:
    SalesData(string_view_t id, uint_t s, currency_t r) noexcept : m_id(id), m_sold(s), m_revenue(r) { //显式构造，保证不异常
        assert(s > 0);
        assert(r > 0);
    };

    SalesData(string_view_t id) noexcept : SalesData(id, 0, 0) { //委托构造
        assert(!id.empty());
    }
public:
    SalesData() = default; // default constructor
    ~SalesData() = default; // default destructor

    SalesData(const this_t&) = default; // default copy constructor
    SalesData& operator=(const this_t&) = default; // default copy assignment operator
    SalesData(this_t&&) = default; // 转移构造 default move constructor
    SalesData& operator=(this_t&&) = default; // 复制构造 default move assignment operator
private: // 用private 分离构造与业务逻辑
    string_t m_id = ""; // 成员变量初始化 initialize the member variable
    uint_t m_sold = 0;
    // 这里用double表示货币，由于精度问题，工业上不这样用
    currency_t m_revenue = 0;

public:
    void inc_sold(uint_t s) noexcept {
        m_sold += s;
    }
    void inc_revenue(currency_t r) noexcept {
        m_revenue += r;
    }
public:
    string_view_t id() const noexcept { // 常函数，不抛异常
        return m_id;
    }
    uint_t sold() const noexcept { // 常函数，不抛异常
        return m_sold;
    }
    currency_t revenue() const noexcept { // 常函数，不抛异常
        return m_revenue;
    }
    currency_t avg_price() const noexcept { // 常函数，不抛异常
        return m_sold ? m_revenue / m_sold : 0;
    }

public: // 序列化放在这里一定程度上影响了单一职责，可能会增加该类的复杂度，要权衡考虑
    MSGPACK_DEFINE(m_id, m_sold, m_revenue); // msgpack序列化

    msgpack::sbuffer pack() const {
        msgpack::sbuffer buf;
        msgpack::pack(buf, *this); // this指针指当前对象，自动创建，但一般用成员函数时不需要用this
        return buf;
    }

    SalesData(const msgpack::sbuffer& buf) {
        msgpack::object_handle oh = msgpack::unpack(buf.data(), buf.size());
        msgpack::object obj = oh.get();
        obj.convert(*this);
    }

public:

};

}
#endif // SALES_DATA_HPP
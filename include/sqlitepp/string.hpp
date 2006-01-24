//////////////////////////////////////////////////////////////////////////////
// string.hpp
//
// Copyright (c) 2005 Pavel Medvedev
// Use, modification and distribution is subject to the
// Boost Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef SQLITEPP_STRING_HPP_INCLUDED
#define SQLITEPP_STRING_HPP_INCLUDED

#include <limits>
#include <cstddef>
#include <iosfwd>
#include <vector>

//////////////////////////////////////////////////////////////////////////////

namespace sqlitepp {

//////////////////////////////////////////////////////////////////////////////

namespace metaprogram {

//////////////////////////////////////////////////////////////////////////////

// Meta if
template<bool C, typename T1, typename T2>
struct if_ { typedef T1 result; };
template<typename T1, typename T2>
struct if_<false, T1, T2> { typedef T2 result; };

struct utf16_char
{
	class unknown_utf16_char_type;
	typedef if_<std::numeric_limits<wchar_t>::digits == 16, wchar_t,
		if_<std::numeric_limits<unsigned short>::digits == 16, unsigned short,
		if_<std::numeric_limits<unsigned int>::digits == 16, unsigned int, 
			unknown_utf16_char_type>::result >::result >::result type;
};

//////////////////////////////////////////////////////////////////////////////

} // namespace metaprogram 

//////////////////////////////////////////////////////////////////////////////

typedef char utf8_char;
typedef metaprogram::utf16_char::type utf16_char;

template<typename Char>
class const_string
{
public:
	typedef Char char_type;

	const_string(char_type const* str = 0)
		: buf_( str, str + strlen(str) )
	{
	}

	~const_string()
	{
		clear();
	}

	void clear()
	{
		buffer().swap(buf_);
	}

	bool empty() const // throw()
	{
		return length() == 0;
	}
	
	size_t length() const // throw()
	{
		return buf_.size();
	}
	
	size_t size() const
	{
		return length() * sizeof(char_type);
	}

	char_type const* c_str() const
	{
		return &buf_[0];
	}
	
	friend const_string operator+(const_string const& s1, const_string const& s2)
	{
		return s1 + s2.c_str();
	}
	friend const_string operator+(const_string const& s1, char_type const* s2)
	{
		buffer buf(s1.buf_);
		buf.insert(buf.end(), s2, s2 + strlen(s2));
		const_string result;
		result.buf_.swap(buf);
		return result;
	}

	friend bool operator==(const_string const& s1, const_string const& s2)
	{
		return s1 == s2.c_str();
	}
	friend bool operator==(const_string const& s1, char_type const* s2)
	{
		return s1.equals(s2);
	}

	friend bool operator!=(const_string const& s1, const_string const& s2)
	{
		return !(s1 == s2);
	}
	friend bool operator!=(const_string const& s1, char_type const* s2)
	{
		return !(s1 ==s2);
	}
	
	friend std::ostream& operator<<(std::ostream& os, const_string const& s)
	{
		return os << s.c_str();
	}
private:
	bool equals(char_type const* str) const // throw()
	{
		return strcmp(this->c_str(), str) == 0;
	}
	
	typedef std::vector<char_type> buffer;
	buffer buf_;
};

typedef const_string<utf8_char> utf8_string;
typedef const_string<utf16_char> utf16_string;

#ifdef SQLITEPP_UTF16
	typedef utf16_char char_t;
	typedef utf16_string string_t;
#else
	typedef utf8_char char_t;
	typedef utf8_string string_t;
#endif // SQLITEPP_UTF16

//////////////////////////////////////////////////////////////////////////////

namespace util {

//////////////////////////////////////////////////////////////////////////////

template<typename C, typename T1, typename T2>
struct selector;

template<typename T1, typename T2>
struct selector<utf8_char, T1, T2>
{
	selector(T1 t1, T2) : result(t1) {}
	typedef T1 type;
	type result;
};

template<typename T1, typename T2>
struct selector<utf16_char, T1, T2>
{
	selector(T1, T2 t2) : result(t2) {}
	typedef T2 type;
	type result;
};

template<typename C, typename T1, typename T2>
typename selector<C, T1, T2>::type select(T1 t1, T2 t2)
{
	return selector<C, T1, T2>(t1, t2).result;
}

//////////////////////////////////////////////////////////////////////////////

} // namespace util {

/////////////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
typename util::selector<char_t, T1, T2>::type select(T1 t1, T2 t2)
{
	return util::selector<char_t, T1, T2>(t1, t2).result;
}

} // namespace sqlitepp

//////////////////////////////////////////////////////////////////////////////

#endif // SQLITEPP_STRING_HPP_INCLUDED

//////////////////////////////////////////////////////////////////////////////
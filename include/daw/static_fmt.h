// The MIT License (MIT)
//
// Copyright (c) 2018 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and / or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <functional>

#include <daw/daw_string_view.h>
#include <daw/daw_static_string.h>

namespace daw {
	namespace static_fmt {
		inline namespace v1 {
			namespace {
				namespace fmt_impl {
					inline constexpr ptrdiff_t const dynamic_extent = -1;

					template<typename CharT, size_t N>
					constexpr daw::basic_static_string<CharT, N>
					cstr_to_arry( CharT const ( &str )[N + 1] ) noexcept {
						daw::basic_static_string<CharT, N> result{};
						std::copy( std::begin( str ), std::prev( std::end( str ) ),
						           result.data( ) );
						return result;
					}

					template<typename CharT, size_t N, size_t M>
					constexpr daw::basic_static_string<CharT, N>
					cstr_to_arry( CharT const ( &str1 )[N + 1],
					              CharT const ( &str2 )[M + 1] ) noexcept {
						daw::basic_static_string<CharT, N + M> result{};
						auto pos =
						  std::copy( std::cbegin( str1 ), std::prev( std::cend( str1 ) ),
						             result.data( ) );
						std::copy( std::cbegin( str2 ), std::prev( std::cend( str2 ) ),
						           pos );
						return result;
					}

					template<typename CharT, size_t N, typename Function>
					struct dstr_t {
						static constexpr ptrdiff_t const extent = dynamic_extent;

						daw::basic_static_string<CharT, N> m_prefix;
						Function m_make_string;

						constexpr dstr_t( CharT const ( &prefix )[N + 1], Function &&func )
						  : m_prefix( cstr_to_arry( prefix ) )
						  , m_make_string( std::forward<Function>( func ) ) {}

						std::string to_string( ) const {
							std::string result( m_prefix.data( ), m_prefix.size( ) );
							result += std::invoke( m_make_string );
							return result;
						}
					};

					template<typename CharT, size_t N, typename Function>
					dstr_t( CharT const ( &prefix )[N], Function &&func )
					  ->dstr_t<CharT, N-1, Function>;

					template<typename CharT, size_t N, size_t M>
					struct sstr_t {
						static constexpr ptrdiff_t const extent = N + M;
						daw::basic_static_string<CharT, extent> m_data;

						constexpr sstr_t( CharT const ( &prefix )[N + 1],
						                  CharT const ( &str )[M + 1] ) noexcept
						  : m_data( cstr_to_arry( prefix, str ) ) {}

						constexpr daw::basic_static_string<CharT, extent> const &
						to_string( ) const noexcept {
							return m_data;
						}
					};

					template<typename CharT, size_t N, size_t M>
					sstr_t( CharT const ( &prefix )[N], CharT const ( &str )[M] )
					  ->sstr_t<CharT, N-1, M-1>;

					template<typename CharT, size_t N>
					auto make_string( CharT const ( &prefix )[N + 1],
					                  std::string const &str ) {
						auto fn = [str]( ) { return str; };
						return dstr_t<CharT, N, decltype( fn )>( prefix, std::move( fn ) );
					};

					template<typename CharT, size_t N, size_t M>
					constexpr sstr_t<CharT, N, M>
					make_string( CharT const ( &prefix )[N + 1],
					             CharT const ( &str )[M + 1] ) noexcept {
						return {prefix, str};
					}

					template<daw::string_view fmt_string, typename... Args>
					struct fmt_t {
						// using values_t = std::tuple <

						constexpr fmt_t( Args &&... ) {}
					};
				} // namespace fmt_impl
			}   // namespace

			template<daw::string_view fmt_string, typename... Args>
			constexpr auto fmt( Args &&... args )
			  -> fmt_impl::fmt_t<fmt_string, Args...> {
				return {make_string( std::forward<Args>( args ) )...};
			}
		} // namespace v1
	}   // namespace static_fmt
	using static_fmt::fmt;
} // namespace daw

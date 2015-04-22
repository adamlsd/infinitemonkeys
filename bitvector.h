/*-
 * Copyright (c) 2015   ADAM David Alan Martin
 *	Infinite Monkeys Fun Project, All Rights Reserved.
 *
 * This file is dually licenced, and may be used by the recipient under the
 * terms of either the Boost Software License, or the 3-Clause BSD License.
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * 3-Clause BSD License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef INFINITE_MONKEYS_BITS_BITVECTOR_HEADER
#define INFINITE_MONKEYS_BITS_BITVECTOR_HEADER

#include <algorithm>
#include <iostream>


#include <vector>
#include <climits>

#include "InvariantChecker.h"

namespace InfiniteMonkeys
{
	namespace Bits
	{
		class bitvector;
		std::ostream &operator << ( std::ostream &, const bitvector & );
		std::istream &operator >> ( std::istream &,       bitvector & );

		namespace detail
		{
			template< bool constness > struct const_unsigned_impl;
			template<> struct const_unsigned_impl< true  > { using type= const unsigned; };
			template<> struct const_unsigned_impl< false > { using type=       unsigned; };

			template< bool constness >
			using const_unsigned= typename const_unsigned_impl< constness >::type;
		}

		class bitvector
		{
			private:
				std::size_t actualSize= 0;
				std::vector< unsigned > data;

				friend std::ostream &operator << ( std::ostream &, const bitvector & );
				friend std::istream &operator >> ( std::istream &,       bitvector & );

				using Invariant= Chipotle::InvariantChecker< bitvector >;
				friend Invariant;

				inline bool
				invariant() const
				{
					return this->actualSize <= ( data.size() * sizeof( unsigned ) * CHAR_BIT );
				}

				/*!
				 * @brief Represents a bit's index in an array of words.
				 */
				struct Index
				{
					std::size_t word_idx;
					std::size_t word_mod;

					explicit inline
					Index( const std::size_t &idx )
							: word_idx( idx / ( sizeof( unsigned ) * CHAR_BIT ) ),
							word_mod( idx % ( sizeof( unsigned ) * CHAR_BIT ) )
					{
						assert( word_mod < sizeof( unsigned ) * CHAR_BIT );
						assert( idx == word_idx * sizeof( unsigned ) * CHAR_BIT + word_mod );
					}
				};

				/*!
				 * @brief Represents a boolean value in a fashion as if it were a reference
				 */
				template< bool constness >
				class boolean_proxy
				{
					private:
						using backing_type= detail::const_unsigned< constness >;
						backing_type *const raw_backing;
						const unsigned index;

					public:
						explicit inline
						boolean_proxy( backing_type &raw, const unsigned i_index )
							: raw_backing( &raw ), index( i_index )
						{
							assert( i_index < sizeof( backing_type ) * CHAR_BIT );
						}

						boolean_proxy &
						operator= ( const bool value )
						{
							const unsigned oldRaw= *raw_backing;
							*raw_backing= ( *raw_backing & ~( 1 << index ) ) | ( value  << index );
							assert( ( *raw_backing & ~oldRaw ) == ( value << index ) );
							assert( bool( *raw_backing & ( 1 << index ) ) == value );
							return *this;
						}

						inline operator bool() const
						{
							return *raw_backing & ( 1 << index );
						}

						inline friend bool
						operator == ( const boolean_proxy &lhs, const boolean_proxy &rhs )
						{
							return static_cast< bool >( lhs ) == static_cast< bool >( rhs );
						}

						inline friend bool
						operator != ( const boolean_proxy &lhs, const boolean_proxy &rhs )
						{
							return !( lhs == rhs );
						}
				};

				inline std::size_t
				bytes() const noexcept
				{
					Invariant checker( this );
					return this->data.size() * sizeof( unsigned );
				}

				inline void *
				raw() noexcept
				{
					Invariant checker( this );
					return &this->data[ 0 ];
				}

				inline const void *
				raw() const noexcept
				{
					Invariant checker( this );
					return &this->data[ 0 ];
				}

			public:
				explicit bitvector()= default;

				inline std::size_t
				size() const noexcept
				{
					Invariant checker( this );
					return this->actualSize;
				}

				inline bool
				empty() const noexcept
				{
					Invariant checker( this );
					return !( this->actualSize );
				}

				inline void
				resize( const std::size_t amount )
				{
					Invariant checker( this );
					this->actualSize= amount;
					const std::size_t words= ( this->actualSize + sizeof( unsigned ) * CHAR_BIT - 1 )
							/ ( sizeof( unsigned ) * CHAR_BIT );
					this->data.resize( words );
				}

				inline bool
				operator[]( const std::size_t idx ) const
				{
					Invariant checker( this );
					assert( idx <= this->data.size() * sizeof( unsigned ) * CHAR_BIT );
					assert( idx <= this->actualSize );

					const Index index( idx );

					assert( index.word_idx < this->data.size() );

					const unsigned &word= this->data[ index.word_idx ];
					return bool( boolean_proxy< true >( word, index.word_mod ) );
				}

				inline boolean_proxy< false >
				operator[]( const std::size_t idx )
				{
					Invariant checker( this );
					assert( idx <= this->data.size() * sizeof( unsigned ) * CHAR_BIT );
					assert( idx <= this->actualSize );

					const Index index( idx );

					assert( index.word_idx < this->data.size() );

					unsigned &word= this->data[ index.word_idx ];
					return boolean_proxy< false >( word, index.word_mod );
				}

				inline void
				push_back( const bool value )
				{
					Invariant checker( this );

					if( this->actualSize == this->data.size() * sizeof( unsigned ) * CHAR_BIT )
					{
						this->data.push_back( 0 );
					}

					const std::size_t last_idx= this->actualSize++;
					assert( last_idx == this->actualSize - 1 );
					const bool check= this->operator[]( std::max( (signed) last_idx - 1, 0 ) );
					this->operator[]( last_idx )= value;
					assert( check == this->operator[]( std::max( (signed) last_idx - 1, 0 ) ) );
					assert( (const_cast< const bitvector * >( this )->operator[]( last_idx ) == value ) );
				}
		};

		inline bool
		operator == ( const bitvector &lhs, const bitvector &rhs )
		{
			if( lhs.size() != rhs.size() ) return false;

			for( std::size_t i= 0; i < lhs.size(); ++i )
			{
				if( lhs[ i ] != rhs[ i ] ) return false;
			}
			return true;
		}

		inline bool
		operator != ( const bitvector &lhs, const bitvector &rhs )
		{
			return !( lhs == rhs );
		}

		inline std::ostream &
		operator << ( std::ostream &os, const bitvector &bv )
		{
			const std::size_t sz= bv.size();
			os.write( reinterpret_cast< const char * >( &sz ), sizeof( sz ) );
			os.write( static_cast< const char * >( bv.raw() ), bv.bytes() );

			return os;
		}

		inline std::istream &
		operator >> ( std::istream &is, bitvector &bv )
		{
			std::size_t sz;
			is.read( reinterpret_cast< char * >( &sz ), sizeof( sz ) );

			bv.resize( sz );

			is.read( static_cast< char * >( bv.raw() ), bv.bytes() );
		}
	}
}

#endif	/*** INFINITE_MONKEYS_BITS_BITVECTOR_HEADER ***/

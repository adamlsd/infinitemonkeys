#include <algorithm>
#include <ostream>
#include <istream>
#include <vector>

#include "InvariantChecker.h"


namespace Chipotle
{
	namespace detail
	{
		template< typename T >
		inline T
		read( std::istream &is )
		{
			T rv;
			is >> rv;

			return rv;
		}
	}
		
	namespace text
	{
		void
		binserialize( const std::vector< bool > &data, std::ostream &os )
		{
			os << data.size();

			for( const auto &&x: data )
			{
				os <<  x << " ";
			}
		}


		void
		deserialize( std::vector< bool > &data, std::istream &is )
		{
			const std::size_t sz= detail::read< std::size_t >( is );

			for( std::size_t i= 0; i < sz; ++i )
			{
				data.push_back( detail::read< bool >( is ) );
			}
		}
	}

	namespace Bits
	{
		class o_biterator
		{
			private:
				unsigned idx= 0;
				unsigned value= 0;
				std::ostream *os;

			public:
				explicit inline
				o_biterator( std::ostream &i_os )
					: os( &i_os ) {}

				inline o_biterator &operator *() { return *this; }
				inline o_biterator &operator ++() { return *this; }
				inline o_biterator &operator++ ( int ) { return *this; }

				inline o_biterator &
				operator= ( const bool bit )
				{
					value|= bit << (idx++);

					if( idx == sizeof( value ) * CHAR_BIT )
					{
						os->write( &value, sizeof( value ) );
						idx= value= 0;
					}
				}
		};

		class i_biterator
		{
			private:
				unsigned idx= 0;
				unsigned value= 0;
				std::istream *is;

			public:
				explicit inline
				i_biterator( std::istream &i_is )
					: is( &i_is ) {}

				inline bool
				operator *() const
				{
					return this->value & ( 1 << idx );
				}

				inline o_biterator &
				operator ++()
				{
					if( ++this->idx == sizeof( value ) * CHAR_BIT )
					{
						this->idx= 0;
						this->is.read( &this->value, sizeof( value ) );

						if( this->is.eof() ) this->is= nullptr;
					}

					return *this;
				}

				inline bool
				operator == ( const i_biterator &rhs ) const
				{
					return this->os == rhs.os && this->idx == rhs.idx;
				}

				inline bool
				operator != ( const i_biterator &rhs ) const
				{
					return !( *this == rhs );
				}
		};
	}
}

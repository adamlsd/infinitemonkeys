#ifndef	BITVECTOR_HEADER
#define	BITVECTOR_HEADER

#include <algorithm>
#include <ostream>
#include <istream>
#include <vector>

#include "InvariantChecker.h"
#include "binserialize.h"


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
		
	namespace Bits
	{
		class bitvector
		{
			private:

				std::size_t actualSize= 0;
				std::vector< unsigned > data;

				using Invariant= Chipotle::InvariantChecker< bitvector >;
				friend Invariant;

				inline bool
				invariant() const
				{
					return this->actualSize <= ( data.size() * sizeof( unsigned ) * CHAR_BIT );
				}

			public:
				explicit bitvector()= default;

				inline bool
				operator[]( const std::size_t idx ) const
				{
					Invariant checker( this );
					assert( idx <= this->data.size() * sizeof( unsigned ) * CHAR_BIT );
					assert( idx <= this->actualSize );

					const std::size_t byte_idx = idx / CHAR_BIT;
					const std::size_t word_idx = byte_idx / sizeof( unsigned );
					assert( word_idx < this->data.size() );

					const unsigned word= this->data[ word_idx ];
	
					const std::size_t word_mod = idx % ( sizeof( unsigned ) * CHAR_BIT );
					return word & ( 1 << word_mod );
				}


				inline void
				push_back( const bool value )
				{
					Invariant checker( this );

					if( this->actualSize == this->data->size() * sizeof( unsigned ) * CHAR_BIT )
					{
						this->data.push_back( 0 );
					}

					const std::size_t word_idx= this->actualSize / ( sizeof( unsigned ) * CHAR_BIT );
					const std::size_t word_mod= this->actualSize % ( sizeof( unsigned ) * CHAR_BIT );

					assert( word_idx < this->data.size() );
					this->data[ word_idx ]= value << word_mod;
				}
		};

		inline std::ostream &
		operator << ( std::ostream &os, const bitvector &bv )
		{
			const std::size_t sz= bv.size()
			os.write( &sz, sizeof( sz ) );

			using std::begin; using std::end;
			std::copy( begin( bv ), end( bv ), o_biterator( os ) );

			return os;
		}
	}
}

#endif	/*** BITVECTOR_HEADER ***/

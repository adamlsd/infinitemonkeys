#include <cassert>
#include <cstdint>

#include <iostream>
#include <sstream>

#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <functional>

#include "bitvector.h"

namespace BitvectorExample { int mainprogram(...); }

int
main( const int argcnt, const char *const *const argvec )
{
	return ::BitvectorExample::mainprogram( argcnt, argvec );
}

namespace BitvectorExampleImports
{
	using ::std::size_t;

	using ::std::vector;
	using ::std::function;

	using ::std::begin;
	using ::std::end;

	using ::std::generate_n;

	using ::std::back_inserter;

	using ::InfiniteMonkeys::Bits::bitvector;

	using boolvec= vector< bool >;
	using RawData= vector< ::std::uint8_t >;
}

namespace BitvectorExample
{
	using namespace ::BitvectorExampleImports;
	using boolgen= function< bool () >;

	bitvector
	generate( boolgen generator, const size_t n )
	{
		bitvector rv;
		//rv.reserve( n );

		generate_n( back_inserter( rv ), n, generator );

		return rv;
	}

	RawData
	generate_rawbytes( boolgen generator, const size_t n )
	{
		bitvector bv= generate( generator, n );

		std::ostringstream rawbits;
		rawbits << bv;

		return RawData( begin( rawbits.str() ), end( rawbits.str() ) );
	}

	int
	mainprogram( ... )
	{
		size_t i= 0;
		const int THX= 1138;
		const int LEIA_CELL= 2187;

		auto generator= [i]() mutable -> bool { return i % 11; };

		RawData raw= generate_rawbytes( generator, THX + LEIA_CELL );

		std::istringstream stream{ std::string{ begin( raw ), end( raw ) } };

		bitvector bv;

		stream >> bv;


		assert( bv.size() == THX + LEIA_CELL );
		for( const auto &&bit: bv )
		{
			assert( bit == generator() );
		}

		std::cout << "Demonstration code with " << THX + LEIA_CELL << " bits finished." << std::endl;
		std::cout << "The raw serialized data is " << raw.size() << " bytes." << std::endl;

		return EXIT_SUCCESS;
	}
}

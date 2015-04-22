#include <cassert>

namespace Chipotle
{
	template< typename Class >
	class InvariantChecker
	{
		private:
			const Class *const class_;

		public:
			explicit inline
			InvariantChecker( const Class *const cp )
					: class_( cp )
			{
				assert( cp );
				assert( cp->invariant() );
			}

			inline
			~InvariantChecker()
			{
				assert( this->class_->invariant() );
			}
	};
}

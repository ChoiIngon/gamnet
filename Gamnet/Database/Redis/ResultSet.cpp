#include "ResultSet.h"

namespace Gamnet { namespace Database {	namespace Redis {
	ResultSet::ResultSet() : impl_(std::shared_ptr<ResultSetImpl>(new ResultSetImpl()))
	{
	}

	ResultSet::iterator ResultSet::begin() {
		return impl_->begin();
	}

	ResultSet::iterator ResultSet::end() const {
		return impl_->end();
	}

	const Variant& ResultSet::operator[] (unsigned int index)
	{
		return impl_->at(index);
	}

	const std::string& ResultSet::error() const
	{
		return impl_->error;
	}
} } }
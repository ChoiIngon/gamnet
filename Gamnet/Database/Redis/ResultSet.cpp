#include "ResultSet.h"
#include "../../Library/Exception.h"
#include "../../Log/Log.h"

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
		if (index >= impl_->size()) // no neet to compare with 0. size() returns unsigned int
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidArrayRangeError, "invalid index(index:", index, ")");
		}
		return impl_->at(index);
	}

	const std::string& ResultSet::error() const
	{
		return impl_->error;
	}

	std::shared_ptr<ResultSetImpl> ResultSet::operator -> ()
	{
		return impl_;
	}
} } }

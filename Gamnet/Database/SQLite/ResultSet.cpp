#include "ResultSet.h"
#include "../../Library/Exception.h"

namespace Gamnet { namespace Database { namespace SQLite {

	ResultSetImpl::ResultSetImpl() : rowCount_(0)
	{
	}

	ResultSetImpl::~ResultSetImpl()
	{
	}

	ResultSet::ResultSet() : impl_(nullptr)
	{
	}

	ResultSet::~ResultSet()
	{
	}

	unsigned int ResultSet::GetRowCount() const
	{
		if (nullptr == impl_)
		{
			return 0;
		}

		return impl_->rowCount_;
	}

	ResultSet::iterator ResultSet::begin()
	{
		return iterator(impl_);
	}

	ResultSet::iterator ResultSet::end() const
	{
		iterator itr(impl_);
		itr.itr_ = impl_->vecRows_.end();
		return itr;
	}

	ResultSet::iterator::iterator() : impl_(nullptr)
	{
	}

	ResultSet::iterator::iterator(const std::shared_ptr<ResultSetImpl>& impl) : impl_(impl)
	{
		if(nullptr != impl_)
		{
			itr_ = impl_->vecRows_.begin();
		}
	}

	ResultSet::iterator& ResultSet::iterator::operator * ()
	{
		return *this;
	}

	ResultSet::iterator& ResultSet::iterator::operator ++ ()
	{
		++itr_;
		return *this;
	}
	ResultSet::iterator& ResultSet::iterator::operator ++ (int)
	{
		itr_++;
		return *this;
	}

	ResultSet::iterator* ResultSet::iterator::operator -> ()
	{
		return this;
	}

	bool ResultSet::iterator::operator != (const ResultSet::iterator& itr) const
	{
		if (itr_ != itr.itr_)
		{
			return true;
		}

		return false;
	}

	bool ResultSet::iterator::operator == (const ResultSet::iterator& itr) const
	{
		if (itr_ != itr.itr_)
		{
			return false;
		}

		return true;
	}

	ResultSet::iterator ResultSet::operator [] (unsigned int index)
	{
		iterator itr(impl_);
		itr.itr_ = impl_->vecRows_.begin() + index;
		return itr;
	}

	Variant ResultSet::iterator::operator [] (const std::string& column_name) const
	{
		auto itr = impl_->mapColumnName_.find(column_name);
		if (impl_->mapColumnName_.end() == itr)
		{
			throw GAMNET_EXCEPTION(ErrorCode::InvalidKeyError, "Unknown column '", column_name, "' in 'field list'");
		}
		
		const std::vector<std::string>& row = *itr_;
		return row[itr->second];
	}
}}}
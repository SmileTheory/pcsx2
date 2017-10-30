#pragma once
#include <exception>
#include <algorithm>

#include <boost/utility.hpp>

namespace shoryu
{
	class archive : boost::noncopyable
	{
	public:
		inline size_t pos()
		{
			return next_ - first_;
		}
		inline size_t length()
		{
			return last_ - first_; 
		}
	protected:
		inline void setbuffer(char* first, char* last)
		{
			first_ = first;
			next_ = first;
			last_ = last;
		}
		inline void overflow_check(size_t size)
		{
			if( (size_t)(last_ - next_) < size)
				throw std::ios::failure("buffer overflow");
		}
		char* first_;
		char* next_;
		char* last_;
	};

	class oarchive : public archive
	{
	public:
		oarchive(char* first, char* last) 
		{
			setbuffer(first, last);
		}
		template<typename T>
		inline oarchive& operator<<(const T& d)
		{
			write((char*)&d, sizeof(T));
			return *this;
		}
		inline void write(char* d, size_t length)
		{
			overflow_check(length);
#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(push) 
#pragma warning(disable:4996) 
#endif 

			std::copy(d, d+length, next_);

#if defined(_MSC_VER) && _MSC_VER >= 1400 
#pragma warning(pop) 
#endif 
			next_+=length;
		}
	};

	class iarchive : public archive
	{
	public:
		iarchive(const char* first, const char* last)
		{
			setbuffer((char*)first, (char*)last);
		}
		template<typename T>
		inline iarchive& operator>>(const T& d)
		{
			read((char*)&d, sizeof(T));
			return *this;
		}
		inline void read(char* d, size_t length)
		{
			overflow_check(length);
			std::copy(next_, next_+length, d);
			next_+=length;
		}
	};
}

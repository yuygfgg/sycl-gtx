#pragma once

#include <CL/cl.h>

#if _MSC_VER <= 1800
#define MSVC_LOW 1
#endif

#if MSVC_LOW
#define SYCL_SWAP(member) swap(first.member, second.member)
#define SYCL_MOVE_INIT(member) member(std::move(move.member))
#define SYCL_THREAD_LOCAL __declspec(thread)
#else
#define SYCL_THREAD_LOCAL thread_local
#endif

// 3.2 C++ Standard library classes required for the interface

#ifndef CL_SYCL_NO_STD_VECTOR
#include <vector>
#endif
#ifndef CL_SYCL_NO_STD_STRING
#include <string>
#endif
#ifndef CL_SYCL_NO_STD_FUNCTION
#include <functional>
#endif
#ifndef CL_SYCL_NO_STD_MUTEX
#include <mutex>
#endif
#ifndef CL_SYCL_NO_UNIQUE_PTR
#include <memory>
#endif
#ifndef CL_SYCL_NO_SHARED_PTR
#include <memory>
#endif
#ifndef CL_SYCL_NO_WEAK_PTR
#include <memory>
#endif

namespace cl {
namespace sycl {

#ifndef CL_SYCL_NO_STD_VECTOR
	template<class T, class Alloc = ::std::allocator<T>>
	using vector_class = ::std::vector<T, Alloc>;
#endif

#ifndef CL_SYCL_NO_STD_STRING
	using string_class = ::std::string;
#endif

#ifndef CL_SYCL_NO_STD_FUNCTION
#if MSVC_LOW
	template <class T>
	class function_class : public ::std::function<T> {
	private:
		using Base = ::std::function<T>;
	public:
		function_class() {}
		function_class(nullptr_t fn)
			: Base(fn) {}
		template<class Fn>
		function_class(Fn fn)
			: Base(fn) {}
		function_class(const Base& x)
			: Base(x) {}
		function_class(Base&& x)
			: Base(std::move(x)) {}
		function_class(const function_class&) = default;
		function_class(function_class&& move)
			: Base((Base&&)std::move(move)) {}
	};
#else
	template<class T>
	using function_class = ::std::function<T>;
#endif
#endif

#ifndef CL_SYCL_NO_STD_MUTEX
	using mutex_class = ::std::mutex;
#endif
#ifndef CL_SYCL_NO_UNIQUE_PTR
	template <class T, class D = ::std::default_delete<T>>
	using unique_ptr_class = ::std::unique_ptr<T, D>;
#endif
#ifndef CL_SYCL_NO_SHARED_PTR
	template <class T>
	using shared_ptr_class = ::std::shared_ptr<T>;
#endif
#ifndef CL_SYCL_NO_WEAK_PTR
	template <class T>
	using weak_ptr_class = ::std::weak_ptr<T>;
#endif


namespace detail {

template<class To, class From>
vector_class<To> transform_vector(vector_class<From> array) {
	return vector_class<To>(array.data(), array.data() + array.size());
}

template<cl_uint extension_macro, class T>
bool has_extension(T* sycl_class, const string_class extension_name) {
	// TODO: Maybe add caching
	auto extensions = sycl_class->get_info<extension_macro>();
	string_class ext_str(extensions);
	return ext_str.find(extension_name) != string_class::npos;
}

template <class T>
using shared_unique = shared_ptr_class<unique_ptr_class<T>>;

template <typename DataType>
static string_class type_string();

#define SYCL_GET_TYPE_STRING(type)			\
template <>									\
static string_class type_string<type>() {	\
	return #type;						\
}

#define SYCL_GET_TYPE_STRING_UNSIGNED(type)			\
template <>											\
static string_class type_string<unsigned type>() {	\
	return "u"#type;								\
}

SYCL_GET_TYPE_STRING(bool)
SYCL_GET_TYPE_STRING(int)
SYCL_GET_TYPE_STRING(char)
SYCL_GET_TYPE_STRING(short)
SYCL_GET_TYPE_STRING(long)
SYCL_GET_TYPE_STRING(float)
SYCL_GET_TYPE_STRING(double)

SYCL_GET_TYPE_STRING_UNSIGNED(int)
SYCL_GET_TYPE_STRING_UNSIGNED(char)
SYCL_GET_TYPE_STRING_UNSIGNED(short)
SYCL_GET_TYPE_STRING_UNSIGNED(long)

#undef SYCL_GET_TYPE_STRING
#undef SYCL_GET_TYPE_STRING_UNSIGNED

} // namespace detail

} // namespace sycl
} // namespace cl

#pragma once

// 3.3.5 Queue class

#include "context.h"
#include "device.h"
#include "error_handler.h"
#include "event.h"
#include "info.h"
#include "param_traits.h"
#include "refc.h"
#include "../common.h"
#include "../debug.h"

namespace cl {
namespace sycl {

// TODO
class handler_event {
public:
	event get_kernel() const;
	event get_complete() const;
	event get_end() const;
};

// Encapsulation of an OpenCL cl_command_queue
class queue {
private:
	device dev;
	context ctx;
	detail::refc<cl_command_queue, clRetainCommandQueue, clReleaseCommandQueue> command_q;
	exception_list ex_list;

	void display_device_info() const;
	cl_command_queue create_queue(info::queue_profiling properties = 0);

public:
	// Creates a queue for a device it chooses according to the heuristics of the default selector.
	// The OpenCL context object is created implicitly.
	explicit queue(const async_handler& asyncHandler = detail::default_async_handler);

	queue(const device_selector& deviceSelector, const async_handler& asyncHandler = detail::default_async_handler);

	queue(const context& syclContext, const device_selector& deviceSelector, const async_handler& asyncHandler = detail::default_async_handler);

	queue(const context& syclContext, const device& syclDevice, const async_handler& asyncHandler = detail::default_async_handler);

	// Chooses a device based on the provided device selector in the given context.
	queue(const context& syclContext, const device& syclDevice, info::queue_profiling profilingFlag, const async_handler& asyncHandler = detail::default_async_handler);

	// Creates a queue for the provided device.
	queue(const device& syclDevice, const async_handler& asyncHandler = detail::default_async_handler);

	// Creates a SYCL queue from an OpenCL queue.
	// At construction it does a retain on the queue memory object.
	queue(cl_command_queue clQueue, const async_handler& asyncHandler = detail::default_async_handler);

	~queue();

	// Copy and move semantics
	queue(const queue&) = default;
#if MSVC_LOW
	queue(queue&& move)
		: SYCL_MOVE_INIT(command_q), SYCL_MOVE_INIT(ctx), SYCL_MOVE_INIT(dev) {}
	friend void swap(queue& first, queue& second) {
		using std::swap;
		SYCL_SWAP(command_q);
		SYCL_SWAP(ctx);
		SYCL_SWAP(dev);
	}
#else
	queue(queue&&) = default;
#endif

	bool is_host();

	// TODO: Returns the underlying OpenCL command queue after doing a retain.
	// Afterwards it needs to be manually released.
	cl_command_queue get();

	// Returns the SYCL context the queue is using.
	context get_context() const;

	// Returns the SYCL device the queue is associated with.
	device get_device() const;

	template <info::queue param>
	typename param_traits<info::queue, param>::type get_info() const {
		return detail::array_traits<
			param_traits_t<info::queue, param>,
			info::queue,
			param,
			1
		>().get(command_q.get());
	}

	// Checks to see if any asynchronous errors have been produced by the queue
	// and if so reports them by passing them to the async_handler provided on construction.
	// If no async_handler was provided then asynchronous exceptions will be lost.
	void throw_asynchronous();

	// Performs a blocking wait for the completion all enqueued tasks in the queue.
	// Synchronous errors will be reported via an exception.
	void wait();

	// Performs a blocking wait for the completion of all enqueued tasks in the queue.
	void wait_and_throw();

	// TODO
	template <typename T>
	handler_event submit(T cgf) {
		command_group(*this, cgf);
		return handler_event();
	}

	// TODO
	template <typename T>
	handler_event submit(T cgf, queue &secondaryQueue);
};

} // namespace sycl
} // namespace cl

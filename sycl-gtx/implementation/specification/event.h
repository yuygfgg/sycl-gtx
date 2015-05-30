#pragma once

// 3.3.6 Event class for OpenCL interoperability

#include "error_handler.h"
#include "info.h"
#include "refc.h"
#include "../common.h"
#include "../debug.h"

namespace cl {
namespace sycl {

class event {
private:
	detail::refc<cl_event, clRetainEvent, clReleaseEvent> evnt;

public:
	event() = default;
	explicit event(cl_event clEvent);

	cl_event get();

	// Return the list of events that this event waits for in the dependence graph.
	vector_class<event> get_wait_list();

	// Wait for the event and the command associated with it to complete.
	void wait();

	// Synchronously wait on a list of events.
	static void wait(const vector_class<event>& event_list);

	void wait_and_throw();
	static void wait_and_throw(const vector_class<event>& event_list);

	//template <info::event param>
	//typename param_traits<info::event, param>::type get_info() const;

	//template <info::event_profiling param>
	//typename param_traits<info::event_profiling, param>::type get_profiling_info() const;
};

} // namespace sycl
} // namespace cl

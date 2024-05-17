#ifndef SHARED_MACRO_HPP
#define SHARED_MACRO_HPP

#include <iostream>

#ifndef __IO
#define __IO volatile
#endif

inline void rgAssertMsgCalled(unsigned long ulLine, char const* const pcFileName, char const* const message) {
	std::cerr << "Assertion failed: " << message << ", file " << pcFileName << ", line " << ulLine << std::endl; \
}

#define RG_ASSERT_MSG(x, msg) if((x) == 0) rgAssertMsgCalled(__LINE__, __FILE__, msg)

#endif
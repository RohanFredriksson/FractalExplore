
#ifndef UTIL_HPP
#define UTIL_HPP

#define UNIQUE_VARIABLE_NAME() CONCATENATE(V, __COUNTER__)
#define CONCATENATE_IMPL(x, y) x##y
#define CONCATENATE(x, y) CONCATENATE_IMPL(x, y)

#endif
#pragma once
#ifdef AUXILIA_BUILD_MODULE
#define EXPORT_AUXILIA export
#else
#define EXPORT_AUXILIA
#endif
#include "./details/config.hpp"
#include "./details/id.hpp"
#include "./details/memory.hpp"
#include "./details/Monostate.hpp"
#include "./details/Property.hpp"
#include "./details/random.hpp"
#include "./details/Status.hpp"
#include "./details/Variant.hpp"
#include "./details/file_reader.hpp"
#include "./details/format.hpp"
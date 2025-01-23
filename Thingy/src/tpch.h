#pragma once

#ifdef T_PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <mutex>
#include <thread>
#include <limits>
#include <stdexcept>
#include <fstream>
#include <chrono>

#include <cstdint>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <condition_variable>
#include <optional>
#include <set>
#include <map>

//Windows 
#ifdef T_PLATFORM_WINDOWS
#include <Windows.h>
#endif

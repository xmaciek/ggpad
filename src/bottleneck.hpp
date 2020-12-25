// GGPAD Copyright 2020 Maciej Latocha ( latocha.maciek@gmail.com )
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include "nocopy.hpp"

class Bottleneck : nocopy {
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::unique_lock<std::mutex> m_ul;

public:
    Bottleneck();
    void notify();
    void wait_for( const std::chrono::high_resolution_clock::duration& );
};

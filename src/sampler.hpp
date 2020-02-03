// GGPAD Copyright 2020 Maciej Latocha
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

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <numeric>

template <std::size_t TCount>
class Sampler {
public:
    using this_type = Sampler<TCount>;
    using clock_type = std::chrono::high_resolution_clock;
    using time_point = clock_type::time_point;
    using duration = clock_type::duration;
    static constexpr auto& now = clock_type::now;

private:
    static_assert( TCount > 1, "Capacity of cycling buffer is too small, expected > 1" );
    std::array<clock_type::duration, TCount> m_samples = {};

public:
    ~Sampler() noexcept = default;
    Sampler() noexcept = default;

    clock_type::duration average() const
    {
        return std::accumulate(
            m_samples.cbegin()
            , m_samples.cend()
            , clock_type::duration( 0 )
        ) / m_samples.size();
    }

    this_type& operator += ( const clock_type::duration& d )
    {
        std::rotate( m_samples.begin(), m_samples.begin() + 1, m_samples.end() );
        m_samples[ 0 ] = d;
        return *this;
    }
};

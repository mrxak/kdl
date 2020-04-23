// Copyright (c) 2019-2020 Tom Hancocks
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#if !defined(KDL_PNG_HPP)
#define KDL_PNG_HPP

#include <vector>
#include "libGraphite/quickdraw/internal/surface.hpp"
#include "libGraphite/data/data.hpp"

namespace kdl { namespace media { namespace image {

    /**
     * The `kdl::media::image::png` class allows reading/writing and working with PNG
     * images.
     */
    class png
    {
    private:
        std::shared_ptr<graphite::qd::surface> m_surface;

        auto decode(graphite::data::reader& reader) -> bool;

    public:
        png(const std::string path);
        png(std::shared_ptr<std::vector<char>> data);

        auto surface() -> std::weak_ptr<graphite::qd::surface>;
    };

}}};

#endif //KDL_PNG_HPP
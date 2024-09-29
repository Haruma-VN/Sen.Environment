#pragma once

#include "kernel/definition/library.hpp"

namespace Sen::Kernel::Definition
{
    namespace MaxRectsAlgorithm
    {
        inline static auto constexpr EDGE_MAX_VALUE = 4096;

        inline static auto constexpr EDGE_MIN_VALUE = 128;

        enum class PACKING_LOGIC : uint8_t
        {
            MAX_AREA,
            MAX_EDGE
        };

        struct Option
        {
            bool smart = true;
            bool pot = true;
            bool square = false;
            int border = 0;
            int padding = 0;
            PACKING_LOGIC logic = PACKING_LOGIC::MAX_AREA;
        };

        struct Rectangle
        {
        public:
            bool mutable rect_used = false;
            int mutable width = 0;
            int mutable height = 0;
            int mutable x = 0;
            int mutable y = 0;
            std::string mutable source;

            Rectangle() = default;

            Rectangle(
                int width,
                int height,
                int x,
                int y, 
                const std::string &source) : width(width), height(height), x(x), y(y), source(source)
            {
                rect_used = true;
                return;
            }

            Rectangle(
                int width,
                int height,
                const std::string &source) : width(width), height(height), source(source)
            {
                rect_used = true;
                return;
            }


            ~Rectangle() = default;

            /**
             * Test if two given rectangle collide each other
             *
             * @static
             * @param {Rectangle} first
             * @param {Rectangle} second
             * @returns
             * @memberof Rectangle
             */
            inline auto static Collide(Rectangle &first, Rectangle &second) -> bool
            {
                return first.collide(second);
            }

            /**
             * Test if the first rectangle contains the second one
             *
             * @static
             * @param {Rectangle} first
             * @param {Rectangle} second
             * @returns
             * @memberof Rectangle
             */
            inline auto static Contain(Rectangle &first, Rectangle &second) -> bool
            {
                return first.contain(second);
            }

            /**
             * Get the area (w * h) of the rectangle
             *
             * @returns {number}
             * @memberof Rectangle
             */
            inline auto area() const -> int
            {
                return width * height;
            }

            /**
             * Test if the given rectangle collide with this rectangle.
             *
             * @param {Rectangle} rect
             * @returns {boolean}
             * @memberof Rectangle
             */

            inline auto collide(const Rectangle &rect) const -> bool
            {
                return rect.x < x + width && rect.x + rect.width > x && rect.y < y + height && rect.y + rect.height > y;
            }

            /**
             * Test if this rectangle contains the given rectangle.
             *
             * @param {Rectangle} rect
             * @returns {boolean}
             * @memberof Rectangle
             */
            inline auto contain(const Rectangle &rect) -> bool
            {
                return rect.x >= x && rect.y >= y && rect.x + rect.width <= x + width && rect.y + rect.height <= y + height;
            }
        };

        struct MaxRectsBin
        {
        public:
            int mutable width;
            int mutable height;
            int mutable padding;
            int mutable maxWidth = EDGE_MAX_VALUE;
            int mutable maxHeight = EDGE_MAX_VALUE;
            std::vector<Rectangle> mutable freeRects;
            std::vector<Rectangle> mutable rects;
            Option mutable options;
            int mutable border;
            bool mutable verticalExpand = false;
            Rectangle mutable stage;

            MaxRectsBin() = default;

            MaxRectsBin(
                int _maxWidth,
                int _maxHeight,
                Option &_options)
            {
                freeRects.clear();
                rects.clear();
                options = std::move(_options);
                maxWidth = _maxWidth;
                maxHeight = _maxHeight;
                width = options.smart ? 1 : maxWidth;
                height = options.smart ? 1 : maxHeight;
                border = options.border;
                padding = options.padding;
                freeRects.emplace_back(Rectangle(maxWidth + padding - border * 2, maxHeight + padding - border * 2, border, border, ""));
                stage = Rectangle(width, height, "");
                return;
            };

            ~MaxRectsBin() = default;

            inline auto add(const Rectangle &rect) const -> bool
            {
                auto result = Rectangle{};
                place(rect, result);
                if (result.rect_used)
                {
                    rects.emplace_back(result);
                }
                return result.rect_used;
            }

        protected:
            inline auto place(const Rectangle &rect, Rectangle &placeNode) const -> void
            {
                auto node = Rectangle{};
                findNode(rect.width + padding, rect.height + padding, node);
                if (node.rect_used)
                {
                    updateBinSize(node);
                    auto numRectToProcess = freeRects.size();
                    auto i = 0;
                    while (i < numRectToProcess)
                    {
                        if (splitNode(freeRects.at(i), node))
                        {
                            freeRects.erase(freeRects.begin() + i);
                            numRectToProcess--;
                            i--;
                        }
                        i++;
                    }
                    pruneFreeList();
                    verticalExpand = width > height ? true : false;
                    rect.x = node.x;
                    rect.y = node.y;
                    placeNode = rect;
                    return;
                }
                else if (!verticalExpand)
                {
                    if (updateBinSize(Rectangle(rect.width + padding, rect.height + padding, width + padding - border, border, rect.source)) ||
                        updateBinSize(Rectangle(rect.width + padding, rect.height + padding, border, height + padding - border, rect.source)))
                    {
                        place(rect, placeNode);
                        return;
                    }
                }
                else
                {
                    if (updateBinSize(Rectangle(rect.width + padding, rect.height + padding, border, height + padding - border, rect.source)) ||
                        updateBinSize(Rectangle(rect.width + padding, rect.height + padding, width + padding - border, border, rect.source)))
                    {
                        place(rect, placeNode);
                        return;
                    }
                }
                return;
            }

            
            inline auto findNode(int width, int height, Rectangle& bestNode) const -> void
            {
                auto score = 1.7976931348623157e+308;
                auto areaFit = 0;
                for (auto &r : freeRects)
                {
                    if (r.width >= width && r.height >= height)
                    {
                        areaFit = options.logic == PACKING_LOGIC::MAX_AREA ? r.width * r.height - width * height : (std::min)(r.width - width, r.height - height);
                        if (areaFit < score)
                        {
                            bestNode = Rectangle(width, height, r.x, r.y, r.source);
                            score = areaFit;
                        }
                    }
                }
                return;
            }

            inline auto splitNode(const Rectangle &freeRect, const Rectangle &usedNode) const -> bool
            {
                // Test if usedNode intersect with freeRect
                if (!freeRect.collide(usedNode))
                {
                    return false;
                }
                // Do vertical split
                if (usedNode.x < freeRect.x + freeRect.width && usedNode.x + usedNode.width > freeRect.x)
                {
                    // New node at the top side of the used node
                    if (usedNode.y > freeRect.y && usedNode.y < freeRect.y + freeRect.height)
                    {
                        auto newNode = Rectangle(freeRect.width, usedNode.y - freeRect.y, freeRect.x, freeRect.y, freeRect.source);
                        freeRects.emplace_back(newNode);
                    }
                    // New node at the bottom side of the used node
                    if (usedNode.y + usedNode.height < freeRect.y + freeRect.height)
                    {
                        auto newNode = Rectangle(freeRect.width, freeRect.y + freeRect.height - (usedNode.y + usedNode.height), freeRect.x, usedNode.y + usedNode.height, freeRect.source);
                        freeRects.emplace_back(newNode);
                    }
                }

                // Do Horizontal split
                if (usedNode.y < freeRect.y + freeRect.height && usedNode.y + usedNode.height > freeRect.y)
                {
                    // New node at the top side of the used node
                    if (usedNode.x > freeRect.x && usedNode.x < freeRect.x + freeRect.width)
                    {
                        auto newNode = Rectangle(usedNode.x - freeRect.x, freeRect.height, freeRect.x, freeRect.y, freeRect.source);
                        freeRects.emplace_back(newNode);
                    }
                    // New node at the bottom side of the used node
                    if (usedNode.x + usedNode.width < freeRect.x + freeRect.width)
                    {
                        auto newNode = Rectangle(freeRect.x + freeRect.width - (usedNode.x + usedNode.width), freeRect.height, usedNode.x + usedNode.width, freeRect.y, freeRect.source);
                        freeRects.emplace_back(newNode);
                    }
                }

                return true;
            }

            inline auto pruneFreeList() const -> void
            {
                auto i = 0;
                auto j = 0;
                auto len = freeRects.size();
                while (i < len)
                {
                    j = i + 1;
                    auto tmpRect1 = freeRects.at(i);
                    while (j < len)
                    {
                        auto tmpRect2 = freeRects.at(j);
                        if (tmpRect2.contain(tmpRect1))
                        {
                            freeRects.erase(freeRects.begin() + i);
                            i--;
                            len--;
                            break;
                        }
                        if (tmpRect1.contain(tmpRect2))
                        {
                            freeRects.erase(freeRects.begin() + j);
                            j--;
                            len--;
                        }
                        j++;
                    }
                    i++;
                }
                return;
            }

            inline auto updateBinSize(const Rectangle &node) const -> bool
            {
                if (!options.smart)
                {
                    return false;
                }
                if (stage.contain(node))
                {
                    return false;
                }
                auto tmpWidth = (std::max)(width, node.x + node.width - padding + border);
                auto tmpHeight = (std::max)(height, node.y + node.height - padding + border);
                if (options.pot)
                {
                    auto log2e = 1.4426950408889634;
                    tmpWidth = std::pow(2, std::ceil(std::log(tmpWidth) * log2e));
                    tmpHeight = std::pow(2, std::ceil(std::log(tmpHeight) * log2e));
                }
                if (options.square)
                {
                    tmpWidth = tmpHeight = (std::max)(tmpWidth, tmpHeight);
                }
                if (tmpWidth > maxWidth + padding || tmpHeight > maxHeight + padding)
                {
                    return false;
                }
                expandFreeRects(tmpWidth + padding, tmpHeight + padding);
                width = stage.width = tmpWidth;
                height = stage.height = tmpHeight;
                return true;
            }

            inline auto expandFreeRects(int r_width, int r_height) const -> void
            {
                for (auto &freeRect : freeRects)
                {
                    if (freeRect.x + freeRect.width >= (std::min)(width + padding - border, r_width))
                    {
                        freeRect.width = r_width - freeRect.x - border;
                    }
                    if (freeRect.y + freeRect.height >= (std::min)(height + padding - border, r_height))
                    {
                        freeRect.height = r_height - freeRect.y - border;
                    }
                }
                freeRects.emplace_back(Rectangle(r_width - width - padding, r_height - border * 2, width + padding - border, border, ""));
                freeRects.emplace_back(Rectangle(r_width - border * 2, r_height - height - padding, border, height + padding - border, ""));
                for (const auto &i : Range(freeRects.size()))
                {
                    if (i >= freeRects.size()) {
                        break;
                    }
                    auto &freeRect = freeRects.at(i);
                    if ((freeRect.width <= 0 || freeRect.height <= 0 || freeRect.x < border || freeRect.y < border))
                    {
                        freeRects.erase(freeRects.begin() + i);
                    }
                }
                pruneFreeList();
                return;
            }
        };

        class MaxRectsPacker
        {
        public:
            int mutable width = EDGE_MAX_VALUE;
            int mutable height = EDGE_MAX_VALUE;
            Option mutable options;
            std::vector<MaxRectsBin> mutable bins;

            MaxRectsPacker() = default;

            MaxRectsPacker(
                int width,
                int height) : width(width), height(height)
            {
            }

            MaxRectsPacker(
                int width,
                int height,
                Option &options) : width(width), height(height), options(options)
            {
            }

            ~MaxRectsPacker() = default;

        private:
            /**
             * Sort the given rects based on longest edge or surface area.
             *
             * If rects have the same sort value, will sort by second key `hash` if presented.
             *
             * @private
             * @param {Rectangle[]} copy rects
             * @param {PACKING_LOGIC} [logic=PACKING_LOGIC.MAX_EDGE] sorting logic, "area" or "edge"
             * @returns
             * @memberof MaxRectsPacker
             */
            inline auto sort(std::vector<Rectangle> &rects, PACKING_LOGIC logic) const -> void
            {

                if (logic == PACKING_LOGIC::MAX_EDGE)
                {
                    std::sort(rects.begin(), rects.end(), [](const Rectangle &a, const Rectangle &b)
                              { return (std::max)(b.width, b.height) < (std::max)(a.width, a.height); });
                }
                else
                {
                    std::sort(rects.begin(), rects.end(), [](const Rectangle &a, const Rectangle &b)
                              { return b.width * b.height < a.width * a.height; });
                }
                return;
            }

        public:
            /**
             * Add a bin/rectangle object extends IRectangle to packer
             * @template T Generic type extends IRectangle interface
             * @param {Rectangle} rect the rect object add to the packer bin
             * @memberof MaxRectsPacker
             */
            inline auto add(const Rectangle &rect) const -> bool
            {
                if (rect.width > width || rect.height > height)
                {
                    throw Exception("rect size greater than packer size", std::source_location::current(), "add"); 
                }
                else
                {
                    for (auto &bin : bins)
                    {
                        if (bin.add(rect)) {
                            return true;
                        }
                    }
                    auto bin = MaxRectsBin(width, height, options);
                    bin.add(rect);
                    bins.emplace_back(bin);
                }
                return rect.rect_used;
            }

            /**
             * Add an Array of bins/rectangles to the packer.
             *
             * note: object has `hash` property will have more stable packing result
             *
             * @param {Rectangle[]} rects Array of bin/rectangles
             * @memberof MaxRectsPacker
             */
            inline auto addArray(std::vector<Rectangle> &rects) const -> void
            {
                sort(rects, options.logic);
                for (const auto &rect : rects)
                {
                    add(rect);
                }
                return;
            }

            /**
             * Reset entire packer to initial states, keep settings
             *
             * @memberof MaxRectsPacker
             */
            inline auto reset() -> void
            {
                bins.clear();
                return;
            }
        };
    }
}
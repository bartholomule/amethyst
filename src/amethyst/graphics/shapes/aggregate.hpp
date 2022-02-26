#pragma once

#include "amethyst/graphics/shapes/shape.hpp"
#include "amethyst/graphics/capabilities.hpp"
#include <vector>
#include <memory>

namespace amethyst
{

    /**
     *
     * An aggregate (collection) of shapes.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.11 $
     *
     */
    template <typename T, typename color_type>
    class aggregate : public shape<T, color_type>
    {
    public:
        using parent = shape<T, color_type>;
        using shape_type = shape_ptr<T, color_type>;
        using shape_list = std::vector<shape_type>;

        aggregate() = default;
        virtual ~aggregate() = default;
        aggregate(const aggregate& old) = default;
        aggregate(aggregate&& old)
            : parent(std::move(old))
            , m_shape_list(std::move(old.m_shape_list))
        {
        }
        aggregate& operator=(const aggregate& old) = default;
        aggregate& operator=(aggregate&& old)
        {
            parent::operator=(std::move(old));
            m_shape_list = std::move(old.m_shape_list);
            return *this;
        }

        /** Returns if the given point is inside the shape. */
        bool inside(const point3<T>& p) const override;

        /** Returns if the given sphere intersects the shape. */
        bool intersects(const sphere<T,color_type>& s) const override;

        /** Returns if the given plane intersects the shape. */
        bool intersects(const plane<T,color_type>& p) const override;

        /** Returns if the given line intersects the shape. */
        using parent::intersects_line;

        bool intersects_line(const unit_line3<T>& line,
            intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements = intersection_requirements()) const override;

        bool intersects_ray(const ray_parameters<T,color_type>& ray,
            intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements = intersection_requirements()) const override;

        /**
         * A quick intersection test.  This will calculate nothing but the
         * distance. This is most useful for shadow tests, and other tests where no
         * textures will be applied.
         */
        bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;

        std::string name() const override { return "aggregate"; }

        intersection_capabilities get_intersection_capabilities() const override
        {
            intersection_capabilities caps = intersection_capabilities::ALL;

            // Checkme! Are any of these capabilities disjoint (like the ones in the object capabilities)?
            for (const auto& obj : m_shape_list)
            {
                caps &= obj->get_intersection_capabilities();
            }
            return caps;
        }
        object_capabilities get_object_capabilities() const override;

        void add(shape_type sp) { m_shape_list.emplace_back(std::move(sp)); }
        size_t size() const { return m_shape_list.size(); }
        shape_type& operator[](size_t index) { return m_shape_list[index]; }
        const shape_type& operator[](size_t index) const { return m_shape_list[index]; }

    private:
        shape_list m_shape_list;
    };

    template <typename T, typename color_type>
    bool aggregate<T,color_type>::inside(const point3<T>& p) const
    {
        for (const auto& s : m_shape_list)
        {
            if (s->inside(p))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    bool aggregate<T,color_type>::intersects(const sphere<T,color_type>& s) const
    {
        for(const auto& p : m_shape_list)
        {
            if (p->intersects(s))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    bool aggregate<T,color_type>::intersects(const plane<T,color_type>& p) const
    {
        for (const auto& s : m_shape_list)
        {
            if (s->intersects(p))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    bool aggregate<T,color_type>::quick_intersection(const unit_line3<T>& line, T time, T& distance) const
    {
        bool hit_something = false;
        T closest = line.limits().end() + 1;

        for(const auto& s : m_shape_list)
        {
            T dist;
            if (s->quick_intersection(line, time, dist))
            {
                if (dist < closest)
                {
                    // NOTE: The minimum does not need to be tested, because it must be
                    // within the range of the line to be considered a hit.  We'll just
                    // save that comparison, as this is slow enough already (for a quick
                    // intersection test).
                    closest = dist;
                    hit_something = true;
                }
            }
        }
        if (hit_something)
        {
            distance = closest;
        }
        return hit_something;
    }

    template <typename T, typename color_type>
    bool aggregate<T,color_type>::intersects_line(const unit_line3<T>& line,
                                       intersection_info<T,color_type>& intersection,
                                       const intersection_requirements& requirements) const
    {
        bool intersects_something = false;

        // Clear it out...
        intersection = intersection_info<T,color_type>();

        for(const auto& obj : m_shape_list)
        {
            intersection_info<T,color_type> temp_intersection;
            if (obj->intersects_line(line, temp_intersection, requirements))
            {

                if (intersects_something && requirements.needs_containers())
                {
                    temp_intersection.append_container(this);
                }

                if (!requirements.needs_all_hits())
                {
                    // This is the easy case, because the entire hit information is
                    // replaced if the hit is closer than the current (if any).
                    if (!intersects_something)
                    {
                        intersects_something = true;
                        intersection = temp_intersection;
                    }
                    else
                    {
                        if (temp_intersection.get_first_distance() < intersection.get_first_distance())
                        {
                            intersection = temp_intersection;
                        }
                        else
                        {
                            // The existing hit is closer...
                        }
                    }
                } // !all hits
                else
                {
                    // This is the harder case, or at least messier case, because some of
                    // the information will need to be preserved (I've chosen the shape,
                    // the distance), for later comparison.

                    // Append the complete intersection information, for later retrieval.
                    intersection.append_intersection(temp_intersection);
                    if (!intersects_something)
                    {
                        // Copy some of the values (distance, shape) to the main intersection container.
                        if (temp_intersection.have_shape())
                        {
                            intersection.set_shape(temp_intersection.get_shape());
                        }
                        if (temp_intersection.have_distance())
                        {
                            intersection.set_first_distance(temp_intersection.get_first_distance());
                        }
                    } // !intersects something
                    else // Do a comparison before assigning.
                    {
                        if ((temp_intersection.have_distance() && intersection.have_distance()) &&
                            (temp_intersection.get_first_distance() < intersection.get_first_distance()))
                        {
                            intersection.set_first_distance(temp_intersection.get_first_distance());
                            // Copy some of the values (distance, shape, line) to the main intersection container.
                            if (temp_intersection.have_shape())
                            {
                                intersection.set_shape(temp_intersection.get_shape());
                            }
                        } // this hit is closer than any preexisting hits.
                    } // prior hits
                } // all hits required
            } // if the shape intersects the line.
        } // for each shape.

        return intersects_something;
    }


    template <typename T, typename color_type>
    bool aggregate<T,color_type>::intersects_ray(const ray_parameters<T,color_type>& ray,
                                      intersection_info<T,color_type>& intersection,
                                      const intersection_requirements& requirements) const
    {
        bool intersects_something = false;

        // Clear it out...
        intersection = intersection_info<T,color_type>();

        for (const auto& obj : m_shape_list)
        {
            intersection_info<T,color_type> temp_intersection;
            if (obj->intersects_ray(ray, temp_intersection, requirements))
            {

                if (intersects_something && requirements.needs_containers())
                {
                    temp_intersection.append_container(this);
                }

                if (!requirements.needs_all_hits())
                {
                    // This is the easy case, because the entire hit information is
                    // replaced if the hit is closer than the current (if any).
                    if (!intersects_something)
                    {
                        intersects_something = true;
                        intersection = temp_intersection;
                    }
                    else
                    {
                        if (temp_intersection.get_first_distance() < intersection.get_first_distance())
                        {
                            intersection = temp_intersection;
                        }
                        else
                        {
                            // The existing hit is closer...
                        }
                    }
                } // !all hits
                else
                {
                    // This is the harder case, or at least messier case, because some of
                    // the information will need to be preserved (I've chosen the shape,
                    // the distance), for later comparison.

                    // Append the complete intersection information, for later retrieval.
                    intersection.append_intersection(temp_intersection);
                    if (!intersects_something)
                    {
                        // Copy some of the values (distance, shape) to the main intersection container.
                        if (temp_intersection.have_shape())
                        {
                            intersection.set_shape(temp_intersection.get_shape());
                        }
                        if (temp_intersection.have_distance())
                        {
                            intersection.set_first_distance(temp_intersection.get_first_distance());
                        }
                    } // !intersects something
                    else // Do a comparison before assigning.
                    {
                        if ((temp_intersection.have_distance() && intersection.have_distance()) &&
                            (temp_intersection.get_first_distance() < intersection.get_first_distance()))
                        {
                            intersection.set_first_distance(temp_intersection.get_first_distance());
                            // Copy some of the values (distance, shape, line) to the main intersection container.
                            if (temp_intersection.have_shape())
                            {
                                intersection.set_shape(temp_intersection.get_shape());
                            }
                        } // this hit is closer than any preexisting hits.
                    } // prior hits
                } // all hits required
            } // if the shape intersects the line.
        } // for each shape.

        return intersects_something;
    }


    template <typename T, typename color_type>
    std::string aggregate<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += aggregate<T,color_type>::name() + "::";
        }

        retval += indentation + string_format("intersection_capabilities=%1\n", to_string(get_intersection_capabilities()));
        retval += indentation + string_format("object_capabilities=%1\n", to_string(get_object_capabilities()));

        std::string level_indent = "  ";

        for( const auto& s : m_shape_list)
        {
            retval += s->to_string(indentation, level_indent) + "\n";
        }

        return retval;
    }

    template <typename T, typename color_type>
    object_capabilities aggregate<T,color_type>::get_object_capabilities() const
    {
        object_capabilities caps = object_capabilities::ALL;
        caps &= ~object_capabilities::MOVABLE;
        caps &= ~object_capabilities::SIMPLE;
        caps |= object_capabilities::CONTAINER;
        caps &= ~object_capabilities::IMPLICIT;

        for (const auto& obj : m_shape_list)
        {
            object_capabilities obj_caps = obj->get_object_capabilities();

            if (!!(obj_caps & object_capabilities::NOT_FINITE))
            {
                caps &= ~object_capabilities::BOUNDABLE;
            }
            if (!!(obj_caps & object_capabilities::BOUNDABLE))
            {
                caps &= ~object_capabilities::NOT_FINITE;
            }
            if (!!(obj_caps & object_capabilities::MOVABLE))
            {
                caps |= object_capabilities::MOVABLE;
            }
            if (!(obj_caps & object_capabilities::POLYGONIZATION))
            {
                caps &= ~object_capabilities::POLYGONIZATION;
            }
            if (!!(obj_caps & object_capabilities::IMPLICIT))
            {
                caps |= object_capabilities::IMPLICIT;
            }
        }
        return caps;
    }
}

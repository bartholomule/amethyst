#include "line_base.hpp"
#include <limits>

namespace amethyst
{
    /**
    *
    * A simple line with a unit direction.
    *
    * @author Kevin Harris <kpharris@users.sourceforge.net>
    * @version $Revision: 1.5 $
    *
    */
    template <typename T, typename point_type, typename vector_type>
    class unit_line_base : protected line_base<T, point_type, vector_type>
    {
    protected:
        using line_type = line_base<T, point_type, vector_type>;
    public:
        unit_line_base() = default;
        unit_line_base(const unit_line_base& old) = default;
        virtual ~unit_line_base() = default;
        unit_line_base& operator=(const unit_line_base& old) = default;

        unit_line_base(const point_type& o, const vector_type& v, const interval<T>& limits = line_type::infinite_interval())
            : line_type(o, unit(v), limits)
            , non_unit_length(length(v))
        {
            if (!infinite())
            {
                line_limits.set(line_limits.begin() * non_unit_length, line_limits.end() * non_unit_length);
            }
        }

        unit_line_base(const point_type& p1, const point_type& p2)
            : unit_line_base(p1, p2 - p1, line_base<T, point_type, vector_type>::segment_one())
        {
        }

        unit_line_base(const line_type& line)
            : unit_line_base(line.o(), unit(line.v()), line.limits())
        {
        }

        operator line_type() const
        {
            if (infinite())
            {
                return line_type(o(), v() * non_unit_length);
            }
            auto lim = limits();
            lim.set(lim.begin() / non_unit_length, lim.end() / non_unit_length);

            return line_type(o(), v() * non_unit_length(), lim);
        }

        T normal_length() const { return non_unit_length; }
        point_type point_at_scaled(const T& t) const { return line_origin + direction() * t * non_unit_length; }

        // Bring in all of the parent functionality without being an is-a relationship.
        using line_type::minimum;
        using line_type::maximum;
        using line_type::o;
        using line_type::v;
        using line_type::origin;
        using line_type::direction;
        using line_type::point_at;
        using line_type::infinite;
        using line_type::limits;
        using line_type::inside;
    protected:
        T non_unit_length = 1;
    };

    template <typename T, typename point_type, typename vector_type>
    std::ostream& operator<<(std::ostream& o, const unit_line_base<T,point_type,vector_type>& l)
    {
        o << "|" << l.o() << ", " << l.v() << ", " << l.normal_length() << ", " << l.limits() << "|";
        return o;
    }

    template <typename T, typename point_type, typename vector_type>
    std::string inspect(const unit_line_base<T,point_type,vector_type>& l)
    {
        return "|" + inspect(l.o()) + ", " + inspect(l.v()) + ", " + inspect(l.normal_length()) + ", " + inspect(l.limits()) + "|";
    }
}


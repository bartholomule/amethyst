#pragma once

#include "graphics/raster.hpp"
#include "graphics/rgbcolor.hpp"

namespace amethyst
{

    // Sorry about these comments, as they are meant to be viewed in html form.
    /**
     *
     * A generic class which combines the capabilities of raster and rgbcolor.
     * This class has almost no functionality of its own.
     *
     * Note the neat feature of this class: it can be reinterpreted as a pointer
     * to the base type given to it.
     *
     * For example:<br>
     * <font color="#008800">image</font>&lt;<font color="#000088">unsigned
     * char</font>&gt im(640, 480);
     *
     * <font color="#880000">
     * // This next variable can be treated as a single array of size      <br>
     * // im.numpixels( ) in length, which would be 307200 in this case.   <br>
     * </font>
     * <font color="#000088">unsigned char</font>*
     * image_data</font> =
     * im.<font color="#008800">reinterpret</font>
     * &lt;<font color="#000088">unsigned char</font>*&gt; ( );<br>
     * ...<br>
     * <font color="#880000">
     * // Act on the data, being careful not to go beyond bounds on either <br>
     * // end, or (whether intentional or not) deallocate the pointer.     <br>
     * </font>
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.7 $
     * @see raster
     * @see rgbcolor
     *
     */
    template <typename T>
    using image = raster<rgbcolor<T>>;
}

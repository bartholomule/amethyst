#pragma once

namespace amethyst
{
    // Common IOR constants.
    template <typename T>
    struct iors
    {
        // Some of these values are borrowed from wikipedia: https://en.wikipedia.org/wiki/Refractive_index
        static constexpr T vacuum = 1;
        static constexpr T helium = 1.000036;
        static constexpr T air = 1.000293;
        static constexpr T hydrogen = 1.000132;
        static constexpr T co2 = 1.000450;
        static constexpr T ice = 1.31;
        static constexpr T water = 1.333;
        static constexpr T ethanol = 1.36;
        static constexpr T olive_oil = 1.47;
        static constexpr T glass = 1.52;
        static constexpr T polycarbonate = 1.58;
        static constexpr T sapphire = 1.77;
        static constexpr T cubic_zirconia = 2.15;
        static constexpr T diamond = 2.52;
        static constexpr T moissanite = 2.65; // Naturally occuring SiC

       // Many more are listed on https://pixelandpoly.com/ior.html, should the need ever arise to have some diverse low-precision values.
    };
}
#pragma once
#include "ppm_io.hpp"
#include "tga_io.hpp"

namespace amethyst
{
	namespace impl
	{
		bool endsWith(const std::string& text, const std::string& pattern);
	}

	template <typename T, typename ColorType = rgbcolor<T>>
	std::unique_ptr<image_io<T, ColorType>> getImageLoader(const std::string& filename)
	{
		if (impl::endsWith(filename, ".ppm") || impl::endsWith(filename, ".PPM"))
		{
			return std::make_unique<ppm_io<T, ColorType>>();
		}
		if (impl::endsWith(filename, ".tga") || impl::endsWith(filename, ".TGA"))
		{
			return std::make_unique<tga_io<T, ColorType>>();
		}

		throw std::runtime_error("Unknown image format for file: " + filename);
	}

	image<double> loadImageD(const std::string& filename);
	image<uint8_t> loadImageI(const std::string& filename);

	template <typename ColorType>
	bool save_image(const std::string& filename, const raster<ColorType>& image)
	{
		auto loader = getImageLoader<ColorType::number_type, ColorType>(filename);
		return loader->output(filename, image);
	}

	template <typename ColorType>
	bool save_image(const std::string& filename, const raster<ColorType>& image, double gamma)
	{
		auto loader = getImageLoader<ColorType::number_type, ColorType>(filename);
		return loader->output(filename, image, gamma);
	}
}
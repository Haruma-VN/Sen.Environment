#pragma once

namespace Sen::Kernel::Support::Texture {

	
	// set pixel

	#define set_pixel(x, y, width) (y * width + x) * 4

	// pixel area

	#define pixel_area(area) area * 4

	// pixel area rgba

	#define pixel_area_rgba(width, height) pixel_area(calculate_area(width, height))

}

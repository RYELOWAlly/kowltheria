#ifndef MATH_HPP
#define MATH_HPP

#define TEMPERATE_THRESHOLD -0.4
#define HOT_THRESHOLD       0.3
#define WATER_THRESHOLD     -0.4
#define MED_THRESHOLD       -0.2
#define MTNHEIGHT_THRS      0.2
#define HIGH_THRESHOLD      0.3
#define MIN_HEIGHT_FOR_LOSS 0.3

#define WORLD_WIDTH       (double)360
#define WORLD_HEIGHT      (double)180

#define ZOOM_X 60
#define ZOOM_Y 40

struct vector
{
	double x;
	double y;
};

vector ProjectLLToXY(int lat, int lng);

#endif

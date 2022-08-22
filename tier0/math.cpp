#include "math.hpp"

vector ProjectLLToXY(int lat, int lng)
{
        double dlat = (double)lat;
        double dlong = (double)lng;

        vector vec;
        vec.x = (dlong + 180.0) *
                (WORLD_WIDTH / 360.0);

        double dlatrad = dlat *
                M_PI / 180.0;

        double mercN = std::log(
                std::tan((M_PI/4.0)+
                         (dlatrad/2.0))
                );

        vec.y = (WORLD_HEIGHT/2.0) -
                (WORLD_WIDTH * mercN / (2.0 * M_PI));

        return vec;
}
 

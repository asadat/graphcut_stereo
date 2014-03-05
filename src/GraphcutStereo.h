#ifndef GRAPH_CUT_STEREO_
#define GRAPH_CUT_STEREO_

#include <cvd/image_io.h>
#include <cvd/image.h>

class GraphcutStereo
{
public:
    GraphcutStereo(int argc, char ** argv);
    ~GraphcutStereo();

    void Run();

private:

    void Display();
    double RGB2Y(CVD::Rgb8 rgb){return rgb.red * 0.299 + rgb.green * 0.587 + rgb.blue * 0.114;}

    CVD::Image<CVD::byte> image[2];
    CVD::Image<CVD::byte> disparity;

};

#endif

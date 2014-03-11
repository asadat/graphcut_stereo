#ifndef GRAPH_CUT_STEREO_
#define GRAPH_CUT_STEREO_

#include <cvd/image_io.h>
#include <cvd/image.h>
#include <cvd/videodisplay.h>
#include "graph.h"

class GraphcutStereo
{
public:
    GraphcutStereo(int argc, char ** argv);
    ~GraphcutStereo();

    void Run();

private:

    typedef Graph<double,double,double> GraphType;

    bool AlphaExpansion();
    //cost functions
    double D(int i, int j, double dp);
    double V(int i1, int j1, double dp1, int i2, int j2, double dp2);
    double E(CVD::Image<CVD::byte> & disp);

    void Display();
    double RGB2Y(CVD::Rgb8 rgb){return rgb.red * 0.299 + rgb.green * 0.587 + rgb.blue * 0.114;}

    std::string fname;

    CVD::VideoDisplay *display;
    int nF;
    int F[50];
    CVD::Image<CVD::byte> image[2];
    CVD::Image<CVD::byte> disparity;

};

#endif

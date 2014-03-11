#ifndef GRAPH_CUT_STEREO_
#define GRAPH_CUT_STEREO_

#include <cvd/image_io.h>
#include <cvd/image.h>
#include <cvd/videodisplay.h>
#include "graph.h"
#include "Visualizer.h"

class GraphcutStereo: public Visualizer
{
public:
    GraphcutStereo(int argc, char ** argv);
    ~GraphcutStereo();

    void Run();
    void Update();

    CVD::Image<CVD::byte> disparity;

    void glDraw();

private:

    typedef Graph<double,double,double> GraphType;

    bool AlphaExpansion(int f=-1);
    //cost functions
    double D(int i, int j, double dp);
    double V(int i1, int j1, double dp1, int i2, int j2, double dp2);
    double E(CVD::Image<CVD::byte> & disp);

    void Display();
    double RGB2Y(CVD::Rgb<CVD::byte> rgb){return rgb.red * 0.299 + rgb.green * 0.587 + rgb.blue * 0.114;}

    std::string fname;

    CVD::VideoDisplay *display;
    int nF;
    int F[50];
    int currentF;

    CVD::Image<CVD::Rgb<CVD::byte> > img[2];
    CVD::Image<CVD::byte> image[2];

};

#endif

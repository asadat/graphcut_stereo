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

    //CVD::Image<CVD::byte> disparity;
    TooN::Matrix<1000,1000> disparity;
    void glDraw();

private:

    typedef Graph<double,double,double> GraphType;

    void HalfSample(TooN::Matrix<1000,1000> &src, TooN::Matrix<1000,1000> &dest);
    void DisparityMedian();
    bool AlphaExpansion(int f=-1);
    //cost functions
    double D(int i, int j, double dp);
    double V(int i1, int j1, double dp1, int i2, int j2, double dp2);
    double E(TooN::Matrix<1000,1000> & disp);
    double SubPixVal(int x, float y, int imageIdx);
    void Display();
    double RGB2Y(CVD::Rgb<CVD::byte> rgb){return rgb.red * 0.299 + rgb.green * 0.587 + rgb.blue * 0.114;}

    float Disparity2Depth(float disp);

    std::string fname;

    CVD::VideoDisplay *display;
    int nF;
    float F[100];
    int currentF;

    GraphType *g;
    GraphType::node_id node_id_map[1000][1000];

    CVD::Image<CVD::Rgb<CVD::byte> > img[2];
    CVD::Image<CVD::byte> image[2];
    TooN::Matrix<1000,1000> meanI;
    TooN::Matrix<1000,1000> varI;
    TooN::Matrix<1000,1000> W;
    CVD::ImageRef imgSize;
    TooN::Matrix<1000,1000> disp_sub;

    //subsamplings
    //CVD::Image<CVD::byte> disparity4x4;
    //CVD::Image<CVD::Rgb<CVD::byte> > img0_4x4;

    TooN::Matrix<1000,1000> disparity4x4;


    CVD::Image<CVD::Rgb<CVD::byte> > img0_4x4; //for saving the color of the original image
    bool drawHighRes;
};

#endif

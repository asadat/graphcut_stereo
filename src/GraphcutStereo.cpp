#include "GraphcutStereo.h"
#include "cvd/videodisplay.h"
#include "cvd/gl_helpers.h"

using namespace CVD;

GraphcutStereo::GraphcutStereo(int argc, char **argv)
{
    CVD::Image<CVD::Rgb8> img[2];
    CVD::img_load(img[0], argv[1]);
    CVD::img_load(img[1], argv[2]);

    image[0].resize(img[0].size());
    image[1].resize(img[1].size());
    disparity.resize(img[1].size());

    for(int i=0; i< img[0].size().y; i++)
        for(int j=0; j< img[0].size().x; j++)
        {
            image[0][i][j] = RGB2Y(img[0][i][j]);
            image[1][i][j] = RGB2Y(img[1][i][j]);
            disparity[i][j] = 1;
        }

    for(int i=0; i< 20; i++)
        F[i] = i;

    display = new VideoDisplay(image[0].size());
}

GraphcutStereo::~GraphcutStereo()
{

}

void GraphcutStereo::Run()
{
    bool flag = true;
    while(flag)
    {
        flag = AlphaExpansion();
        Display();
    }
}

void GraphcutStereo::Display()
{


    Image<byte> disimg;
    disimg.resize(disparity.size());

    for(int i=0; i< disparity.size().y; i++)
        for(int j=0; j< disparity.size().x; j++)
        {
            disimg[i][j] = disparity[i][j]*20;
        }

    glDrawPixels(disimg);

//    glColor3f(1,0,0);
//    glBegin(GL_LINES);
//    glVertex(image[0].size()/2);
//    glVertex(image[0].size());
//    glEnd();
    glFlush();

    //std::cin.get();
}

double GraphcutStereo::D(int i, int j, double dp)
{
    int j2 = j - dp;
    double val = 255;
    if(j2 >= 0)
    {

        double val1 = fabs(image[0][i][j] - (image[1][i][j2]));
        double val2 = 255*255;
        if(j2-1>=0 )
            val2 = fabs(image[0][i][j] - (0.5*image[1][i][j2-1] + 0.5*image[1][i][j2]));

        double val3 = 255*255;
        if(j2+1 < image[0].size().x)
            val3 = fabs(image[0][i][j] - (0.5*image[1][i][j2] + 0.5*image[1][i][j2+1]));

        if(val1<val2)
        {
            if(val3<val1)
                val = val3;
            else
                val = val1;
        }
        else
        {
            if(val3<val2)
                val = val3;
            else
                val = val2;
        }

        //val *= (1.0/255.0);
        val = fabs(val*val);
    }

    return val;
}

double GraphcutStereo::V(int i1, int j1, double dp1, int i2, int j2, double dp2)
{
    //Potts model for now;
//    double val=0;
//    if(fabs(dp1 - dp2) > 0.5)
//    {
//        if(fabs(image[0][i1][j1] - image[0][i2][j2]) <= 5)
//            val = 100;
//        else
//            val = 50;
//    }

//    return val;

    double val =0;
    val = 5*fabs(dp1-dp2);
    if(val > 10)
        val = 10;

    return val;
}

double GraphcutStereo::E(Image<byte> &disp)
{
    double energy = 0;
    ImageRef size = image[0].size();
    for(int i=0; i<size.y; i++)
        for(int j=0; j<size.x; j++)
        {
            //printf("D1\n");
            energy += D(i,j,disp[i][j]);
            //printf("D2\n");

            if(j+1 < size.x)
            {
                //printf("V1\n");
                energy += V(i,j,disp[i][j],i,j+1,disp[i][j+1]);
                //printf("V2\n");
            }
        }

    return energy;

}

//GraphcutStereo::GraphType::node_id GraphcutStereo::add_node_if_necessary(, std::map<int, GraphType::node_id> &node_id_map, GraphType *g)
//{
//    GraphType::node_id  id;
//    if ( node_id_map.find(n) != node_id_map.end() )
//    {
//        id = node_id_map[ n ];
//    }
//    else
//    {
//        id = g->add_node();
//        node_id_map[ n ] = id;
//    }
//    return id;
//}

bool GraphcutStereo::AlphaExpansion()
{
    ImageRef size = image[0].size();
    Image<byte> newdisp(disparity.copy_from_me());

    static double lastE=99999999999;

    for(int k=0; k<20; k++)
    {
        int f = F[k];
        printf("alpha = %d\n", f);
        std::map<std::pair<int,int>, GraphType::node_id> nodeidMap;
        GraphType *g = new GraphType(size.x*size.y*2,size.x*size.y*2*4);
        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                std::pair<int, int> px;
                px.first = i;
                px.second = j;
                GraphType::node_id nid = g->add_node();
                nodeidMap[px] = nid;

                g->add_tweights(nid, D(i,j,disparity[i][j]), D(i,j,f));
            }

        for(int i=0; i<size.y-1; i++)
            for(int j=0; j<size.x-1; j++)
            {
                std::pair<int, int> pxl;
                pxl.first = i;
                pxl.second = j;
                std::pair<int, int> pxr;
                pxr.first = i;
                pxr.second = j+1;

                std::pair<int, int> pxd;
                pxr.first = i+1;
                pxr.second = j;

                GraphType::node_id lnode = nodeidMap[pxl];
                GraphType::node_id rnode = nodeidMap[pxr];
                GraphType::node_id dnode = nodeidMap[pxd];

                GraphType::node_id nid = g->add_node();

                g->add_tweights(nid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);
                g->add_edge(lnode, nid, V(i,j,disparity[i][j], i,j+1,f),0);
                g->add_edge(nid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

                GraphType::node_id dnid = g->add_node();
                g->add_tweights(dnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
                g->add_edge(lnode, dnid, V(i,j,disparity[i][j], i+1,j,f),0);
                g->add_edge(dnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);

            }

        double flow = g->maxflow();
       // printf("flow:%f\n", flow);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                std::pair<int, int> px;
                px.first = i;
                px.second = j;
                GraphType::node_id nodeid = nodeidMap[px];
                if(g->what_segment(nodeid) == GraphType::SOURCE)
                {
                    //printf("In source ..\n");
                    newdisp[i][j] = f;
                }
            }

        double newE = E(newdisp);
        if( newE < lastE)
        {
            lastE = newE;
            printf("flow reduced to :%f\n", flow);
            disparity.copy_from(newdisp);
        }

        nodeidMap.clear();
        delete g;
    }


    return true;
}

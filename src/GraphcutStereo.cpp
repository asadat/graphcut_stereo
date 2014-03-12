#include "GraphcutStereo.h"
#include "cvd/videodisplay.h"
#include "cvd/gl_helpers.h"
#include <sys/time.h>
#include "cvd/vision.h"

using namespace CVD;

GraphcutStereo::GraphcutStereo(int argc, char **argv)
{
    drawHighRes = false;
    nF = 20;
    fname.clear();
    fname.append(argv[1]);


    CVD::img_load(img[0], argv[1]);
    CVD::img_load(img[1], argv[2]);

    ImageRef size = img[0].size();

    image[0].resize(size);
    image[1].resize(size);
    disparity.resize(size);
    img0_4x4 = CVD::halfSample(img[0]);

    for(int i=0; i< size.y; i++)
        for(int j=0; j< size.x; j++)
        {
            image[0][i][j] = RGB2Y(img[0][i][j]);
            image[1][i][j] = RGB2Y(img[1][i][j]);
            disparity[i][j] = 0;
        }

    for(int i=0; i< nF; i++)
        F[i] = -1;

    for(int i=0; i< nF; i++)
    {
        F[i]=nF-1-i;
        continue;

        while(true)
        {
            int j=rand()%(nF);
            if(F[j] == -1)
            {
                F[j] = i;
                break;
            }
        }
    }

    g = new GraphType(size.x*size.y*2,size.x*size.y*2*4);
    currentF = 0;

    std::string output_name = fname;
    output_name.erase(output_name.begin()+output_name.length()-4,output_name.begin()+output_name.length());
    CVD::img_save(image[0],output_name+ "disparity.png");

    //display = new VideoDisplay(image[0].size());
    //display = new VideoDisplay(0,0,400,400);
}

GraphcutStereo::~GraphcutStereo()
{

}

void GraphcutStereo::Run()
{
    int n=0;
    bool flag = true;
    while(flag)
    {
        n++;

//        timeval t2;
//        timeval t1;
//       gettimeofday(&t1, NULL);


        flag = AlphaExpansion();
//        gettimeofday(&t2, NULL);

//        double elapsedTime = (-t1.tv_sec + t2.tv_sec) * 1000.0;      // sec to ms
//        elapsedTime += (-t1.tv_usec + t2.tv_usec) / 1000.0;   // us to ms

//        printf("DTime: %f \n", elapsedTime);
        Display();
        if(n>4)
            return;
    }
}

void GraphcutStereo::Update()
{
    currentF++;
    if(currentF >= nF)
        currentF = 0;

    drawHighRes = AlphaExpansion(currentF);

}

void GraphcutStereo::glDraw()
{
//    float w=10;
//    glLineWidth(2);
//    glColor3f(0,0,0);
//    glBegin(GL_LINES);
//    for(int i=0; i<=w; i++)
//    {
//        glVertex3f(-w/2, -w/2+i, 0);
//        glVertex3f( w/2, -w/2+i, 0);
//    }

//    for(int i=0; i<=w; i++)
//    {
//        glVertex3f(-w/2+i, -w/2, 0);
//        glVertex3f(-w/2+i,  w/2, 0);
//    }

//    glEnd();



    float dx = -0.1;
    float dy = 0.1;
    float dz = 0.5;
    int ds = 1;
    if(drawHighRes)
        ds=4;

    ImageRef size = disparity4x4.size();
    glPointSize(5);
    glBegin(GL_QUADS);
    for(int i=0; i<size.y-ds; i+=ds)
        for(int j=0; j<size.x-ds; j+=ds)
        {
	    

            CVD::Rgb<CVD::byte> c = img0_4x4[i][j];
            //glColor(c.red/255.0, c.green/255.0, c.blue/255.0);
            glColor(c);
            glVertex3f(j*dy,Disparity2Depth(disparity4x4[i][j])*dz,i*dx);
            glVertex3f(j*dy,Disparity2Depth(disparity4x4[i+ds][j])*dz,(i+ds)*dx);
            glVertex3f((j+ds)*dy,Disparity2Depth(disparity4x4[i+ds][j+ds])*dz,(i+ds)*dx);
            glVertex3f((j+ds)*dy,Disparity2Depth(disparity4x4[i][j+ds])*dz,i*dx);
        }
    glEnd();
}

void GraphcutStereo::Display()
{
    Image<byte> disimg;
    disimg.resize(disparity.size());

    for(int i=0; i< disparity.size().y; i++)
        for(int j=0; j< disparity.size().x; j++)
        {
            int cd = disparity[i][j]*12.5;
            if(cd > 255)
                cd= 255;

            disimg[i][j] = cd;
        }

    std::string output_name = fname;
    output_name.erase(output_name.begin()+output_name.length()-4,output_name.begin()+output_name.length());
    CVD::img_save(disimg,output_name+ "disparity.png");
//    glDrawPixels(disimg);

////    glColor3f(1,0,0);
////    glBegin(GL_LINES);
////    glVertex(image[0].size()/2);
////    glVertex(image[0].size());
////    glEnd();
//    glFlush();

    //std::cin.get();
}

double GraphcutStereo::D(int i, int j, double dp)
{
    double val=0;
    for(int ii=i-2; ii<=i+2;ii++)
        for(int jj=j-2; jj<=j+2;jj++)
        {
            int jj2 = jj-dp;
            if(ii <0 || jj < 0 || ii >= image[0].size().y || jj >= image[0].size().x || jj2 < 0)
                val +=255;
            else
                val += fabs(image[0][ii][jj] - image[1][ii][jj2]);
        }

    val = (val/(5*5))<200?(val/(5*5)):200;

    return val*val;

//    int j2 = j - dp;
//    double val = 255;
//    if(j2 >= 0)
//    {

//        double val1 = fabs(image[0][i][j] - (image[1][i][j2]));
//        double val2 = 255*255;
//        if(j2-1>=0 )
//            val2 = fabs(image[0][i][j] - (0.5*image[1][i][j2-1] + 0.5*image[1][i][j2]));

//        double val3 = 255*255;
//        if(j2+1 < image[0].size().x)
//            val3 = fabs(image[0][i][j] - (0.5*image[1][i][j2] + 0.5*image[1][i][j2+1]));

//        if(val1<val2)
//        {
//            if(val3<val1)
//                val = val3;
//            else
//                val = val1;
//        }
//        else
//        {
//            if(val3<val2)
//                val = val3;
//            else
//                val = val2;
//        }

//        //val *= (1.0/255.0);
//        val = fabs(val*val);
//    }

//    return val;
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

    double val = 0;
    val = 10*fabs(dp1-dp2);
    if(val >= 20)
    {
        val = 20;
    }

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

float GraphcutStereo::Disparity2Depth(int disp)
{
    return nF - disp;
}

bool GraphcutStereo::AlphaExpansion(int f)
{
    static int round = 0;
    round++;
    if(round > nF*1.5)
        return false;

    ImageRef size = image[0].size();

    static double lastE=99999999999;

    Image<byte> newdisp(disparity.copy_from_me());
    //std::map<std::pair<int,int>, GraphType::node_id> nodeidMap;

    int initF = (f==-1)?0:f;
    int endF = (f==-1)?nF:(f+1);

    for(int k=initF; k<endF; k++)
    {
        g->reset();
      //  nodeidMap.clear();

        timeval t2;
        timeval t1;
       gettimeofday(&t1, NULL);

        int f = F[k];
        printf("alpha = %d\n", f);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                node_id_map[i][j] = -1;
            }

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
                newdisp[i][j] = disparity[i][j];

                //std::pair<int, int> px;
                //px.first = i;
                //px.second = j;
                GraphType::node_id nid = node_id_map[i][j];
                if(nid == -1)
                {
                    nid = g->add_node();
                    node_id_map[i][j] = nid;
                }


                g->add_tweights(nid, D(i,j,disparity[i][j]), D(i,j,f));


                GraphType::node_id lnode = nid;
                GraphType::node_id rnode = node_id_map[i][j+1];
                GraphType::node_id dnode = node_id_map[i+1][j];

                if(rnode == -1)
                {
                    rnode = g->add_node();
                    node_id_map[i][j+1] = rnode;
                }

                if(dnode == -1)
                {
                    dnode = g->add_node();
                    node_id_map[i+1][j] = dnode;
                }

                GraphType::node_id anid = g->add_node();

                g->add_tweights(anid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);
                g->add_edge(lnode, anid, V(i,j,disparity[i][j], i,j+1,f),0);
                g->add_edge(anid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

                GraphType::node_id adnid = g->add_node();
                g->add_tweights(adnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
                g->add_edge(lnode, adnid, V(i,j,disparity[i][j], i+1,j,f),0);
                g->add_edge(adnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);
            }

//        for(int i=0; i<size.y-1; i++)
//            for(int j=0; j<size.x-1; j++)
//            {
////                std::pair<int, int> pxl;
////                pxl.first = i;
////                pxl.second = j;
////                std::pair<int, int> pxr;
////                pxr.first = i;
////                pxr.second = j+1;

////                std::pair<int, int> pxd;
////                pxr.first = i+1;
////                pxr.second = j;

////                GraphType::node_id lnode = nodeidMap[pxl];
////                GraphType::node_id rnode = nodeidMap[pxr];
////                GraphType::node_id dnode = nodeidMap[pxd];

//                GraphType::node_id lnode = node_id_map[i][j];
//                GraphType::node_id rnode = node_id_map[i][j+1];
//                GraphType::node_id dnode = node_id_map[i+1][j];

//                GraphType::node_id nid = g->add_node();

//                g->add_tweights(nid, V(i,j,disparity[i][j], i,j+1,disparity[i][j+1]), 0);
//                g->add_edge(lnode, nid, V(i,j,disparity[i][j], i,j+1,f),0);
//                g->add_edge(nid, rnode, V(i,j,f, i,j+1,disparity[i][j+1]), 0);

//                GraphType::node_id dnid = g->add_node();
//                g->add_tweights(dnid, V(i,j,disparity[i][j], i+1,j,disparity[i+1][j]), 0);
//                g->add_edge(lnode, dnid, V(i,j,disparity[i][j], i+1,j,f),0);
//                g->add_edge(dnid, dnode, V(i,j,f, i+1,j,disparity[i+1][j]),0);

//            }

        timeval t22;
        timeval t12;


       gettimeofday(&t12, NULL);

       double elapsedTime3 = (-t1.tv_sec + t12.tv_sec) * 1000.0;      // sec to ms
       elapsedTime3 += (-t1.tv_usec + t12.tv_usec) / 1000.0;   // us to ms

       printf("Setup Time: %f \n", elapsedTime3/1000.0);

        double flow = g->maxflow();
       // printf("flow:%f\n", flow);
        gettimeofday(&t22, NULL);

        double elapsedTime2 = (-t12.tv_sec + t22.tv_sec) * 1000.0;      // sec to ms
        elapsedTime2 += (-t12.tv_usec + t22.tv_usec) / 1000.0;   // us to ms

        printf("Flow Time: %f \n", elapsedTime2/1000.0);

        for(int i=0; i<size.y; i++)
            for(int j=0; j<size.x; j++)
            {
//                std::pair<int, int> px;
//                px.first = i;
//                px.second = j;
//                GraphType::node_id nodeid = nodeidMap[px];
                GraphType::node_id nodeid = node_id_map[i][j];
                if(g->what_segment(nodeid) == GraphType::SOURCE)
                {
                    //printf("In source ..\n");
                    newdisp[i][j] = f;
                }
            }

        double newE = E(newdisp);
        if( newE < lastE)
        {
            round = 0;
            printf("flow reduced from:%f to:%f\n", lastE, newE);
            lastE = newE;
            disparity.copy_from(newdisp);
            disparity4x4 = CVD::halfSample(disparity);
        }

        //nodeidMap.clear();
        //delete g;


        gettimeofday(&t2, NULL);

        double elapsedTime = (-t1.tv_sec + t2.tv_sec) * 1000.0;      // sec to ms
        elapsedTime += (-t1.tv_usec + t2.tv_usec) / 1000.0;   // us to ms

        printf("DTime: %f \n", elapsedTime/1000.0);
    }


    return true;
}



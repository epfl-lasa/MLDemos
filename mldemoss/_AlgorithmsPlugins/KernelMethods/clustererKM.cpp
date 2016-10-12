/*********************************************************************
MLDemos: A User-Friendly visualization toolkit for machine learning
Copyright (C) 2010  Basilio Noris
Contact: mldemos@b4silio.com

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*********************************************************************/
#include "public.h"
#include "clustererKM.h"
#include <numeric>
#include <assert.h>

using namespace std;

ClustererKM::~ClustererKM()
{
    DEL(kmeans);
}

void ClustererKM::Train(std::vector< fvec > samples)
{
    if(!samples.size()) return;
    if(!nbClusters) nbClusters = 1;
    if(!bIterative)
    {
        DEL(kmeans);
    }
    bool bInit = false;
    if(kmeans && kmeans->GetClusters() != nbClusters) DEL(kmeans);
    if(!kmeans)
    {
        bInit = true;
        kmeans = new KMeansCluster(nbClusters);
        kmeans->AddPoints(samples);
        kmeans->SetPlusPlus(kmeansPlusPlus);
        kmeans->InitClusters();
    }
    kmeans->SetSoft(bSoft);
    kmeans->SetGMM(bGmm);
    kmeans->SetBeta(beta);
    kmeans->SetPower(power);

    kmeans->Update(bInit);

    if(!bIterative)
    {
        int iterations = 20;
        FOR(i, iterations) kmeans->Update();
    }
}

fvec ClustererKM::Test( const fvec &sample)
{
    fvec res;
    res.resize(nbClusters,0);
    if(!kmeans) return res;
    kmeans->Test(sample, res);
    float sum = 0;
    FOR(i, res.size()) sum += res[i];
    FOR(i, res.size()) res[i] /= sum;
    return res;
}

fvec ClustererKM::Test( const fVec &sample)
{
    fvec res;
    res.resize(nbClusters,0);
    if(!kmeans) return res;
    kmeans->Test(sample, res);
    float sum = 0;
    FOR(i, res.size()) sum += res[i];
    FOR(i, res.size()) res[i] /= sum;
    return res;
}

void ClustererKM::SetParams(u32 clusters, int method, float beta, int power, bool kmeansPlusPlus)
{

    this->nbClusters = clusters;
    this->beta = beta;
    this->power = power;
    this->kmeansPlusPlus = kmeansPlusPlus;
    this->method = method;

    switch(method)
    {
    case t_kmeans:
        this->bSoft = false;
        this->bGmm = false;
        break;
    case t_soft:
        this->bSoft = true;
        this->bGmm = false;
        break;
    case t_gmm:
        this->bSoft = false;
        this->bGmm = true;
        break;
    }
}


const char *ClustererKM::GetInfoString()
{
    char *text = new char[1024];
    sprintf(text, "K-Means\n");
    sprintf(text, "%sClusters: %d\n", text, nbClusters);
    sprintf(text, "%sType:", text);
    if(!bSoft && !bGmm) sprintf(text, "%sK-Means (plusplus: %i)\n", text, kmeansPlusPlus);
    else if(bSoft) sprintf(text, "%sSoft K-Means (beta: %.3f, plusplus: %i)\n", text, beta, kmeansPlusPlus);
    else sprintf(text, "%sGMM\n", text);
    sprintf(text, "%sMetric: ", text);
    switch(power)
    {
    case 0:
        sprintf(text, "%sinfinite norm\n", text);
        break;
    case 1:
        sprintf(text, "%s1-norm (Manhattan)\n", text);
        break;
    case 2:
        sprintf(text, "%s2-norm (Euclidean)\n", text);
        break;
    default:
        sprintf(text, "%s%d-norm\n", text, power);
        break;
    }
    return text;
}


float ClustererKM::GetLogLikelihood(std::vector<fvec> const & samples){
    float RSS=0;

    std::vector<fvec> means = kmeans->GetMeans();
    std::vector<ClusterPoint> const & clusterPoint = kmeans->GetClusterPoints();

 /*   if(means.size() == 0){
        std::cout<< "mean.size() == 0" << std::endl;
        std::cout<< "samples.size(): " << samples.size() << std::endl;
   }

    if( means.size() == 2){
        std::cout<< "means.size() == 2" << std::endl;
    }
*/

    int i,k,d;
    float dist=0;
    unsigned int D = means[0].size();
    fvec rss(means.size());

    switch(method)
    {
    case t_kmeans:
    {
        FOR(i,clusterPoint.size()){

            dist=0;
            k = clusterPoint[i].cluster;
            FOR(d,D){
/*                if(samples[i][0] != clusterPoint[i].point[0]){
                    std::cout<< "--- Not Equal --- " << std::endl;
                    std::cout<< samples[i][0] << " " << clusterPoint[i].point[d] << std::endl;
                }
                */
                dist = dist + (means[k][d] - clusterPoint[i].point[d]) * (means[k][d] - clusterPoint[i].point[d]);
            }
            rss[k] = rss[k] + dist;
        }
        break;
    }
    case t_soft:
    {
        FOR(i,clusterPoint.size()){
            FOR(k,means.size()){
                dist=0;
                FOR(d,D){
                    dist = dist + (means[k][d] - clusterPoint[i].point[d]) * (means[k][d] - clusterPoint[i].point[d]);
                }
                rss[k] =+ clusterPoint[i].weights[k] * dist;
            }
        }
    }
        break;
    }


    FOR(k,rss.size()){
        RSS =+ rss[k];
    }

    return RSS;
}

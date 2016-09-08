/*
Copyright (c) 2016, Los Alamos National Security, LLC
All rights reserved.

Copyright 2016. Los Alamos National Security, LLC. This software was produced 
under U.S. Government contract DE-AC52-06NA25396 for Los Alamos National 
Laboratory (LANL), which is operated by Los Alamos National Security, LLC for 
the U.S. Department of Energy. The U.S. Government has rights to use, 
reproduce, and distribute this software.  NEITHER THE GOVERNMENT NOR LOS 
ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR 
ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  If software is modified 
to produce derivative works, such modified software should be clearly marked, 
so as not to confuse it with the version available from LANL.

Additionally, redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following conditions 
are met:
1.      Redistributions of source code must retain the above copyright notice, 
        this list of conditions and the following disclaimer.
2.      Redistributions in binary form must reproduce the above copyright 
        notice, this list of conditions and the following disclaimer in the 
        documentation and/or other materials provided with the distribution.
3.      Neither the name of Los Alamos National Security, LLC, Los Alamos 
        National Laboratory, LANL, the U.S. Government, nor the names of its 
        contributors may be used to endorse or promote products derived from 
        this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT 
NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL 
SECURITY, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Quadrature.hh"
#include "Assert.hh"
#include <stdio.h>
#include <math.h>


// Gaussian quadratures
// Made by util/CreateGaussLegendre.py
static const double s_n2[] = { -0.5773502691896257, 0.5773502691896257 };
static const double s_w2[] = { 1.0000000000000000, 1.0000000000000000 };
static const double s_n4[] = { -0.8611363115940526, -0.3399810435848563, 0.3399810435848563, 0.8611363115940526 };
static const double s_w4[] = { 0.3478548451374540, 0.6521451548625462, 0.6521451548625462, 0.3478548451374540 };
static const double s_n6[] = { -0.9324695142031521, -0.6612093864662645, -0.2386191860831969, 0.2386191860831969, 0.6612093864662645, 0.9324695142031521 };
static const double s_w6[] = { 0.1713244923791694, 0.3607615730481388, 0.4679139345726918, 0.4679139345726918, 0.3607615730481388, 0.1713244923791694 };
static const double s_n8[] = { -0.9602898564975362, -0.7966664774136267, -0.5255324099163290, -0.1834346424956498, 0.1834346424956498, 0.5255324099163290, 0.7966664774136267, 0.9602898564975362 };
static const double s_w8[] = { 0.1012285362903753, 0.2223810344533751, 0.3137066458778874, 0.3626837833783620, 0.3626837833783620, 0.3137066458778874, 0.2223810344533751, 0.1012285362903753 };
static const double s_n10[] = { -0.9739065285171717, -0.8650633666889845, -0.6794095682990244, -0.4333953941292472, -0.1488743389816312, 0.1488743389816312, 0.4333953941292472, 0.6794095682990244, 0.8650633666889845, 0.9739065285171717 };
static const double s_w10[] = { 0.0666713443086850, 0.1494513491505808, 0.2190863625159831, 0.2692667193099972, 0.2955242247147539, 0.2955242247147539, 0.2692667193099972, 0.2190863625159831, 0.1494513491505808, 0.0666713443086850 };
static const double s_n12[] = { -0.9815606342467192, -0.9041172563704748, -0.7699026741943047, -0.5873179542866174, -0.3678314989981802, -0.1252334085114689, 0.1252334085114689, 0.3678314989981802, 0.5873179542866174, 0.7699026741943047, 0.9041172563704748, 0.9815606342467192 };
static const double s_w12[] = { 0.0471753363865124, 0.1069393259953184, 0.1600783285433459, 0.2031674267230659, 0.2334925365383547, 0.2491470458134026, 0.2491470458134026, 0.2334925365383547, 0.2031674267230659, 0.1600783285433459, 0.1069393259953184, 0.0471753363865124 };
static const double s_n14[] = { -0.9862838086968124, -0.9284348836635735, -0.8272013150697650, -0.6872929048116855, -0.5152486363581541, -0.3191123689278897, -0.1080549487073437, 0.1080549487073437, 0.3191123689278897, 0.5152486363581541, 0.6872929048116855, 0.8272013150697650, 0.9284348836635735, 0.9862838086968124 };
static const double s_w14[] = { 0.0351194603317524, 0.0801580871597607, 0.1215185706879027, 0.1572031671581933, 0.1855383974779377, 0.2051984637212955, 0.2152638534631576, 0.2152638534631576, 0.2051984637212955, 0.1855383974779377, 0.1572031671581933, 0.1215185706879027, 0.0801580871597607, 0.0351194603317524 };
static const double s_n16[] = { -0.9894009349916499, -0.9445750230732326, -0.8656312023878318, -0.7554044083550030, -0.6178762444026438, -0.4580167776572274, -0.2816035507792589, -0.0950125098376375, 0.0950125098376375, 0.2816035507792589, 0.4580167776572274, 0.6178762444026438, 0.7554044083550030, 0.8656312023878318, 0.9445750230732326, 0.9894009349916499 };
static const double s_w16[] = { 0.0271524594117528, 0.0622535239386485, 0.0951585116824923, 0.1246289712555341, 0.1495959888165768, 0.1691565193950028, 0.1826034150449238, 0.1894506104550687, 0.1894506104550687, 0.1826034150449238, 0.1691565193950028, 0.1495959888165768, 0.1246289712555341, 0.0951585116824923, 0.0622535239386485, 0.0271524594117528 };
static const double s_n18[] = { -0.9915651684209309, -0.9558239495713977, -0.8926024664975558, -0.8037049589725231, -0.6916870430603532, -0.5597708310739475, -0.4117511614628426, -0.2518862256915055, -0.0847750130417353, 0.0847750130417353, 0.2518862256915055, 0.4117511614628426, 0.5597708310739475, 0.6916870430603532, 0.8037049589725231, 0.8926024664975558, 0.9558239495713977, 0.9915651684209309 };
static const double s_w18[] = { 0.0216160135264830, 0.0497145488949704, 0.0764257302548894, 0.1009420441062870, 0.1225552067114782, 0.1406429146706505, 0.1546846751262652, 0.1642764837458328, 0.1691423829631436, 0.1691423829631436, 0.1642764837458328, 0.1546846751262652, 0.1406429146706505, 0.1225552067114782, 0.1009420441062870, 0.0764257302548894, 0.0497145488949704, 0.0216160135264830 };
static const double s_n20[] = { -0.9931285991850949, -0.9639719272779138, -0.9122344282513259, -0.8391169718222188, -0.7463319064601508, -0.6360536807265150, -0.5108670019508271, -0.3737060887154195, -0.2277858511416451, -0.0765265211334973, 0.0765265211334973, 0.2277858511416451, 0.3737060887154195, 0.5108670019508271, 0.6360536807265150, 0.7463319064601508, 0.8391169718222188, 0.9122344282513259, 0.9639719272779138, 0.9931285991850949 };
static const double s_w20[] = { 0.0176140071391515, 0.0406014298003864, 0.0626720483341095, 0.0832767415767052, 0.1019301198172402, 0.1181945319615184, 0.1316886384491767, 0.1420961093183822, 0.1491729864726040, 0.1527533871307260, 0.1527533871307260, 0.1491729864726040, 0.1420961093183822, 0.1316886384491767, 0.1181945319615184, 0.1019301198172402, 0.0832767415767052, 0.0626720483341095, 0.0406014298003864, 0.0176140071391515 };
static const double s_n22[] = { -0.9942945854823994, -0.9700604978354287, -0.9269567721871740, -0.8658125777203002, -0.7878168059792081, -0.6944872631866827, -0.5876404035069116, -0.4693558379867570, -0.3419358208920842, -0.2078604266882213, -0.0697392733197222, 0.0697392733197222, 0.2078604266882213, 0.3419358208920842, 0.4693558379867570, 0.5876404035069116, 0.6944872631866827, 0.7878168059792081, 0.8658125777203002, 0.9269567721871740, 0.9700604978354287, 0.9942945854823994 };
static const double s_w22[] = { 0.0146279952982719, 0.0337749015848153, 0.0522933351526825, 0.0697964684245201, 0.0859416062170677, 0.1004141444428810, 0.1129322960805393, 0.1232523768105125, 0.1311735047870624, 0.1365414983460153, 0.1392518728556321, 0.1392518728556321, 0.1365414983460153, 0.1311735047870624, 0.1232523768105125, 0.1129322960805393, 0.1004141444428810, 0.0859416062170677, 0.0697964684245201, 0.0522933351526825, 0.0337749015848153, 0.0146279952982719 };
static const double s_n24[] = { -0.9951872199970214, -0.9747285559713095, -0.9382745520027328, -0.8864155270044011, -0.8200019859739029, -0.7401241915785544, -0.6480936519369755, -0.5454214713888396, -0.4337935076260451, -0.3150426796961634, -0.1911188674736163, -0.0640568928626056, 0.0640568928626056, 0.1911188674736163, 0.3150426796961634, 0.4337935076260451, 0.5454214713888396, 0.6480936519369755, 0.7401241915785544, 0.8200019859739029, 0.8864155270044011, 0.9382745520027328, 0.9747285559713095, 0.9951872199970214 };
static const double s_w24[] = { 0.0123412297999860, 0.0285313886289335, 0.0442774388174196, 0.0592985849154367, 0.0733464814110802, 0.0861901615319536, 0.0976186521041139, 0.1074442701159658, 0.1155056680537258, 0.1216704729278036, 0.1258374563468286, 0.1279381953467525, 0.1279381953467525, 0.1258374563468286, 0.1216704729278036, 0.1155056680537258, 0.1074442701159658, 0.0976186521041139, 0.0861901615319536, 0.0733464814110802, 0.0592985849154367, 0.0442774388174196, 0.0285313886289335, 0.0123412297999860 };
static const double s_n26[] = { -0.9958857011456169, -0.9783854459564709, -0.9471590666617142, -0.9026378619843071, -0.8454459427884981, -0.7763859488206788, -0.6964272604199573, -0.6066922930176181, -0.5084407148245057, -0.4030517551234863, -0.2920048394859569, -0.1768588203568902, -0.0592300934293132, 0.0592300934293132, 0.1768588203568902, 0.2920048394859569, 0.4030517551234863, 0.5084407148245057, 0.6066922930176181, 0.6964272604199573, 0.7763859488206788, 0.8454459427884981, 0.9026378619843071, 0.9471590666617142, 0.9783854459564709, 0.9958857011456169 };
static const double s_w26[] = { 0.0105513726173399, 0.0244178510926323, 0.0379623832943637, 0.0509758252971484, 0.0632740463295749, 0.0746841497656602, 0.0850458943134853, 0.0942138003559144, 0.1020591610944256, 0.1084718405285767, 0.1133618165463197, 0.1166604434852967, 0.1183214152792624, 0.1183214152792624, 0.1166604434852967, 0.1133618165463197, 0.1084718405285767, 0.1020591610944256, 0.0942138003559144, 0.0850458943134853, 0.0746841497656602, 0.0632740463295749, 0.0509758252971484, 0.0379623832943637, 0.0244178510926323, 0.0105513726173399 };
static const double s_n28[] = { -0.9964424975739544, -0.9813031653708728, -0.9542592806289382, -0.9156330263921321, -0.8658925225743951, -0.8056413709171791, -0.7356108780136318, -0.6566510940388650, -0.5697204718114017, -0.4758742249551183, -0.3762515160890787, -0.2720616276351780, -0.1645692821333808, -0.0550792898840343, 0.0550792898840343, 0.1645692821333808, 0.2720616276351780, 0.3762515160890787, 0.4758742249551183, 0.5697204718114017, 0.6566510940388650, 0.7356108780136318, 0.8056413709171791, 0.8658925225743951, 0.9156330263921321, 0.9542592806289382, 0.9813031653708728, 0.9964424975739544 };
static const double s_w28[] = { 0.0091242825930933, 0.0211321125927716, 0.0329014277823041, 0.0442729347590042, 0.0551073456757166, 0.0652729239669995, 0.0746462142345688, 0.0831134172289013, 0.0905717443930329, 0.0969306579979302, 0.1021129675780610, 0.1060557659228466, 0.1087111922582944, 0.1100470130164755, 0.1100470130164755, 0.1087111922582944, 0.1060557659228466, 0.1021129675780610, 0.0969306579979302, 0.0905717443930329, 0.0831134172289013, 0.0746462142345688, 0.0652729239669995, 0.0551073456757166, 0.0442729347590042, 0.0329014277823041, 0.0211321125927716, 0.0091242825930933 };
static const double s_n30[] = { -0.9968934840746495, -0.9836681232797472, -0.9600218649683075, -0.9262000474292743, -0.8825605357920527, -0.8295657623827684, -0.7677774321048262, -0.6978504947933158, -0.6205261829892429, -0.5366241481420199, -0.4470337695380892, -0.3527047255308781, -0.2546369261678899, -0.1538699136085835, -0.0514718425553177, 0.0514718425553177, 0.1538699136085835, 0.2546369261678899, 0.3527047255308781, 0.4470337695380892, 0.5366241481420199, 0.6205261829892429, 0.6978504947933158, 0.7677774321048262, 0.8295657623827684, 0.8825605357920527, 0.9262000474292743, 0.9600218649683075, 0.9836681232797472, 0.9968934840746495 };
static const double s_w30[] = { 0.0079681924961701, 0.0184664683110921, 0.0287847078833228, 0.0387991925696264, 0.0484026728305939, 0.0574931562176186, 0.0659742298821804, 0.0737559747377048, 0.0807558952294198, 0.0868997872010827, 0.0921225222377858, 0.0963687371746439, 0.0995934205867949, 0.1017623897484052, 0.1028526528935585, 0.1028526528935585, 0.1017623897484052, 0.0995934205867949, 0.0963687371746439, 0.0921225222377858, 0.0868997872010827, 0.0807558952294198, 0.0737559747377048, 0.0659742298821804, 0.0574931562176186, 0.0484026728305939, 0.0387991925696264, 0.0287847078833228, 0.0184664683110921, 0.0079681924961701 };


/* 
    getXi
    
    Returns first coordinate of angle.
*/
double Quadrature::getXi(const UINT angle) const
{
    Assert(angle < getNumAngles());
    return c_xi[angle];
}


/* 
    getEta
    
    Returns second coordinate of angle.
*/
double Quadrature::getEta(const UINT angle) const
{
    Assert(angle < getNumAngles());
    return c_eta[angle];
}


/* 
    getMu
    
    Returns third coordinate of angle.
*/
double Quadrature::getMu(const UINT angle) const
{
    Assert(angle < getNumAngles());
    return c_mu[angle];
}


/* 
    getWt
    
    Returns weight associated with angle.
*/
double Quadrature::getWt(const UINT angle) const
{
    Assert(angle < getNumAngles());
    return c_w[angle];
}


/* 
    getOmega
    
    Returns all three coordinates of angle.
*/
std::vector<double> Quadrature::getOmega(const UINT angle) const
{
    Assert(angle < getNumAngles());
    std::vector<double> omega(3);
    omega[0] = c_xi[angle];
    omega[1] = c_eta[angle];
    omega[2] = c_mu[angle];
    return omega;
}


/*
    getNumAngles
    
    Returns the number of angles.
*/
UINT Quadrature::getNumAngles() const
{
    Assert(c_numAngles > 0);
    return c_numAngles;
}


/*
    Quadrature
    
    Creates the quadrature.
*/
Quadrature::Quadrature(const UINT snOrder)
{
    // Gauss weights/nodes
    const double *gw = NULL;
    const double *gn = NULL;
    
    
    // Get the right Gaussian data
    switch(snOrder) {
        case 2: 
            gw = s_w2;
            gn = s_n2;
            break;
        case 4: 
            gw = s_w4;
            gn = s_n4;
            break;
        case 6: 
            gw = s_w6;
            gn = s_n6;
            break;
        case 8: 
            gw = s_w8;
            gn = s_n8;
            break;
        case 10: 
            gw = s_w10;
            gn = s_n10;
            break;
        case 12: 
            gw = s_w12;
            gn = s_n12;
            break;
        case 14: 
            gw = s_w14;
            gn = s_n14;
            break;
        case 16: 
            gw = s_w16;
            gn = s_n16;
            break;
        case 18: 
            gw = s_w18;
            gn = s_n18;
            break;
        case 20: 
            gw = s_w20;
            gn = s_n20;
            break;
        case 22: 
            gw = s_w22;
            gn = s_n22;
            break;
        case 24: 
            gw = s_w24;
            gn = s_n24;
            break;
        case 26: 
            gw = s_w26;
            gn = s_n26;
            break;
        case 28: 
            gw = s_w28;
            gn = s_n28;
            break;
        case 30: 
            gw = s_w30;
            gn = s_n30;
            break;
        default:
            Insist(false, "Quadrature order not supported.");
            break;
    }
    
    
    // Allocate memory for quadrature
    c_numAngles = 2 * snOrder * snOrder;
    c_mu.resize(c_numAngles);
    c_eta.resize(c_numAngles);
    c_xi.resize(c_numAngles);
    c_w.resize(c_numAngles);
    
    
    // Create quadrature
    for(UINT i = 0; i < snOrder; i++) {
    for(UINT k = 0; k < 2 * snOrder; k++) {
        double phi = (2.0 * M_PI) / (2 * snOrder) * (k + 0.5);
        UINT index = i * 2 * snOrder + k;
        
        c_xi[index]  = sqrt(1.0 - gn[i] * gn[i]) * cos(phi);
        c_eta[index] = sqrt(1.0 - gn[i] * gn[i]) * sin(phi);
        c_mu[index]  = gn[i];
        c_w[index]   = gw[i] * (2.0 * M_PI) / (2 * snOrder);
    }}
}


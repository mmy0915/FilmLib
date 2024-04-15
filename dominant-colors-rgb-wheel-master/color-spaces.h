/*#-------------------------------------------------
#
#            Color spaces conversions
#
#    by AbsurdePhoton - www.absurdephoton.fr
#
#                v1.2 - 2020/02/06
#
#  Color spaces :
#    - RGB
#    - CIE XYZ
#    - CIE xyY
#    - CIE L*A*B* and CIE LCHab
#    - CIE L*u*v* and CIE LCHuv
#    - HSL
#    - HSV
#    - HWB
#    - Hunter Lab
#    - LMS
#    - CYMK
#
#  + RGB and CIELAb color utils
#
#-------------------------------------------------*/

#ifndef COLORSPACES
#define COLORSPACES

#include <string>

//// palette

struct struct_rgb {
    long double R; // range [0..1] - Red - display [0..255]
    long double G; // range [0..1] - Green - display [0..255]
    long double B; // range [0..1] - Blue - display [0..255]
};
struct struct_hsl {
    long double H; // range [0..1] - Hue - display [0..360]
    long double S; // range [0..1] - Saturation - display [0..100]
    long double L; // range [0..1] - Lightness - display [0..100]
    long double C; // range [0..1] - Chroma - display [0..100]
};
struct struct_hsv {
    long double H; // range [0..1] - Hue - display [0..360]
    long double S; // range [0..1] - Saturation - display [0..100]
    long double V; // range [0..1] - Value - display [0..100]
    long double C; // range [0..1] - Chroma - display [0..100]
};
struct struct_hwb {
    long double H; // range [0..1] - Hue - display [0..360]
    long double W; // range [0..1] - White - display [0..100]
    long double B; // range [0..1] - Black - display [0..100]
};
struct struct_xyz {
    long double X; // range [0..1] - mix of response curves - display [0..100]
    long double Y; // range [0..1] - luminance - display [0..100]
    long double Z; // range [0..1] - quasi-equal to blue - display [0..100]
};
struct struct_xyy {
    long double x; // range [0..1] - chromaticity - display [0..1]
    long double y; // range [0..1] - chromaticity - display [0..1]
    long double Y; // range [0..1] - luminance - display [0..100]
};
struct struct_lab {
    long double L; // range [0..1] - Lightness - display [0..100]
    long double A; // range [-1..1] - red/magenta to green - display [-128..127]
    long double B; // range [-1..1] - yellow to blue - display [-128..127]
};
struct struct_hlab {
    long double L; // range [0..1] - Lightness - display [0..100]
    long double A; // range [-1..1] - red/magenta to green - display [-100..100]
    long double B; // range [-1..1] - yellow to blue - display [-100..100]
};
struct struct_lchab {
    long double L; // range [0..1] - Lightness - display [0..100]
    long double C; // range [0..1] - Chroma - display [0..100]
    long double H; // range [0..1] - Hue - display [0..360]
};
struct struct_lchuv {
    long double L; // range [0..1] - Lightness - display [0..100]
    long double C; // range [0..1] - Chroma - display [0..100]
    long double H; // range [0..1] - Hue - display [0..360]
};
struct struct_lms {
    long double L; // range [0..1] - Long - display [0..100]
    long double M; // range [0..1] - Medium - display [0..100]
    long double S; // range [0..1] - Short - display [0..100]
};
struct struct_luv {
    long double L; // range [0..1] - Lightness - display [0..100]
    long double u; // range [0..1] - chromaticity - display [-134..220]
    long double v; // range [0..1] - chromaticity - display [-140..122]
};
struct struct_cmyk {
    long double C; // range [0..1] - Cyan - display [0..100]
    long double M; // range [0..1] - Magenta - display [0..100]
    long double Y; // range [0..1] - Yellow - display [0..100]
    long double K; // range [0..1] - Black - display [0..100]
};

struct struct_palette { // structure of a color value
    struct_rgb RGB; // all known color spaces
    struct_hsl HSL;
    struct_hsv HSV;
    struct_hwb HWB;
    struct_xyz XYZ;
    struct_xyy XYY;
    struct_lab CIELAB;
    struct_hlab HLAB;
    struct_lchab LCHAB;
    struct_lchuv LCHUV;
    struct_lms LMS;
    struct_luv LUV;
    struct_cmyk CMYK;
    std::string hexa; // RGB hexa value
    int count; // occurences
    long double percentage; // percentage
    std::string name; // name of color
    bool selected; // selection indicator
    bool visible; // visibility indicator
};

//// Distances

long double EuclidianDistanceSpace(const long double &x1, const long double &y1, const long double &z1,
                                   const long double &x2, const long double &y2, const long double &z2); // euclidian distance in 3-dimension
long double EuclidianDistancePlane(const long double &x1, const long double &y1,
                                   const long double &x2, const long double &y2); // euclidian distance in 2-dimension
long double distanceCIEDE2000LAB(const long double &L1, const long double &A1, const long double &B1,
                                 const long double &L2, const long double &A2, const long double &B2, const long double k_L, const long double k_C, const long double k_H); // distance in CIELAB space
long double DistanceFromBlackRGB(const long double &R, const long double &G, const long double &B); // CIEDE2000 distance from RGB(0,0,0)
long double DistanceFromWhiteRGB(const long double &R, const long double &G, const long double &B); // CIEDE2000 distance from RGB(1,1,1)
long double DistanceFromGrayRGB(const long double &R, const long double &G, const long double &B); // CIEDE2000 distance from nearest gray (computed in CIELAB)
long double DistanceRGB(const long double &R1, const long double &G1, const long double &B1,
                        const long double &R2, const long double &G2, const long double &B2, const long double k_L, const long double k_C, const long double k_H); // CIEDE2000 distance between 2 RGB values

//// RGB

void RGBMean(const long double &R1, const long double &G1, const long double &B1, const long double W1,
             const long double &R2, const long double &G2, const long double &B2, const long double W2,
             long double &R, long double &G, long double &B); // mean RGB value of 2 RGB values
void RGBtoStandard(const long double &r, const long double &g, const long double &b, int &R, int &G, int &B); // convert RGB [0..1] to RGB [0..255])
void GammaCorrectionToSRGB(const long double &R, const long double &G, const long double &B, long double &r, long double &g, long double &b); // Apply linear RGB gamma correction to sRGB
void GammaCorrectionFromSRGB(const long double &R, const long double &G, const long double &B, long double &r, long double &g, long double &b); // Apply linear gamma correction from sRGB
void HSLChfromRGB(const long double &R, const long double &G, const long double &B, long double &H, long double &S, long double&L, long double &C, long double &h); // get HSL values from RGB using HSL, CIELab and CIELCHab
long double PerceivedBrightnessRGB(const long double &R, const long double &G, const long double &B); // perceived brightness of RGB value
bool IsRGBColorDark(int red, int green, int blue); // is the RGB value given dark or not ?

//// Color spaces conversions

void WavelengthToXYZ(const long double &w, long double &X, long double &Y, long double &Z); // wavelength to XYZ color space
void SpectralColorToRGB(const long double &L, long double &R, long double &G, long double &B); // roughly convert wavelength value 400-700 nm to RGB [0..1]

void RGBtoHSV(const long double &R, const long double &G, const long double &B, long double& H, long double& S, long double &V, long double &C); // convert RGB value to HSV
void HSVtoRGB(const long double &H, const long double &S, const long double &V, long double &R, long double &G, long double &B); // convert HSV value to RGB
void HSVtoStandard(const long double &h, const long double &s, const long double &v, int &H, int &S, int &V); // convert HSV [0..1] to HSV H [0..359] S and V [0..100]

void RGBtoHSL(const long double &R, const long double &G, const long double &B, long double &H, long double &S, long double &L, long double &C); // convert RGB value to HSL
void HSLtoRGB(const long double &H, const long double &S, const long double &L, long double &R, long double &G, long double &B); // convert HSL value to RGB
void HSLtoStandard(const long double &h, const long double &s, const long double &l, int &H, int &S, int &L); // convert HSL [0..1] to HSL H [0..359] S and L [0..100]

void HSVtoHWB(const long double &h, const long double &s, const long double &v, long double &H, long double &W, long double &B); // convert HSV value to HWB
void HWBtoHSV(const long double &h, const long double &w, const long double &b, long double &H, long double &S, long double &V); // convert HWB vlaue to HSV
void RGBtoHWB(const long double &r, const long double &g, const long double &b, long double &H, long double &W, long double &B); // convert RGB value to HWB
void HWBtoRGB(const long double &h, const long double &w, const long double &b, long double &R, long double &G, long double &B); // convert HWB value to RGB
void HWBtoStandard(const long double &h, const long double &w, const long double &b, int &H, int &W, int &B); // convert HWB [0..1] to HWB H [0..359] W and B [0..100]

void RGBtoXYZ(const long double &R, const long double &G, const long double &B, long double &X, long double &Y, long double &Z); // convert RGB value to CIE XYZ
void XYZtoRGB(const long double &X, const long double &Y, const long double &Z, long double &R, long double &G, long double &B); // convert from XYZ to RGB
void XYZtoRGBNoClipping(const long double &X, const long double &Y, const long double &Z, long double &R, long double &G, long double &B); // convert from XYZ to RGB (in fact sRGB) without clipping to [0..1]
void XYZtoxyY(const long double &X, const long double &Y, const long double &Z, long double &x, long double &y); // convert CIE XYZ value to CIE xyY
void xyYtoXYZ(const long double &x, const long double &y, const long double &Y, long double &X, long double &Z); // convert CIE xyY value to CIE XYZ
void XYZtoStandard(const long double &x, const long double &y, const long double &z, int &X, int &Y, int &Z); // convert XYZ [0..1] to XYZ [0..100]

void XYZtoLAB(const long double &X, const long double &Y, const long double &Z, long double &L, long double &A, long double &B); // convert CIE XYZ value to CIE LAB
void LABtoXYZ(const long double &L, const long double &A, const long double &B, long double &X, long double &Y, long double &Z); // convert CIE LAB value to CIE XYZ
void LABtoStandard(const long double &l, const long double &a, const long double &b, int &L, int &A, int &B); // convert CIELab [0..1] to CIELab L [0..100] a and b [-128..127]
void LABtoLCHab(const long double &A, const long double &B, long double &C, long double &H); // convert from LAB to LCH - L is the same so no need to convert
void LCHabToLAB(const long double &C, const long double &H, long double &A, long double &B); // convert from LCH to LAB - L is the same so no need to convert
void LCHabtoStandard(const long double &l, const long double &c, const long double &h, int &L, int &C, int &H); // convert CIE LCHab [0..1] to CIE LCHab L [0..100] C [0..100+] H [0..359]

void XYZtoLuv(const long double &X, const long double &Y, const long double &Z, long double &L, long double &u, long double &v); // convert CIE XYZ value to CIE L*u*v*
void LuvToXYZ(const long double &L, const long double &u, const long double &v, long double &X, long double &Y, long double &Z); // convert CIE L*u*v* value to CIE XYZ
void LuvToStandard(const long double &l, const long double &u, const long double &v, int &L, int &U, int &V); // convert CIELab [0..1] to CIELab L [0..100] u and v [-100..100]
void LuvToLCHuv(const long double &u, const long double &v, long double &C, long double &H); // convert from Luv to LCHuv - L is the same so no need to convert
void LCHuvToLuv(const long double &C, const long double &H, long double &u, long double &v); // convert from LCHuv to LUV - L is the same so no need to convert
void LCHuvtoStandard(const long double &l, const long double &c, const long double &h, int &L, int &C, int &H); // convert CIE LCHuv [0..1] to CIE LCHuv L [0..100] C [0..100+] H [0..359]

void XYZtoHLAB(const long double &X, const long double &Y, const long double &Z, long double &L, long double &A, long double &B); // convert from XYZ to Hunter Lab
void HLABtoXYZ(const long double &L, const long double &A, const long double &B, long double &X, long double &Y, long double &Z); // convert from Hunter Lab to XYZ
void HLABtoStandard(const long double &l, const long double &a, const long double &b, int &L, int &A, int &B); // convert Hunter Lab [0..1] to Hunter Lab L [0..100] a and b [-100..100]

void XYZtoLMS(const long double &X, const long double &Y, const long double &Z, long double &L, long double &M, long double &S); // convert from XYZ to LMS

void RGBtoCMYK(const long double &R, const long double &G, const long double &B, long double &C, long double &M, long double &Y, long double &K); // convert from RGB to CMYK
void CMYKtoRGB(const long double &C, const long double &M, const long double &Y, const long double &K, long double &R, long double &G, long double &B); // convert from CMYK to RGB
void CMYKtoStandard(const long double &c, const long double &m, const long double &y, const long double &b, int &C, int &M, int &Y, int &K); // convert CMYK [0..1] to CMYK [0..100]

//// wavelength XYZ data
// wavelength value of light in nm to CIE XYZ

const int wavelength_XYZ_start = 390; // first value starts at 390nm
const int wavelength_XYZ_nb = 441; // total : 441 values
static const long double wavelength_XYZ[wavelength_XYZ_nb][4] = { // CIE 1931 2° CMF - 1nm step
    {390,0.003769647,0.0004146161,0.0184726},
    {391,0.004532416,0.0005028333,0.02221101},
    {392,0.005446553,0.0006084991,0.02669819},
    {393,0.006538868,0.0007344436,0.03206937},
    {394,0.007839699,0.0008837389,0.03847832},
    {395,0.009382967,0.001059646,0.04609784},
    {396,0.01120608,0.001265532,0.05511953},
    {397,0.01334965,0.001504753,0.06575257},
    {398,0.0158569,0.001780493,0.07822113},
    {399,0.01877286,0.002095572,0.09276013},
    {400,0.02214302,0.002452194,0.109609},
    {401,0.02601285,0.002852216,0.1290077},
    {402,0.03043036,0.003299115,0.1512047},
    {403,0.03544325,0.003797466,0.1764441},
    {404,0.0410964,0.004352768,0.2049517},
    {405,0.04742986,0.004971717,0.2369246},
    {406,0.05447394,0.005661014,0.2725123},
    {407,0.06223612,0.006421615,0.311782},
    {408,0.07070048,0.007250312,0.3547064},
    {409,0.07982513,0.008140173,0.4011473},
    {410,0.08953803,0.00907986,0.4508369},
    {411,0.09974848,0.01005608,0.5034164},
    {412,0.1104019,0.01106456,0.5586361},
    {413,0.1214566,0.01210522,0.6162734},
    {414,0.1328741,0.01318014,0.6760982},
    {415,0.1446214,0.01429377,0.7378822},
    {416,0.1566468,0.01545004,0.8013019},
    {417,0.1687901,0.01664093,0.8655573},
    {418,0.1808328,0.01785302,0.9295791},
    {419,0.1925216,0.01907018,0.9921293},
    {420,0.2035729,0.02027369,1.051821},
    {421,0.2137531,0.02144805,1.107509},
    {422,0.2231348,0.02260041,1.159527},
    {423,0.2319245,0.02374789,1.208869},
    {424,0.2403892,0.02491247,1.256834},
    {425,0.2488523,0.02612106,1.305008},
    {426,0.2575896,0.02739923,1.354758},
    {427,0.2664991,0.02874993,1.405594},
    {428,0.2753532,0.03016909,1.456414},
    {429,0.2838921,0.03165145,1.50596},
    {430,0.2918246,0.03319038,1.552826},
    {431,0.29892,0.03477912,1.595902},
    {432,0.3052993,0.03641495,1.635768},
    {433,0.3112031,0.03809569,1.673573},
    {434,0.3169047,0.03981843,1.710604},
    {435,0.3227087,0.0415794,1.74828},
    {436,0.3288194,0.04337098,1.787504},
    {437,0.3349242,0.0451718,1.826609},
    {438,0.3405452,0.0469542,1.863108},
    {439,0.3451688,0.04868718,1.894332},
    {440,0.3482554,0.05033657,1.917479},
    {441,0.3494153,0.05187611,1.930529},
    {442,0.3489075,0.05332218,1.934819},
    {443,0.3471746,0.05470603,1.93265},
    {444,0.3446705,0.05606335,1.926395},
    {445,0.3418483,0.05743393,1.918437},
    {446,0.339024,0.05885107,1.91043},
    {447,0.3359926,0.06030809,1.901224},
    {448,0.3324276,0.06178644,1.889},
    {449,0.3280157,0.0632657,1.871996},
    {450,0.3224637,0.06472352,1.848545},
    {451,0.3156225,0.06614749,1.817792},
    {452,0.3078201,0.06757256,1.781627},
    {453,0.2994771,0.06904928,1.742514},
    {454,0.2909776,0.0706328,1.702749},
    {455,0.2826646,0.07238339,1.664439},
    {456,0.2747962,0.0743596,1.629207},
    {457,0.2674312,0.07659383,1.59736},
    {458,0.2605847,0.07911436,1.568896},
    {459,0.2542749,0.08195345,1.543823},
    {460,0.2485254,0.08514816,1.522157},
    {461,0.2433039,0.08872657,1.503611},
    {462,0.2383414,0.09266008,1.486673},
    {463,0.2333253,0.09689723,1.469595},
    {464,0.2279619,0.1013746,1.450709},
    {465,0.2219781,0.1060145,1.42844},
    {466,0.2151735,0.1107377,1.401587},
    {467,0.2075619,0.1155111,1.370094},
    {468,0.1992183,0.1203122,1.33422},
    {469,0.190229,0.1251161,1.294275},
    {470,0.1806905,0.1298957,1.25061},
    {471,0.1707154,0.1346299,1.203696},
    {472,0.1604471,0.1393309,1.154316},
    {473,0.1500244,0.1440235,1.103284},
    {474,0.1395705,0.1487372,1.051347},
    {475,0.129192,0.1535066,0.9991789},
    {476,0.1189859,0.1583644,0.9473958},
    {477,0.1090615,0.1633199,0.8966222},
    {478,0.09951424,0.1683761,0.8473981},
    {479,0.0904185,0.1735365,0.8001576},
    {480,0.08182895,0.1788048,0.7552379},
    {481,0.07376817,0.1841819,0.7127879},
    {482,0.06619477,0.1896559,0.6725198},
    {483,0.0590638,0.1952101,0.6340976},
    {484,0.05234242,0.2008259,0.5972433},
    {485,0.04600865,0.2064828,0.5617313},
    {486,0.04006154,0.2121826,0.5274921},
    {487,0.03454373,0.2180279,0.4948809},
    {488,0.02949091,0.2241586,0.4642586},
    {489,0.0249214,0.2307302,0.4358841},
    {490,0.02083981,0.237916,0.4099313},
    {491,0.01723591,0.2458706,0.3864261},
    {492,0.01407924,0.2546023,0.3650566},
    {493,0.01134516,0.264076,0.3454812},
    {494,0.009019658,0.274249,0.3274095},
    {495,0.007097731,0.285068,0.3105939},
    {496,0.005571145,0.2964837,0.2948102},
    {497,0.004394566,0.308501,0.2798194},
    {498,0.003516303,0.3211393,0.26541},
    {499,0.002887638,0.3344175,0.2514084},
    {500,0.002461588,0.3483536,0.2376753},
    {501,0.002206348,0.3629601,0.2241211},
    {502,0.002149559,0.3782275,0.2107484},
    {503,0.002337091,0.3941359,0.1975839},
    {504,0.002818931,0.4106582,0.1846574},
    {505,0.003649178,0.4277595,0.1720018},
    {506,0.004891359,0.4453993,0.1596918},
    {507,0.006629364,0.4635396,0.1479415},
    {508,0.008942902,0.4821376,0.1369428},
    {509,0.01190224,0.501143,0.1268279},
    {510,0.01556989,0.5204972,0.1176796},
    {511,0.01997668,0.5401387,0.109497},
    {512,0.02504698,0.5600208,0.1020943},
    {513,0.0306753,0.5800972,0.09527993},
    {514,0.03674999,0.6003172,0.08890075},
    {515,0.04315171,0.6206256,0.08283548},
    {516,0.04978584,0.6409398,0.07700982},
    {517,0.05668554,0.6610772,0.07144001},
    {518,0.06391651,0.6808134,0.06615436},
    {519,0.07154352,0.6999044,0.06117199},
    {520,0.07962917,0.718089,0.05650407},
    {521,0.08821473,0.7351593,0.05215121},
    {522,0.09726978,0.7511821,0.04809566},
    {523,0.1067504,0.7663143,0.0443172},
    {524,0.1166192,0.7807352,0.04079734},
    {525,0.1268468,0.7946448,0.03751912},
    {526,0.137406,0.8082074,0.03446846},
    {527,0.1482471,0.8213817,0.03163764},
    {528,0.1593076,0.8340701,0.02901901},
    {529,0.1705181,0.8461711,0.02660364},
    {530,0.1818026,0.8575799,0.02438164},
    {531,0.193109,0.8682408,0.02234097},
    {532,0.2045085,0.8783061,0.02046415},
    {533,0.2161166,0.8879907,0.01873456},
    {534,0.228065,0.8975211,0.01713788},
    {535,0.2405015,0.9071347,0.01566174},
    {536,0.2535441,0.9169947,0.01429644},
    {537,0.26713,0.9269295,0.01303702},
    {538,0.2811351,0.9366731,0.01187897},
    {539,0.2954164,0.9459482,0.01081725},
    {540,0.3098117,0.9544675,0.00984647},
    {541,0.3241678,0.9619834,0.008960687},
    {542,0.3384319,0.968439,0.008152811},
    {543,0.3525786,0.9738289,0.007416025},
    {544,0.3665839,0.9781519,0.006744115},
    {545,0.3804244,0.9814106,0.006131421},
    {546,0.3940988,0.9836669,0.005572778},
    {547,0.4076972,0.9852081,0.005063463},
    {548,0.4213484,0.9863813,0.004599169},
    {549,0.4352003,0.9875357,0.004175971},
    {550,0.4494206,0.9890228,0.003790291},
    {551,0.4641616,0.9910811,0.003438952},
    {552,0.4794395,0.9934913,0.003119341},
    {553,0.495218,0.9959172,0.002829038},
    {554,0.5114395,0.9980205,0.002565722},
    {555,0.5280233,0.9994608,0.002327186},
    {556,0.5448696,0.999993,0.00211128},
    {557,0.5618898,0.9997557,0.001915766},
    {558,0.5790137,0.9989839,0.001738589},
    {559,0.5961882,0.9979123,0.00157792},
    {560,0.6133784,0.9967737,0.001432128},
    {561,0.6305897,0.9957356,0.001299781},
    {562,0.6479223,0.9947115,0.001179667},
    {563,0.6654866,0.9935534,0.001070694},
    {564,0.6833782,0.9921156,0.0009718623},
    {565,0.7016774,0.9902549,0.0008822531},
    {566,0.720411,0.9878596,0.0008010231},
    {567,0.7394495,0.9849324,0.0007273884},
    {568,0.7586285,0.9815036,0.0006606347},
    {569,0.7777885,0.9776035,0.0006001146},
    {570,0.796775,0.9732611,0.0005452416},
    {571,0.815453,0.9684764,0.0004954847},
    {572,0.8337389,0.9631369,0.0004503642},
    {573,0.8515493,0.9571062,0.0004094455},
    {574,0.8687862,0.950254,0.0003723345},
    {575,0.8853376,0.9424569,0.0003386739},
    {576,0.9011588,0.9336897,0.0003081396},
    {577,0.9165278,0.9242893,0.000280437},
    {578,0.9318245,0.9146707,0.0002552996},
    {579,0.9474524,0.9052333,0.0002324859},
    {580,0.9638388,0.8963613,0.0002117772},
    {581,0.9812596,0.8883069,0.0001929758},
    {582,0.9992953,0.8808462,0.0001759024},
    {583,1.017343,0.8736445,0.0001603947},
    {584,1.03479,0.8663755,0.0001463059},
    {585,1.051011,0.8587203,0.0001335031},
    {586,1.065522,0.8504295,0.000121866},
    {587,1.078421,0.8415047,0.0001112857},
    {588,1.089944,0.8320109,0.0001016634},
    {589,1.10032,0.8220154,0.00009291},
    {590,1.109767,0.8115868,8.494468E-05},
    {591,1.118438,0.8007874,7.769425E-05},
    {592,1.126266,0.7896515,7.109247E-05},
    {593,1.133138,0.7782053,6.507936E-05},
    {594,1.138952,0.7664733,5.960061E-05},
    {595,1.14362,0.7544785,5.460706E-05},
    {596,1.147095,0.7422473,5.005417E-05},
    {597,1.149464,0.7298229,4.590157E-05},
    {598,1.150838,0.7172525,4.211268E-05},
    {599,1.151326,0.7045818,3.865437E-05},
    {600,1.151033,0.6918553,3.549661E-05},
    {601,1.150002,0.6791009,3.26122E-05},
    {602,1.148061,0.6662846,2.997643E-05},
    {603,1.144998,0.6533595,2.756693E-05},
    {604,1.140622,0.6402807,2.536339E-05},
    {605,1.134757,0.6270066,2.334738E-05},
    {606,1.127298,0.6135148,2.150221E-05},
    {607,1.118342,0.5998494,1.981268E-05},
    {608,1.108033,0.5860682,0.000018265},
    {609,1.096515,0.5722261,1.684667E-05},
    {610,1.083928,0.5583746,1.554631E-05},
    {611,1.070387,0.5445535,1.43536E-05},
    {612,1.055934,0.5307673,1.325915E-05},
    {613,1.040592,0.517013,1.225443E-05},
    {614,1.024385,0.5032889,1.133169E-05},
    {615,1.007344,0.489595,1.048387E-05},
    {616,0.9895268,0.4759442,0},
    {617,0.9711213,0.4623958,0},
    {618,0.9523257,0.4490154,0},
    {619,0.9333248,0.4358622,0},
    {620,0.9142877,0.4229897,0},
    {621,0.8952798,0.4104152,0},
    {622,0.8760157,0.3980356,0},
    {623,0.8561607,0.38573,0},
    {624,0.8354235,0.3733907,0},
    {625,0.8135565,0.3609245,0},
    {626,0.7904565,0.348286,0},
    {627,0.7664364,0.3355702,0},
    {628,0.7418777,0.3228963,0},
    {629,0.7171219,0.3103704,0},
    {630,0.6924717,0.2980865,0},
    {631,0.66816,0.286116,0},
    {632,0.6442697,0.2744822,0},
    {633,0.620845,0.2631953,0},
    {634,0.5979243,0.2522628,0},
    {635,0.575541,0.2416902,0},
    {636,0.5537296,0.2314809,0},
    {637,0.5325412,0.2216378,0},
    {638,0.5120218,0.2121622,0},
    {639,0.492207,0.2030542,0},
    {640,0.4731224,0.1943124,0},
    {641,0.4547417,0.1859227,0},
    {642,0.4368719,0.1778274,0},
    {643,0.4193121,0.1699654,0},
    {644,0.401898,0.1622841,0},
    {645,0.3844986,0.1547397,0},
    {646,0.3670592,0.1473081,0},
    {647,0.3497167,0.1400169,0},
    {648,0.3326305,0.1329013,0},
    {649,0.3159341,0.1259913,0},
    {650,0.2997374,0.119312,0},
    {651,0.2841189,0.112882,0},
    {652,0.2691053,0.1067113,0},
    {653,0.2547077,0.1008052,0},
    {654,0.2409319,0.09516653,0},
    {655,0.2277792,0.08979594,0},
    {656,0.2152431,0.08469044,0},
    {657,0.203301,0.07984009,0},
    {658,0.1919276,0.07523372,0},
    {659,0.1810987,0.07086061,0},
    {660,0.1707914,0.06671045,0},
    {661,0.1609842,0.0627736,0},
    {662,0.1516577,0.05904179,0},
    {663,0.1427936,0.05550703,0},
    {664,0.1343737,0.05216139,0},
    {665,0.1263808,0.04899699,0},
    {666,0.1187979,0.04600578,0},
    {667,0.1116088,0.04317885,0},
    {668,0.1047975,0.04050755,0},
    {669,0.09834835,0.03798376,0},
    {670,0.09224597,0.03559982,0},
    {671,0.08647506,0.03334856,0},
    {672,0.08101986,0.03122332,0},
    {673,0.07586514,0.0292178,0},
    {674,0.07099633,0.02732601,0},
    {675,0.0663996,0.02554223,0},
    {676,0.06206225,0.02386121,0},
    {677,0.05797409,0.02227859,0},
    {678,0.05412533,0.0207902,0},
    {679,0.050506,0.01939185,0},
    {680,0.04710606,0.01807939,0},
    {681,0.04391411,0.01684817,0},
    {682,0.04091411,0.01569188,0},
    {683,0.03809067,0.01460446,0},
    {684,0.03543034,0.01358062,0},
    {685,0.03292138,0.01261573,0},
    {686,0.03055672,0.01170696,0},
    {687,0.02834146,0.01085608,0},
    {688,0.02628033,0.01006476,0},
    {689,0.02437465,0.009333376,0},
    {690,0.02262306,0.008661284,0},
    {691,0.02101935,0.008046048,0},
    {692,0.01954647,0.00748113,0},
    {693,0.01818727,0.006959987,0},
    {694,0.01692727,0.00647707,0},
    {695,0.01575417,0.006027677,0},
    {696,0.01465854,0.005608169,0},
    {697,0.01363571,0.005216691,0},
    {698,0.01268205,0.004851785,0},
    {699,0.01179394,0.004512008,0},
    {700,0.01096778,0.004195941,0},
    {701,0.01019964,0.003902057,0},
    {702,0.009484317,0.003628371,0},
    {703,0.008816851,0.003373005,0},
    {704,0.008192921,0.003134315,0},
    {705,0.00760875,0.002910864,0},
    {706,0.007061391,0.002701528,0},
    {707,0.006549509,0.002505796,0},
    {708,0.00607197,0.002323231,0},
    {709,0.005627476,0.002153333,0},
    {710,0.005214608,0.001995557,0},
    {711,0.004831848,0.001849316,0},
    {712,0.004477579,0.001713976,0},
    {713,0.004150166,0.001588899,0},
    {714,0.003847988,0.001473453,0},
    {715,0.003569452,0.001367022,0},
    {716,0.003312857,0.001268954,0},
    {717,0.003076022,0.001178421,0},
    {718,0.002856894,0.001094644,0},
    {719,0.002653681,0.001016943,0},
    {720,0.002464821,0.0009447269,0},
    {721,0.00228906,0.0008775171,0},
    {722,0.002125694,0.0008150438,0},
    {723,0.001974121,0.0007570755,0},
    {724,0.001833723,0.0007033755,0},
    {725,0.001703876,0.000653705,0},
    {726,0.001583904,0.0006078048,0},
    {727,0.001472939,0.0005653435,0},
    {728,0.001370151,0.0005260046,0},
    {729,0.001274803,0.0004895061,0},
    {730,0.001186238,0.000455597,0},
    {731,0.001103871,0.0004240548,0},
    {732,0.001027194,0.000394686,0},
    {733,0.0009557493,0.0003673178,0},
    {734,0.0008891262,0.0003417941,0},
    {735,0.0008269535,0.0003179738,0},
    {736,0.0007689351,0.0002957441,0},
    {737,0.0007149425,0.0002750558,0},
    {738,0.000664859,0.000255864,0},
    {739,0.0006185421,0.0002381142,0},
    {740,0.0005758303,0.0002217445,0},
    {741,0.0005365046,0.0002066711,0},
    {742,0.0005001842,0.0001927474,0},
    {743,0.0004665005,0.0001798315,0},
    {744,0.0004351386,0.0001678023,0},
    {745,0.0004058303,0.0001565566,0},
    {746,0.0003783733,0.0001460168,0},
    {747,0.0003526892,0.0001361535,0},
    {748,0.0003287199,0.0001269451,0},
    {749,0.0003063998,0.0001183671,0},
    {750,0.0002856577,0.0001103928,0},
    {751,0.0002664108,0.0001029908,0},
    {752,0.0002485462,9.611836E-05,0},
    {753,0.0002319529,8.973323E-05,0},
    {754,0.00021653,8.379694E-05,0},
    {755,0.0002021853,7.827442E-05,0},
    {756,0.0001888338,7.313312E-05,0},
    {757,0.0001763935,6.834142E-05,0},
    {758,0.0001647895,6.387035E-05,0},
    {759,0.0001539542,5.969389E-05,0},
    {760,0.000143827,5.578862E-05,0},
    {761,0.0001343572,5.213509E-05,0},
    {762,0.0001255141,4.872179E-05,0},
    {763,0.0001172706,4.553845E-05,0},
    {764,0.0001095983,4.257443E-05,0},
    {765,0.0001024685,3.981884E-05,0},
    {766,9.584715E-05,3.725877E-05,0},
    {767,8.968316E-05,3.487467E-05,0},
    {768,8.392734E-05,3.264765E-05,0},
    {769,7.853708E-05,3.05614E-05,0},
    {770,7.347551E-05,2.860175E-05,0},
    {771,6.871576E-05,2.675841E-05,0},
    {772,6.425257E-05,2.502943E-05,0},
    {773,6.008292E-05,2.341373E-05,0},
    {774,0.000056201,2.190914E-05,0},
    {775,5.25987E-05,2.051259E-05,0},
    {776,4.926279E-05,0.000019219,0},
    {777,4.616623E-05,0.000018018,0},
    {778,4.328212E-05,0.000016899,0},
    {779,4.058715E-05,1.585309E-05,0},
    {780,3.806114E-05,1.487243E-05,0},
    {781,3.568818E-05,1.395085E-05,0},
    {782,3.346023E-05,1.308528E-05,0},
    {783,3.13709E-05,1.227327E-05,0},
    {784,2.941371E-05,1.151233E-05,0},
    {785,2.758222E-05,0.0000108,0},
    {786,2.586951E-05,1.013364E-05,0},
    {787,0.000024267,9.509919E-06,0},
    {788,2.276639E-05,8.92563E-06,0},
    {789,2.136009E-05,8.377852E-06,0},
    {790,2.004122E-05,7.86392E-06,0},
    {791,1.88038E-05,7.381539E-06,0},
    {792,1.764358E-05,6.929096E-06,0},
    {793,1.655671E-05,6.505136E-06,0},
    {794,1.553939E-05,6.108221E-06,0},
    {795,1.458792E-05,5.736935E-06,0},
    {796,1.369853E-05,5.389831E-06,0},
    {797,1.286705E-05,5.065269E-06,0},
    {798,1.208947E-05,4.761667E-06,0},
    {799,1.136207E-05,4.477561E-06,0},
    {800,1.068141E-05,4.211597E-06,0},
    {801,1.004411E-05,3.962457E-06,0},
    {802,9.446399E-06,3.728674E-06,0},
    {803,8.884754E-06,3.508881E-06,0},
    {804,8.35605E-06,3.301868E-06,0},
    {805,7.857521E-06,3.106561E-06,0},
    {806,0.000007387,2.922119E-06,0},
    {807,6.943576E-06,2.748208E-06,0},
    {808,6.526548E-06,2.58456E-06,0},
    {809,6.135087E-06,2.430867E-06,0},
    {810,5.768284E-06,2.286786E-06,0},
    {811,5.425069E-06,2.151905E-06,0},
    {812,0.000005104,2.025656E-06,0},
    {813,4.803525E-06,1.907464E-06,0},
    {814,4.52235E-06,1.796794E-06,0},
    {815,4.259166E-06,1.693147E-06,0},
    {816,4.012715E-06,0.000001596,0},
    {817,3.781597E-06,1.504903E-06,0},
    {818,3.564496E-06,1.419245E-06,0},
    {819,3.360236E-06,1.3386E-06,0},
    {820,3.167765E-06,1.262556E-06,0},
    {821,2.986206E-06,1.190771E-06,0},
    {822,0.000002815,0.000001123,0},
    {823,2.653663E-06,1.059151E-06,0},
    {824,2.501725E-06,0.000000999,0},
    {825,2.358723E-06,9.422514E-07,0},
    {826,2.224206E-06,8.888804E-07,0},
    {827,2.097737E-06,8.38669E-07,0},
    {828,1.978894E-06,7.914539E-07,0},
    {829,1.867268E-06,7.47077E-07,0},
    {830,1.762465E-06,7.05386E-07,0}
};

#endif // COLORSPACES

#include "stdio.h"
#include <fstream>
#include <opencv2/opencv.hpp>

#include "mat-image-tools.h"
#include "dominant-colors.h"
#include "color-spaces.h"
#include "angles.h"


struct struct_palette { // structure of a color value
	int R; // RGB in [0..55]
	int G;
	int B;
	long double H, S, L, C, h; // in [0..1]
	long double distanceBlack, distanceWhite, distanceGray; // distance from gray values
	string hexa; // hexadecimal RGB
	int count; // number of pixels of this RGB color in image
	float percentage; // percentge of use in image
	string name; // color name

	struct_palette(int _R, int _G,int _B, long double _H, long double _S, long double _L, long double _C, long double _h, long double _distanceBlack, 
		long double _distanceWhite, long double _distanceGray , string _hexa, int _count, float _percentage, string _name) : R(_R), G(_G), B(_B), H(_H), S(_S), L(_L), C(_C), h(_h), distanceBlack(_distanceBlack), distanceWhite(_distanceWhite), distanceGray(_distanceGray), hexa(_hexa), count(_count), percentage(_percentage), name(_name) {}
};

void GammaCorrectionToSRGB(const long double& R, const long double& G, const long double& B, long double& r, long double& g, long double& b) // Apply linear RGB gamma correction to sRGB
{
    // Gamma correction - conversion to linear space - source http://www.brucelindbloom.com/index.html?Eqn_RGB_to_XYZ.html
    if (R > 0.04045)
        r = powl((R + 0.055) / 1.055, 2.4);
    else
        r = R / 12.92;
    if (G > 0.04045)
        g = powl((G + 0.055) / 1.055, 2.4);
    else
        g = G / 12.92;
    if (B > 0.04045)
        b = powl((B + 0.055) / 1.055, 2.4);
    else
        b = B / 12.92;
}

void RGBtoXYZ(const long double& R, const long double& G, const long double& B, long double& X, long double& Y, long double& Z) // convert RGB (in fact sRGB) value to CIE XYZ
{ // function checked OK with other calculators
    long double r, g, b;

    GammaCorrectionToSRGB(R, G, B, r, g, b); // gamma correction to linear sRGB

    // Gammut conversion to sRGB - source http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
    X = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
    Y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
    Z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;
}

void XYZtoLAB(const long double& X, const long double& Y, const long double& Z, long double& L, long double& A, long double& B) // convert CIE XYZ value to CIE L*A*B*
{ // function checked OK with other calculators
    // reference white in XYZ
    long double ref_X = 0.95047L;
    long double ref_Y = 1.0L;
    long double ref_Z = 1.08883L;
    // CIE values
    long double E = 216.0L / 24389.0L;
    long double K = 24389.0L / 27.0L;

    long double Xr = X / ref_X;
    long double Yr = Y / ref_Y;
    long double Zr = Z / ref_Z;

    long double fX, fY, fZ;
    if (Xr > E)
        fX = powl(Xr, 1.0L / 3.0L);
    else
        fX = (K * Xr + 16.0L) / 116.0L;
    if (Yr > E)
        fY = powl(Yr, 1.0L / 3.0L);
    else
        fY = (K * Yr + 16.0L) / 116.0L;
    if (Zr > E)
        fZ = powl(Zr, 1.0L / 3.0L);
    else
        fZ = (K * Zr + 16.0L) / 116.0L;

    L = 116.0L * fY - 16.0L;
    A = 500.0L * (fX - fY);
    B = 200.0L * (fY - fZ);

    L = L / 100.0L; // to stay in [0..1] range
    A = A / 127.0L;
    B = B / 127.0L;
}

void HSVtoStandard(const long double& h, const long double& s, const long double& v, int& H, int& S, int& V) // convert HSV [0..1] to HSV H [0..359] S and V [0..100]
{
    H = round(h * 360.0L);
    S = round(s * 100.0L);
    V = round(v * 100.0L);
}

long double distanceCIEDE2000LAB(const long double& L1, const long double& A1, const long double& B1,
    const long double& L2, const long double& A2, const long double& B2,
    const long double k_L, const long double k_C, const long double k_H) // distance in CIELAB space using last and best formula (CIEDE2000) compared to CIE94 and CIE76
{
    // Adapted from Gregory Fiumara
    // source : http://github.com/gfiumara/CIEDE2000
    // Based on "The CIEDE2000 Color-Difference Formula: Implementation Notes, Supplementary Test Data, and Mathematical Observations" by Gaurav Sharma, Wencheng Wu, and Edul N. Dalal
    // Results checked against all article values page 24 : OK
    // "For these and all other numerical/graphical delta E00 values reported in this article, we set the parametric weighting factors to unity(i.e., k_L = k_C = k_H = 1.0)." (p27)
    // This function is SLOW, for speed use euclidian distance

    // Step 0 : convert values to correct ranges : L, a and b must be in [0..100]
    const long double l1 = L1 * 100.0;
    const long double l2 = L2 * 100.0;
    const long double a1 = A1 * 100.0;
    const long double a2 = A2 * 100.0;
    const long double b1 = B1 * 100.0;
    const long double b2 = B2 * 100.0;

    // Remark : all angles in equations are expressed in degrees, but atan, cos and sin need radians !

    // Step 1
    // equation 2 : chromas
    long double C1 = sqrtl((a1 * a1) + (b1 * b1));
    long double C2 = sqrtl((a2 * a2) + (b2 * b2));
    // equation 3 : chromas mean
    long double barC = (C1 + C2) / 2.0;
    // equation 4 : G
    long double G = 0.5 * (1 - sqrtl(powl(barC, 7) / (powl(barC, 7) + powl(25, 7))));
    // equation 5 : a
    long double a1Prime = (1.0 + G) * a1;
    long double a2Prime = (1.0 + G) * a2;
    // equation 6 : C' from LCH
    long double CPrime1 = sqrtl((a1Prime * a1Prime) + (b1 * b1));
    long double CPrime2 = sqrtl((a2Prime * a2Prime) + (b2 * b2));
    // equation 7 : H' from LCH
    long double hPrime1;
    if (b1 == 0 && a1Prime == 0)
        hPrime1 = 0.0;
    else {
        hPrime1 = atan2l(b1, a1Prime);
        if (hPrime1 < 0)
            hPrime1 += Pi * 2.0;
        hPrime1 = hPrime1 * 180.0 / Pi;
    }

    long double hPrime2;
    if (b2 == 0 && a2Prime == 0)
        hPrime2 = 0.0;
    else {
        hPrime2 = atan2l(b2, a2Prime);
        if (hPrime2 < 0)
            hPrime2 += Pi * 2.0;
        hPrime2 = hPrime2 * 180.0 / Pi;
    }

    // Step 2
    // equation 8 : delta L
    long double deltaLPrime = l2 - l1;
    // equation 9 : delta C'
    long double deltaCPrime = CPrime2 - CPrime1;
    // equation 10 : delta h'
    long double deltahPrime;
    long double CPrimeProduct = CPrime1 * CPrime2;

    if (CPrimeProduct == 0)
        deltahPrime = 0;
    else {
        deltahPrime = hPrime2 - hPrime1;
        if (deltahPrime < -180)
            deltahPrime += 360.0;
        else if (deltahPrime > 180)
            deltahPrime -= 360.0;
    }
    // Equation 11 : delta H'
    long double deltaHPrime = 2.0 * sqrtl(CPrimeProduct) * sinl(Angle::DegToRad(deltahPrime) / 2.0);

    // Step 3
    // equation 12 : L mean
    long double barLPrime = (l1 + l2) / 2.0;
    // equation 13 : C mean
    long double barCPrime = (CPrime1 + CPrime2) / 2.0;
    // equation 14 : bar h'
    long double barhPrime, hPrimeSum = hPrime1 + hPrime2;

    if (CPrime1 * CPrime2 == 0) {
        barhPrime = hPrimeSum;
    }
    else {
        if (abs(hPrime1 - hPrime2) <= 180)
            barhPrime = hPrimeSum / 2.0;
        else {
            if (hPrimeSum < 360)
                barhPrime = (hPrimeSum + 360) / 2.0;
            else
                barhPrime = (hPrimeSum - 360) / 2.0;
        }
    }
    // equation 15 : T
    long double T = 1.0 - (0.17 * cosl(Angle::DegToRad(barhPrime) - Angle::DegToRad(30.0))) +
        (0.24 * cosl(2.0 * Angle::DegToRad(barhPrime))) +
        (0.32 * cosl((3.0 * Angle::DegToRad(barhPrime)) + Angle::DegToRad(6.0))) -
        (0.20 * cosl((4.0 * Angle::DegToRad(barhPrime)) - Angle::DegToRad(63.0)));
    // equation 16 : delta theta
    long double deltaTheta = Angle::DegToRad(30.0) * expl(-powl((Angle::DegToRad(barhPrime) - Angle::DegToRad(275.0)) / Angle::DegToRad(25.0), 2.0));
    // equation 17
    long double R_C = 2.0 * sqrtl(pow(barCPrime, 7.0) / (powl(barCPrime, 7.0) + powl(25, 7)));
    // equation 18
    long double S_L = 1 + ((0.015 * powl(barLPrime - 50.0, 2.0)) / sqrtl(20 + powl(barLPrime - 50.0, 2.0)));
    // equation 19
    long double S_C = 1 + (0.045 * barCPrime);
    // equation 20
    long double S_H = 1 + (0.015 * barCPrime * T);
    // equation 21
    long double R_T = (-sinl(2.0 * deltaTheta)) * R_C;

    // equation 22 : delta E (distance)
    long double deltaE = sqrtl(powl(deltaLPrime / (k_L * S_L), 2.0) +
        powl(deltaCPrime / (k_C * S_C), 2.0) +
        powl(deltaHPrime / (k_H * S_H), 2.0) +
        (R_T * (deltaCPrime / (k_C * S_C)) * (deltaHPrime / (k_H * S_H))));

    return (deltaE);
}

void FindColorName(const int& n_palette) // find color name for one palette item
{
	bool found = false;
	long double distance = 1000000; // distance from nearest color
	int index = 0; // to keep nearest color index in color names table

	for (int c = 0; c < nb_color_names; c++) { // search in color names table
		if ((palettes[n_palette].R == color_names[c].R) && (palettes[n_palette].G == color_names[c].G) && (palettes[n_palette].B == color_names[c].B)) { // same RGB values found
			palettes[n_palette].name = color_names[c].name; // assign color name to color in palette
			found = true; // color found in color names database
			break; // get out of loop
		}
		else { // exact color not found
			long double d = DistanceRGB((long double)(palettes[n_palette].R) / 255.0, (long double)(palettes[n_palette].G) / 255.0, (long double)(palettes[n_palette].B) / 255.0,
				(long double)(color_names[c].R) / 255.0, (long double)(color_names[c].G) / 255.0, (long double)(color_names[c].B) / 255.0,
				1.0, 0.5, 1.0); // CIEDE2000 distance with emphasis on Lightness
			if (d < distance) { // if distance is smaller than before
				distance = d; // new distance
				index = c; // keep index
			}
		}
	}
	if (!found) // picked color not found in palette so display nearest color
		palettes[n_palette].name = color_names[index].name; // assign color name
}

void RGBtoStandard(const long double& r, const long double& g, const long double& b, int& R, int& G, int& B) // convert RGB [0..1] to RGB [0..255]
{
    R = round(r * 255.0L);
    G = round(g * 255.0L);
    B = round(b * 255.0L);
}

void RGBtoHSV(const long double& R, const long double& G, const long double& B, long double& H, long double& S, long double& V, long double& C) // convert RGB value to HSV+C
{ // function checked OK with other calculators
    long double cmax = std::max(std::max(R, G), B);    // maximum of RGB
    long double cmin = std::min(std::min(R, G), B);    // minimum of RGB
    long double diff = cmax - cmin;       // diff of cmax and cmin.

    if (diff > 0) { // not particular case of diff = 0 -> find if R G or B is dominant
        if (cmax == R) // R is dominant
            H = 60.0 * (fmod(((G - B) / diff), 6)); // compute H
        else if (cmax == G) // G is dominant
            H = 60 * (((B - R) / diff) + 2); // compute H
        else if (cmax == B) // B is dominant
            H = 60 * (((R - G) / diff) + 4); // compute H

        if (cmax > 0) // compute S
            S = diff / cmax;
        else
            S = 0;

        V = cmax; // compute V
    }
    else { // particular case -> H = red (convention)
        H = 0;
        S = 0;
        V = cmax;
    }

    if (H < 0) // H must be in [0..360] range
        H += 360;
    if (H >= 360)
        H -= 360;

    // Final results are in range [0..1]
    H = H / 360.0; // was in degrees
    C = diff; // chroma
}

void SavePalette(const struct_palette& palette)
{
	CString  strPathName;
	GetModuleFileNameA(NULL, strPathName.GetBuffer(256), 256);
	strPathName.ReleaseBuffer(256);
	int nPos = strPathName.ReverseFind(_T('\\'));
	strPathName = strPathName.Left(nPos + 1);
	CString app_workpath = strPathName + _T("\\WORK\\DATA\\");

	nb_color_names = -1;
	// read color names from .csv file
	std::string line; // line to read in text file
	std::ifstream names; // file to read
	names.open(app_workpath + _T("color-names.csv")); // read color names file

	if (names) { // if successfully read
		nb_color_names = -1; // index of color names array
		size_t pos; // index for find function
		std::string s; // used for item extraction
		getline(names, line); // read first line (header)
		while (getline(names, line)) { // read each line of text file: R G B name
			pos = 0; // find index at the beginning of the line
			nb_color_names++; // current index in color names array
			int pos2 = line.find(";", pos); // find first semicolon char
			s = line.substr(pos, pos2 - pos); // extract R value
			color_names[nb_color_names].R = std::stoi(s); // R in array
			pos = pos2 + 1; // next char
			pos2 = line.find(";", pos); // find second semicolon char
			s = line.substr(pos, pos2 - pos); // extract G value
			color_names[nb_color_names].G = std::stoi(s); // G in array
			pos = pos2 + 1; // next char
			pos2 = line.find(";", pos); // find third semicolon char
			s = line.substr(pos, pos2 - pos); // extract B value
			color_names[nb_color_names].B = std::stoi(s); // B in array
			s = line.substr(pos2 + 1, line.length() - pos2); // color name is at the end of the line
			color_names[nb_color_names].name = s; // color name in array
		}

		names.close(); // close text file
	}

	//palette .CSV file (text)
	std::ofstream saveCSV; // file to save
	saveCSV.open(app_workpath + _T("-palette.csv")); // save palette file
	if (saveCSV) { // if successfully open
		// CSV header
		saveCSV << "Name (string);RGB.R [0..255];RGB.G [0..255];RGB.B [0..255];RGB.hexa (string);percentage [0..100];sRGB.R [0..255];sRGB.G [0..255];sRGB.B [0..255];";
		saveCSV << "HSV.H [0..359];HSV.S [0..100];HSV.V [0..100];HSL.H [0..359];HSL.S [0..100];HSL.L [0..100];";
		saveCSV << "XYZ.X [0..100];XYZ.Y [0..100];XYZ.Z [0..100];Lab.L [0..100];Lab.a [-128..127];Lab.b [-128..127];LCHab.L [0..100];LCHab.C [0..100+];LCHab.H [0..359];";
		saveCSV << "CMYK.C [0..100];CMYK.M [0..100];CMYK.Y [0..100];CMYK.K [0..100]";
		saveCSV << "\n"; // header
		// palette
		for (int n = 0; n < nb_palettes; n++) { // read entire palette
			if (palettes[n].name == "Not computed") // if the color name was not computed
				FindColorName(n); // find color name
			saveCSV << palettes[n].name << ";"; // color name
			saveCSV << palettes[n].R << ";"; // save RGB values
			saveCSV << palettes[n].G << ";";
			saveCSV << palettes[n].B << ";";
			saveCSV << palettes[n].hexa << ";"; // save hexa
			saveCSV << round(palettes[n].percentage * 100.0); // save percentage
			long double X, Y, Z, L, A, B, H, S, V, C, M, K, R, G;
			int x, y, z, l, a, b, h, s, v, c, m, k, r, g;
			GammaCorrectionToSRGB(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0, R, G, B);
			RGBtoStandard(R, G, B, r, g, b);
			saveCSV << ";" << r << ";" << g << ";" << b; // sRGB
			RGBtoHSV(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0, H, S, V, C);
			HSVtoStandard(H, S, V, h, s, v);
			saveCSV << ";" << h << ";" << s << ";" << v; // HSV
			RGBtoHSL(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0, H, S, L, C);
			HSLtoStandard(H, S, L, h, s, l);
			saveCSV << ";" << h << ";" << s << ";" << l; // HSL
			RGBtoXYZ(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0, X, Y, Z);
			XYZtoStandard(X, Y, Z, x, y, z);
			saveCSV << ";" << x << ";" << y << ";" << z; // XYZ
			XYZtoLAB(X, Y, Z, L, A, B);
			LABtoStandard(L, A, B, l, a, b);
			saveCSV << ";" << l << ";" << a << ";" << b; // CIELab
			LABtoLCHab(A, B, C, H);
			LCHabtoStandard(L, C, H, l, c, h);
			saveCSV << ";" << l << ";" << c << ";" << h; // CIELCh
			RGBtoCMYK(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0, C, M, Y, K);
			CMYKtoStandard(C, M, Y, K, c, m, y, k);
			saveCSV << ";" << c << ";" << m << ";" << y << ";" << k; // CMYK
			saveCSV << "\n";
		}
		saveCSV.close(); // close text file
	}
    std::cout << "CSV saved successfully." << std::endl;

    //save palettes to jpg
    std::string filename = "output.jpg";

    // 可选地设置 JPG 压缩质量（范围 0 到 100，100 表示最佳质量）
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95); // 设置质量为 95

    // 使用 imwrite 函数保存图像
    bool success = cv::imwrite(filename, image, compression_params);

    if (success) {
        std::cout << "Image saved successfully." << std::endl;
    }
    else {
        std::cout << "Error saving the image." << std::endl;
    }
}

void ComputePaletteValues(const int n) // compute palette values from RGB for one color
{
    HSLChfromRGB((long double)(palettes[n].R / 255.0), (long double)(palettes[n].G / 255.0), (long double)(palettes[n].B / 255.0),
        palettes[n].H, palettes[n].S, palettes[n].L, palettes[n].C, palettes[n].h); // get H, S and L

    // hexadecimal value
    QString hex;
    if (palettes[n].R == -1) // not a plaette color, set it to 000000 because some filters use this to take out this color
        hex = "000000";
    else
        hex = QString(" %1").arg(((palettes[n].R & 0xff) << 16)
            + ((palettes[n].G & 0xff) << 8)
            + (palettes[n].B & 0xff)
            , 6, 16, QChar('0')).trimmed(); // compute hexa RGB value
    hex = "#" + hex; // add a hash character
    palettes[n].hexa = hex.toUpper().toUtf8().constData(); // save hex value

    // distances to black, white and gray points, computed with CIEDE2000 distance algorithm
    palettes[n].distanceBlack = DistanceFromBlackRGB(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0);
    palettes[n].distanceWhite = DistanceFromWhiteRGB(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0);
    palettes[n].distanceGray = DistanceFromGrayRGB(palettes[n].R / 255.0, palettes[n].G / 255.0, palettes[n].B / 255.0);
}

int CountRGBUniqueValues(const cv::Mat& image) // count number of RGB colors in image
{
    std::set<int> unique;

    for (Vec3b& p : cv::Mat_<Vec3b>(image)) { // iterate over pixels (assummes CV_8UC3 !)
        int n = (p[0] << 16) | (p[1] << 8) | (p[2]); // "hash" representation of the pixel
        unique.insert(n);
    }

    return unique.size();
}

void SortPalettes() // sort palette values and create palette image
{
    if (nb_palettes < 2) // only one color in palette, no need to sort it !
        return;

    std::sort(palettes, palettes + nb_palettes,
        [](const struct_palette& a, const struct_palette& b) {return a.percentage > b.percentage; });

    //// sort by type
    //if (ui->comboBox_sort->currentText() == "Percentage")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.percentage > b.percentage; });
    //else if (ui->comboBox_sort->currentText() == "Lightness")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.L < b.L; });
    //else if (ui->comboBox_sort->currentText() == "Hue (HSL)")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.H < b.H; });
    //else if (ui->comboBox_sort->currentText() == "Hue (CIE LCHab)")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.h < b.h; });
    //else if (ui->comboBox_sort->currentText() == "Saturation")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.S < b.S; });
    //else if (ui->comboBox_sort->currentText() == "Chroma")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.C < b.C; });
    //else if (ui->comboBox_sort->currentText() == "RGB (hexa)")
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.hexa < b.hexa; });
    //else if (ui->comboBox_sort->currentText() == "Rainbow6") // Hue + Luma
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return int(a.H * 60.0) + sqrt(0.241 * a.R + 0.691 * a.G + 0.068 * a.B) < int(b.H * 60.0) + sqrt(0.241 * b.R + 0.691 * b.G + 0.068 * b.B); });
}

void ComputePaletteImage() // compute palette image from palettes values
{
    // compute percentages
    int total = 0; // total number of pixels
    for (int n = 0; n < nb_palettes; n++) { // for each color in palette
        cv::Mat1b mask; // current color mask
        cv::inRange(quantized, cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R),
            cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R),
            mask); // create mask for current color
        palettes[n].count = cv::countNonZero(mask); // count pixels in this mask
        total += palettes[n].count; // increase total number of pixels
    }
    for (int n = 0; n < nb_palettes; n++) // for each color in palette
        palettes[n].percentage = (long double)(palettes[n].count) / (long double)(total); // compute color percentage in palette

    SortPalettes(); // sort palette by type from GUI

    // create palette image
    palette = cv::Mat::zeros(cv::Size(palette_width, palette_height), CV_8UC3); // create blank image
    double offset = 0; // current x position in palette
    for (int n = 0; n < nb_palettes; n++) { // for each color in palette
        cv::rectangle(palette, cv::Rect(round(offset), 0,
            round(palettes[n].percentage * double(palette_width)), palette_height),
            cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R), -1); // rectangle of current color
        offset += round(palettes[n].percentage * double(palette_width)); // next x position in palette
        //if (ui->checkBox_palette_scale->isChecked()) { // use percentage scale ?
        //    cv::rectangle(palette, cv::Rect(round(offset), 0,
        //        round(palettes[n].percentage * double(palette_width)), palette_height),
        //        cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R), -1); // rectangle of current color
        //    offset += round(palettes[n].percentage * double(palette_width)); // next x position in palette
        //}
        //else { // draw without scale
        //    cv::rectangle(palette, cv::Rect(round(offset), 0,
        //        round(double(palette_width) / nb_palettes), palette_height),
        //        cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R), -1); // rectangle of current color
        //    offset += round(double(palette_width) / nb_palettes); // next x position in palette
        }
    }
}

long double DistanceRGB(const long double& R1, const long double& G1, const long double& B1,
    const long double& R2, const long double& G2, const long double& B2,
    const long double k_L, const long double k_C, const long double k_H) // CIEDE2000 distance between 2 RGB values
{
    long double X, Y, Z, L1, a1, b1, L2, a2, b2;
    RGBtoXYZ(R1, G1, B1, X, Y, Z); // convert RGB to XYZ
    XYZtoLAB(X, Y, Z, L1, a1, b1); // convert XYZ to CIELab
    RGBtoXYZ(R2, G2, B2, X, Y, Z); // same for 2nd RGB value
    XYZtoLAB(X, Y, Z, L2, a2, b2);
    return distanceCIEDE2000LAB(L1, a1, b1, L2, a2, b2, k_L, k_C, k_H); // CIEDE2000 distance
}

void Compute(cv::Mat image) // analyze image dominant colors
{
    //if (!loaded) { // nothing loaded yet = get out
    //    return;
    //}

    /*QApplication::setOverrideCursor(Qt::WaitCursor); // wait cursor
    timer.start(); // reinit timer
    ShowTimer(true); // show it
    qApp->processEvents();*/

    cv::Mat imageCopy; // work on a copy of the image, because gray colors can be filtered
    image.copyTo(imageCopy);

    long double H, S, L;
    /*if (ui->checkBox_filter_grays->isChecked()) { // filter whites, blacks and grays if gray filter is set
        cv::Vec3b RGB;
        for (int x = 0; x < imageCopy.cols; x++) // parse temp image
            for (int y = 0; y < imageCopy.rows; y++) {
                RGB = imageCopy.at<cv::Vec3b>(y, x); // current pixel color
                long double C, h;
                HSLChfromRGB(double(RGB[2] / 255.0), double(RGB[1] / 255.0), double(RGB[0] / 255.0), H, S, L, C, h); // get HSL values
                long double dBlack = DistanceFromBlackRGB((long double)(RGB[2]) / 255.0, (long double)(RGB[1]) / 255.0, (long double)(RGB[0]) / 255.0); // compute distances from black, white and gray points
                long double dWhite = DistanceFromWhiteRGB((long double)(RGB[2]) / 255.0, (long double)(RGB[1]) / 255.0, (long double)(RGB[0]) / 255.0);
                long double dGray = DistanceFromGrayRGB((long double)(RGB[2]) / 255.0, (long double)(RGB[1]) / 255.0, (long double)(RGB[0]) / 255.0);

                if ((dGray < graysLimit) or (dBlack < blacksLimit) or (dWhite < whitesLimit)) // white or black or gray pixel ?
                    imageCopy.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0); // replace it with black in temp image
            }
    }*/

    //nb_palettes = ui->spinBox_nb_palettes->value(); // how many dominant colors
    nb_palettes = 4
    int nb_palettes_asked = nb_palettes; // save asked number of colors for later
    //ui->spinBox_nb_palettes->setStyleSheet("QSpinBox{color:black;background-color: white;}"); // show number of colors in black (in case it was red before)

    //if (ui->checkBox_filter_grays->isChecked()) { // if grays and blacks and whites are filtered
    //    cv::Mat1b black_mask;
    //    cv::inRange(imageCopy, cv::Vec3b(0, 0, 0), cv::Vec3b(0, 0, 0), black_mask); // extract black pixels from image (= whites and blacks and grays)
    //    if ((cv::sum(black_mask) != cv::Scalar(0, 0, 0))) // image contains black pixels ?
    //        nb_palettes++; // add one color to asked number of colors in palette, to remove it later and only get colors
    //}

    // set all palette values to dummy values
    for (int n = 0; n < nb_palettes; n++) {
        palettes[n].R = -1;
        palettes[n].G = -1;
        palettes[n].B = -1;
        palettes[n].count = -1;
        palettes[n].percentage = -1;
        palettes[n].distanceBlack = 0;
        palettes[n].distanceWhite = 100;
        palettes[n].distanceGray = 100;
        palettes[n].name = "Not computed";
    }

    int totalMean = 0; // number of colors obtained with Mean algorithms (mean-shift and sectored-means)

    cv::Mat1f colors; // store palette from K-means
        quantized = dominantcolorskmeanscielab(imagecopy, nb_palettes, colors); // get quantized image and palette

        // palette from quantized image
   cv::vec3b color[nb_palettes]; // temp palette
   int nbcolor = 0; // current color
   for (int x = 0; x < quantized.cols; x++) // parse entire image
        for (int y = 0; y < quantized.rows; y++) {
            cv::vec3b col = quantized.at<cv::vec3b>(y, x); // current pixel color
            bool found = false;
            for (int i = 0; i < nbcolor; i++) // look into temp palette
                if (col == color[i]) { // if color already exists
                    found = true; // found, don't add it
                    break;
                }
                if (!found) { // color not already in temp palette
                    color[nbcolor] = col; // save new color
                    palettes[nbcolor].r = col[2]; // copy rgb values to global palette
                    palettes[nbcolor].g = col[1];
                    palettes[nbcolor].b = col[0];
                    nbcolor++; // one more color
                }
         }
 
    //bool isChecked_mean_shift = isChecked_eigen_vectors = isChecked_k_means = isChecked_sectored_means = False;
    //if (isChecked_mean_shift) { // mean-shift algorithm checked : intermediate number of colors unknown
    //    cv::Mat temp = ImgRGBtoLab(imageCopy); // convert image to CIELab
    //    MeanShift MSProc(ui->horizontalSlider_mean_shift_spatial->value(), ui->horizontalSlider_mean_shift_color->value()); // create instance of Mean-shift
    //    MSProc.MeanShiftFilteringCIELab(temp); // Mean-shift filtering
    //    MSProc.MeanShiftSegmentationCIELab(temp); // Mean-shift segmentation
    //    quantized = ImgLabToRGB(temp); // convert image back to RGB

    //    // palette from quantized image
    //    struct struct_colors { // color index and count
    //        cv::Vec3b RGB;
    //        int count;
    //    };
    //    int nb_count = CountRGBUniqueValues(quantized); // number of colors in quantized image : we don't know how many
    //    struct_colors color[nb_count]; // temp palette

    //    int nbColor = 0;
    //    for (int x = 0; x < quantized.cols; x++) // parse quantized image
    //        for (int y = 0; y < quantized.rows; y++) {
    //            cv::Vec3b col = quantized.at<cv::Vec3b>(y, x); // current pixel
    //            bool found = false; // indicates if color was known
    //            for (int i = 0; i < nbColor; i++) // parse colors index
    //                if (col == color[i].RGB) { // if color already registered
    //                    found = true; // we found it !
    //                    color[i].count++; // add 1 pixel to the count of this color
    //                    break; // stop
    //                }
    //            if (!found) { // if color was not found
    //                color[nbColor].RGB = col; // add it to the index
    //                color[nbColor].count = 1; // 1 pixel found for the moment
    //                nbColor++; // increase number of colors found
    //            }
    //        }
    //    std::sort(color, color + nbColor,
    //        [](const struct_colors& a, const struct_colors& b) {return a.count > b.count; }); // sort colors by count, descending

    //    int total = quantized.rows * quantized.cols; // number of pixels in image
    //    // clean insignificant colors by percentage
    //    while ((nbColor > 1) and (double(color[nbColor - 1].count) / total < 0.005)) // is the last color percentage an insignificant value ?
    //        nbColor--; // one less color to consider

    //    if (nbColor > nb_count) // number of asked colors could be inferior to the real number of colors in quantized image
    //        nbColor = nb_count;
    //    nb_palettes = nbColor; // real number of colors in palette

    //    for (int n = 0; n < nbColor; n++) { // for all colors in Mean-shift palette
    //        palettes[n].R = color[n].RGB[2]; // copy RGB values to global palette
    //        palettes[n].G = color[n].RGB[1];
    //        palettes[n].B = color[n].RGB[0];
    //        totalMean += color[n].count; // compute total number of pixels for this color
    //    }
    //}
    //else if (isChecked_eigen_vectors) { // eigen method : number of colors known from the start
    //    cv::Mat conv = ImgRGBtoLab(imageCopy); // convert image to CIELab
    //    cv::Mat result;
    //    std::vector<cv::Vec3f> temp;
    //    temp = DominantColorsEigenCIELab(conv, nb_palettes, result); // get dominant palette, palette image and quantized image

    //    quantized = ImgLabToRGB(result); // convert Quantized back to RGB

    //    // palette from quantized image
    //    cv::Vec3b color[nb_palettes]; // temp palette
    //    int nbColor = 0; // current color
    //    for (int x = 0; x < quantized.cols; x++) // parse entire image
    //        for (int y = 0; y < quantized.rows; y++) {
    //            cv::Vec3b col = quantized.at<cv::Vec3b>(y, x); // current pixel color
    //            bool found = false;
    //            for (int i = 0; i < nbColor; i++) // look into temp palette
    //                if (col == color[i]) { // if color already exists
    //                    found = true; // found, don't add it
    //                    break;
    //                }
    //            if (!found) { // color not already in temp palette
    //                color[nbColor] = col; // save new color
    //                palettes[nbColor].R = col[2]; // copy RGB values to global palette
    //                palettes[nbColor].G = col[1];
    //                palettes[nbColor].B = col[0];
    //                nbColor++; // one more color
    //            }
    //        }
    //}
    //else if (isChecked_k_means) { // K-means algorithm : number of colors known from the start
    //    cv::Mat1f colors; // store palette from K-means
    //    quantized = DominantColorsKMeansCIELAB(imageCopy, nb_palettes, colors); // get quantized image and palette

    //    // palette from quantized image
    //    cv::Vec3b color[nb_palettes]; // temp palette
    //    int nbColor = 0; // current color
    //    for (int x = 0; x < quantized.cols; x++) // parse entire image
    //        for (int y = 0; y < quantized.rows; y++) {
    //            cv::Vec3b col = quantized.at<cv::Vec3b>(y, x); // current pixel color
    //            bool found = false;
    //            for (int i = 0; i < nbColor; i++) // look into temp palette
    //                if (col == color[i]) { // if color already exists
    //                    found = true; // found, don't add it
    //                    break;
    //                }
    //            if (!found) { // color not already in temp palette
    //                color[nbColor] = col; // save new color
    //                palettes[nbColor].R = col[2]; // copy RGB values to global palette
    //                palettes[nbColor].G = col[1];
    //                palettes[nbColor].B = col[0];
    //                nbColor++; // one more color
    //            }
    //        }
    //}
    //else if (isChecked_sectored_means) { // sectored-means : intermediate number of colors unknown
    //    if (ui->checkBox_sectored_means_levels->isChecked()) // choice of Chroma and Lightness levels ?
    //        SectoredMeansSegmentationLevels(imageCopy, ui->horizontalSlider_sectored_means_levels->value(), quantized); // get sectored-means quantized with choice of levels
    //    else
    //        SectoredMeansSegmentationCategories(imageCopy, quantized); // get sectored-means quantized without choice of levels

    //    // palette from quantized image
    //    struct struct_colors { // sgtruct for color index and count
    //        cv::Vec3b RGB;
    //        int count;
    //    };
    //    int nb_count = CountRGBUniqueValues(quantized); // number of colors in quantized image : we don't know how many
    //    if (nb_palettes > nb_palettes_max) // limit number of colors to consider
    //        nb_palettes = nb_palettes_max;
    //    struct_colors color[nb_count]; // temp palette

    //    int nbColor = 0;
    //    for (int x = 0; x < quantized.cols; x++) // parse quantized image
    //        for (int y = 0; y < quantized.rows; y++) {
    //            cv::Vec3b col = quantized.at<cv::Vec3b>(y, x);
    //            bool found = false; // indicates if color was known
    //            for (int i = 0; i < nbColor; i++) // parse colors index
    //                if (col == color[i].RGB) { // if color already registered
    //                    found = true; // we found it !
    //                    color[i].count++; // add 1 pixel to the count of this color
    //                    break; // stop
    //                }
    //            if (!found) { // if color was not found
    //                color[nbColor].RGB = col; // add it to the index
    //                color[nbColor].count = 1; // 1 pixel found for the moment
    //                nbColor++; // increase number of colors found
    //            }
    //        }

    //    std::sort(color, color + nbColor,
    //        [](const struct_colors& a, const struct_colors& b) {return a.count > b.count; }); // sort colors by count, descending

    //    int total = quantized.rows * quantized.cols; // number of pixels in image
    //    // delete insignificant colors
    //    while ((nbColor > 1) and (double(color[nbColor - 1].count) / total < 0.005)) // is the last color percentage an insignificant value ?
    //        nbColor--; // one less color to consider

    //    if (nbColor > nb_count) // number of asked colors could be inferior to the real number of colors in quantized image
    //        nbColor = nb_count;
    //    if (nbColor > nb_palettes_max) // number of colors must not be superior to max number of colors in palette
    //        nbColor = nb_palettes_max;
    //    nb_palettes = nbColor; // real number of colors to consider

    //    for (int n = 0; n < nbColor; n++) { // for all colors in Mean-shift palette
    //        palettes[n].R = color[n].RGB[2]; // copy RGB values to global palette
    //        palettes[n].G = color[n].RGB[1];
    //        palettes[n].B = color[n].RGB[0];
    //        totalMean += color[n].count; // compute total number of pixels for this color
    //    }
    //}

    // compute HSL values from RGB + hexa + distances
    for (int n = 0; n < nb_palettes; n++) // for each color in palette
        ComputePaletteValues(n); // compute values other than RGB

    // clean palette : number of asked colors may be superior to number of colors found
    int nb_real = CountRGBUniqueValues(quantized); // how many colors in quantized image, really ?
    if (nb_real < nb_palettes) { // if asked number of colors exceeds total number of colors in image
        std::sort(palettes, palettes + nb_palettes,
            [](const struct_palette& a, const struct_palette& b) {return a.hexa > b.hexa; }); // sort palette by hexa value, decending
        if (((palettes[0].R == palettes[1].R) and (palettes[0].G == palettes[1].G)
            and (palettes[0].B == palettes[1].B)) or (palettes[0].R == -1)) // if first color in palette is equal to second or it's a dummy color -> we have to reverse sort
            std::sort(palettes, palettes + nb_palettes,
                [](const struct_palette& a, const struct_palette& b) {return a.hexa > b.hexa; }); // sort the palette, this time by increasing hexa values
        nb_palettes = nb_real; // new number of colors in palette
    }

    //int total; // total number of pixels to consider for percentages
    //if ((ui->radioButton_mean_shift->isChecked()) or (ui->radioButton_sectored_means->isChecked())) // particular case of mean algorithms
    //    total = totalMean; // total is the mean total computed before
    //else // not mean algorithm
    //    total = quantized.rows * quantized.cols; // total is the size of quantized image in pixels

    //// delete blacks in palette if "filter grays" enabled because there really can be one blackish color in the quantized image that could have been mixed with others
    //if (ui->checkBox_filter_grays->isChecked()) { // delete last "black" values in palette
    //    bool black_found = false;
    //    std::sort(palettes, palettes + nb_palettes,
    //        [](const struct_palette& a, const struct_palette& b) {return a.distanceBlack > b.distanceBlack; }); // sort palette by distance from black, descending
    //    while ((nb_palettes > 1) and (palettes[nb_palettes - 1].distanceBlack < blacksLimit)) { // at the end of palette, find black colors
    //        cv::Mat1b black_mask;
    //        cv::inRange(quantized, cv::Vec3b(palettes[nb_palettes - 1].B, palettes[nb_palettes - 1].G, palettes[nb_palettes - 1].R),
    //            cv::Vec3b(palettes[nb_palettes - 1].B, palettes[nb_palettes - 1].G, palettes[nb_palettes - 1].R),
    //            black_mask); // extract this black color from image in a mask
    //        int c = countNonZero(black_mask); // how many pixels are black ?
    //        total = total - c; // update total pixel count
    //        palettes[nb_palettes - 1].R = -1; // exclude this black color from palette
    //        nb_palettes--; // one less color in palette
    //        if (c > 0) // really found black color ?
    //            black_found = true; // black color found !
    //    }
    //    if (black_found) { // if black color found
    //        ui->spinBox_nb_palettes->setValue(nb_palettes); // show new number of colors without black values
    //    }
    //}

    // compute percentages (NOT the final value)
    for (int n = 0; n < nb_palettes; n++) { // for each color in palette
        cv::Mat1b mask; // current color mask
        cv::inRange(quantized, cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R),
            cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R),
            mask); // create mask for current color
        palettes[n].count = cv::countNonZero(mask); // count pixels in this mask
        palettes[n].percentage = (long double)(palettes[n].count) / (long double)(total); // compute color percentage in image
    }

    // regroup near colors
    bool regroup_isChecked = False;
    if (regroup_isChecked) { // is "regroup colors" enabled ?
        bool regroup = false; // if two colors are regrouped this will be true
        for (int n = 0; n < nb_palettes; n++) // parse palette
            for (int i = 0; i < nb_palettes; i++) { // parse the same palette to compare values
                if ((n != i) and (palettes[n].R + palettes[n].G + palettes[n].B != 0) and (palettes[i].R + palettes[i].G + palettes[i].B != 0)
                    and (palettes[n].R > 0) and (palettes[i].R > 0)) { // exlude same color index and black values and dummy colors
                    long double d = DistanceRGB((long double)palettes[n].R / 255.0, (long double)palettes[n].G / 255.0, (long double)palettes[n].B / 255.0,
                        (long double)palettes[i].R / 255.0, (long double)palettes[i].G / 255.0, (long double)palettes[i].B / 255.0,
                        1.0, 0.5, 1.0); // distance with less for chroma
                    if (d < 15) { // check if the two colors are near ("regroup" filter distance)
                        long double R, G, B;
                        RGBMean((long double)palettes[n].R / 255.0, (long double)palettes[n].G / 255.0, (long double)palettes[n].B / 255.0, palettes[n].count,
                            (long double)palettes[i].R / 255.0, (long double)palettes[i].G / 255.0, (long double)palettes[i].B / 255.0, palettes[i].count,
                            R, G, B); // the new color is the RGB mean of the two colors

                        // change quantized image
                        cv::Mat mask1;
                        cv::inRange(quantized, cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R),
                            cv::Vec3b(palettes[n].B, palettes[n].G, palettes[n].R),
                            mask1); // extract first color n from image
                        quantized.setTo(cv::Vec3b(round(B * 255.0), round(G * 255.0), round(R * 255.0)), mask1); // replace pixels with new color in Quantized
                        cv::Mat mask2; // do the same for the second i color
                        cv::inRange(quantized, cv::Vec3b(palettes[i].B, palettes[i].G, palettes[i].R),
                            cv::Vec3b(palettes[i].B, palettes[i].G, palettes[i].R),
                            mask2); // extract second color i from image
                        quantized.setTo(cv::Vec3b(round(B * 255.0), round(G * 255.0), round(R * 255.0)), mask2); // replace pixels with new color in Quantized

                        // new palette values
                        palettes[n].R = round(R * 255.0); // replace colors in palette n with new color values
                        palettes[n].G = round(G * 255.0);
                        palettes[n].B = round(B * 255.0);
                        palettes[n].count += palettes[i].count; // merge the two colors count
                        palettes[n].percentage += palettes[i].percentage; // and merge the percentage too
                        ComputePaletteValues(n); // compute new palette values other than RGB

                        // palette has changed
                        palettes[i].R = -1; // dummy value (important, it excludes this color now from the algorithm)
                        regroup = true; // at least one color regroup was found
                    }
                }
            }
        if (regroup) { // at least one color regroup was found so palette has changed
            std::sort(palettes, palettes + nb_palettes,
                [](const struct_palette& a, const struct_palette& b) {return a.R > b.R; }); // sort palette by hexa value, descending
            while ((nb_palettes > 1) and (palettes[nb_palettes - 1].R == -1)) // look for excluded colors
                nb_palettes--; // update palette count
        }
    }

    nb_palettes_found = nb_palettes; // max number of colors found, keep it

    // delete non significant values in palette by percentage
    bool isChecked_checkBox_filter_percent = true;
    if (isChecked_checkBox_filter_percent) { // filter by x% enabled ?
        bool cleaning_found = false; // indicator
        std::sort(palettes, palettes + nb_palettes,
            [](const struct_palette& a, const struct_palette& b) {return a.percentage > b.percentage; }); // sort palette by percentage, descending
        while ((nb_palettes > 1) and (palettes[nb_palettes - 1].percentage * 100 < ui->1)) { // at the end of palette, find colors < x% of image
            cv::Mat1b cleaning_mask;
            cv::inRange(quantized, cv::Vec3b(palettes[nb_palettes - 1].B, palettes[nb_palettes - 1].G, palettes[nb_palettes - 1].R),
                cv::Vec3b(palettes[nb_palettes - 1].B, palettes[nb_palettes - 1].G, palettes[nb_palettes - 1].R),
                cleaning_mask); // extract this color from image
            int c = cv::countNonZero(cleaning_mask); // count occurences of this color
            total = total - c; // update total pixel count
            nb_palettes--; // exclude this color from palette
            if (c > 0) // really found this color ?
                cleaning_found = true; // palettes count has changed
        }
        if (cleaning_found) { // if cleaning found
            //ui->spinBox_nb_palettes->setValue(nb_palettes); // show new number of colors without cleaned values
            // re-compute percentages
            for (int n = 0; n < nb_palettes; n++) // for each color in palette
                palettes[n].percentage = (long double)(palettes[n].count) / (long double)(total); // update percentage with new total
        }
    }

    // find color name by CIEDE2000 distance for all palette
    for (int n = 0; n < nb_palettes; n++) { // for each color in palette
        FindColorName(n); // find its name
    }

    if (nb_palettes < 1) // at least one color in palette !
        nb_palettes = 1;
    if (palettes[nb_palettes - 1].R == -1) { // if the only color in palette is a dummy one
        palettes[nb_palettes - 1].R = 0; // "paint it black" !
        palettes[nb_palettes - 1].G = 0;
        palettes[nb_palettes - 1].B = 0;
    }
    if (nb_palettes > nb_palettes_asked) // limit number of colors to asked number of colors
        nb_palettes = nb_palettes_asked;

    ResetSort(); // reset combo box to default (percentage) without activating it
    ComputePaletteImage(); // create palette image

    zoom = false; // no zoom for Image and Quantized
    pickedColor = cv::Vec3b(-1, -1, -1); // dummy values
    //ShowWheel(); // display color wheel
    ShowResults(); // show result images

    //ShowTimer(false); // show elapsed time
    //QApplication::restoreOverrideCursor(); // Restore cursor

    computed = true; // success !

}

int main() {
    std::string imageFilePath = "path_to_your_image.jpg";

    // 使用 cv::imread 函数读取图像
    cv::Mat image = cv::imread(imageFilePath, cv::IMREAD_COLOR);

    // 检查图像是否成功加载
    if (image.empty()) {
        std::cerr << "Could not read the image: " << imageFilePath << std::endl;
        return 1; // 或者处理错误情况
    }

    // 在这里处理 image ...

    // 如果你想显示图像，可以使用 OpenCV 的 HighGUI 模块。
    // 注意：你需要在调用 cv::waitKey 函数之前调用 cv::destroyAllWindows 或者 cv::destroyWindow 来释放窗口资源。
    // cv::imshow("Loaded Image", image);
    // cv::waitKey(0);
    // cv::destroyAllWindows();

    Compute(image);
    SavePalette();

    return 0;
}
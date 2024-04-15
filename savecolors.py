import os
import csv
import struct
import struct
import math
class CImageInfo:
    def __init__(self):
        self.app_workpath = None
        self.color_names = []
        self.nb_color_names = -1
        self.palettes = []
        self.nb_palettes = 0

    def gamma_correction_srgb(channel):
        # Assume the input range is 0.0 to 1.0 (if 0-255 then divide by 255)
        if channel <= 0.04045:
            return channel / 12.92
        else:
            return ((channel + 0.055) / 1.055) ** 2.4

    def rgb_to_xyz(r, g, b):
        # Apply gamma correction to the RGB values
        r_lin = r.gamma_correction_srgb(r)
        g_lin = g.gamma_correction_srgb(g)
        b_lin = b.gamma_correction_srgb(b)

        # sRGB to XYZ conversion matrix coefficients
        x = r_lin * 0.4124564 + g_lin * 0.3575761 + b_lin * 0.1804375
        y = r_lin * 0.2126729 + g_lin * 0.7151522 + b_lin * 0.0721750
        z = r_lin * 0.0193339 + g_lin * 0.1191920 + b_lin * 0.9503041

        return x, y, z

    def xyz_to_lab(x, y, z):
        # Define reference white in XYZ
        ref_x = 0.95047
        ref_y = 1.0
        ref_z = 1.08883
        # Define CIE values
        epsilon = 216.0 / 24389.0
        kappa = 24389.0 / 27.0

        # Calculate ratio of XYZ to reference white
        xr = x / ref_x
        yr = y / ref_y
        zr = z / ref_z

        # Calculate f(X), f(Y), f(Z)
        fx = math.pow(xr, 1 / 3) if xr > epsilon else (kappa * xr + 16) / 116
        fy = math.pow(yr, 1 / 3) if yr > epsilon else (kappa * yr + 16) / 116
        fz = math.pow(zr, 1 / 3) if zr > epsilon else (kappa * zr + 16) / 116

        # Calculate L*, a*, b*
        l = 116 * fy - 16
        a = 500 * (fx - fy)
        b = 200 * (fy - fz)

        # Normalize L*, a*, b* to stay in the [0..1] range
        l = l / 100.0
        a = a / 127.0
        b = b / 127.0

        return l, a, b

    # Example usage
    x, y, z = 0.95047, 1.0, 1.08883  # D65 reference white
    l, a, b = xyz_to_lab(x, y, z)
    print("L*:", l, "a*:", a, "b*:", b)
    # Example usage with RGB values in the range [0, 255]
    r, g, b = 255, 0, 0  # Red color
    # Normalize the RGB values to [0, 1]
    r_normalized, g_normalized, b_normalized = r / 255.0, g / 255.0, b / 255.0
    x, y, z = rgb_to_xyz(r_normalized, g_normalized, b_normalized)
    print("XYZ:", x, y, z)

    def distanceCIEDE2000LAB(L1, A1, B1, L2, A2, B2, k_L=1.0, k_C=1.0, k_H=1.0):
        # Constants
        Pi = math.pi

        # Convert values to correct ranges: L, a, and b must be in [0..100]
        l1 = L1 * 100.0
        l2 = L2 * 100.0
        a1 = A1 * 100.0
        a2 = A2 * 100.0
        b1 = B1 * 100.0
        b2 = B2 * 100.0

        # Step 1
        C1 = math.sqrt((a1 * a1) + (b1 * b1))
        C2 = math.sqrt((a2 * a2) + (b2 * b2))
        barC = (C1 + C2) / 2.0
        G = 0.5 * (1 - math.sqrt((barC ** 7) / (barC ** 7 + 25 ** 7)))
        a1_prime = (1.0 + G) * a1
        a2_prime = (1.0 + G) * a2
        C_prime1 = math.sqrt((a1_prime * a1_prime) + (b1 * b1))
        C_prime2 = math.sqrt((a2_prime * a2_prime) + (b2 * b2))

        h_prime1 = math.atan2(b1, a1_prime) * 180.0 / Pi if b1 != 0 or a1_prime != 0 else 0.0
        h_prime1 += 360.0 if h_prime1 < 0 else 0.0

        h_prime2 = math.atan2(b2, a2_prime) * 180.0 / Pi if b2 != 0 or a2_prime != 0 else 0.0
        h_prime2 += 360.0 if h_prime2 < 0 else 0.0

        # Step 2
        delta_L_prime = l2 - l1
        delta_C_prime = C_prime2 - C_prime1

        deltah_prime = h_prime2 - h_prime1 if C1 * C2 != 0 else 0.0
        deltah_prime += 360.0 if deltah_prime < -180 else deltah_prime
        deltah_prime -= 360.0 if deltah_prime > 180 else deltah_prime

        delta_H_prime = 2.0 * math.sqrt(C_prime1 * C_prime2) * math.sin(math.radians(deltah_prime / 2.0))

        # Step 3
        barL_prime = (l1 + l2) / 2.0
        barC_prime = (C_prime1 + C_prime2) / 2.0

        h_prime_sum = h_prime1 + h_prime2
        if C_prime1 * C_prime2 == 0:
            barh_prime = h_prime_sum
        else:
            if abs(h_prime1 - h_prime2) <= 180:
                barh_prime = h_prime_sum / 2.0
            else:
                barh_prime = (h_prime_sum + 360) / 2.0 if h_prime_sum < 360 else (h_prime_sum - 360) / 2.0

        T = 1 - 0.17 * math.cos(math.radians(barh_prime - 30)) + \
            0.24 * math.cos(math.radians(2 * barh_prime)) + \
            0.32 * math.cos(math.radians(3 * barh_prime + 6)) - \
            0.20 * math.cos(math.radians(4 * barh_prime - 63))

        delta_theta = math.radians(30) * math.exp(
            -((math.radians(barh_prime) - math.radians(275)) / math.radians(25)) ** 2)
        R_C = 2 * math.sqrt((barC_prime ** 7) / (barC_prime ** 7 + 25 ** 7))
        S_L = 1 + (0.015 * (barL_prime - 50) ** 2) / math.sqrt(20 + (barL_prime - 50) ** 2)
        S_C = 1 + 0.045 * barC_prime
        S_H = 1 + 0.015 * barC_prime * T
        R_T = -math.sin(It
        seems
        the
        response
        was
        cut
        off.Here
        's the completion of the Python function for calculating the CIEDE2000 color difference:

        ```python
        R_T = -math.sin(2 * delta_theta) * R_C

        # Step 4: Compute the color difference
        delta_E = math.sqrt(
            (delta_L_prime / (k_L * S_L)) ** 2 +
            (delta_C_prime / (k_C * S_C)) ** 2 +
            (delta_H_prime / (k_H * S_H)) ** 2 +
            R_T * (delta_C_prime / (k_C * S_C)) * (delta_H_prime / (k_H * S_H))
        )
        return delta_E

    # Example usage:
    print(distanceCIEDE2000LAB(50.0, 2.6772, -79.7751, 50.0, 0.0, -82.7485))
    def distance_rgb(r1, g1, b1, r2, g2, b2, k_l, k_c, k_h):
        # 转换第一个RGB到XYZ
        x1, y1, z1 = rgb_to_xyz(r1, g1, b1)
        # 转换XYZ到CIELab
        l1, a1, b1_lab = xyz_to_lab(x1, y1, z1)

        # 转换第二个RGB到XYZ
        x2, y2, z2 = rgb_to_xyz(r2, g2, b2)
        # 转换XYZ到CIELab
        l2, a2, b2_lab = xyz_to_lab(x2, y2, z2)

        # 计算CIEDE2000颜色距离
        return ciede2000(l1, a1, b1_lab, l2, a2, b2_lab, k_l, k_c, k_h)

    def find_color_name(n_palette, palettes, color_names, nb_color_names, distance_rgb_func):
        found = False
        nearest_distance = float('inf')  # Represents a very large number
        nearest_color_index = 0

        # Assuming palettes and color_names are lists of dicts with keys 'R', 'G', 'B', and 'name'
        for c in range(nb_color_names):
            if (palettes[n_palette]['R'] == color_names[c]['R'] and
                    palettes[n_palette]['G'] == color_names[c]['G'] and
                    palettes[n_palette]['B'] == color_names[c]['B']):
                # Exact color found
                palettes[n_palette]['name'] = color_names[c]['name']
                found = True
                break
            else:
                # Calculate distance between the colors
                d = distance_rgb_func(
                    palettes[n_palette]['R'] / 255.0,
                    palettes[n_palette]['G'] / 255.0,
                    palettes[n_palette]['B'] / 255.0,
                    color_names[c]['R'] / 255.0,
                    color_names[c]['G'] / 255.0,
                    color_names[c]['B'] / 255.0
                )
                if d < nearest_distance:
                    nearest_distance = d
                    nearest_color_index = c

        if not found:
            # Assign nearest color name if exact match wasn't found
            palettes[n_palette]['name'] = color_names[nearest_color_index]['name']

    # The distance_rgb_func is a placeholder for whatever distance function you use
    # For example, it could be the CIEDE2000 formula, which you would need to implement
    def save_palette(self, palette):
        module_path = os.path.dirname(os.path.abspath(__file__))
        self.app_workpath = os.path.join(module_path, "WORK", "DATA")

        # Read color names from .csv file
        try:
            with open(os.path.join(self.app_workpath, "color-names.csv"), mode='r') as csvfile:
                csv_reader = csv.reader(csvfile, delimiter=';')
                next(csv_reader)  # Skip header
                for row in csv_reader:
                    r, g, b, name = int(row[0]), int(row[1]), int(row[2]), row[3]
                    self.color_names.append({'R': r, 'G': g, 'B': b, 'name': name})
                    self.nb_color_names += 1
        except FileNotFoundError:
            print("color-names.csv not found")

        # Write palette to .CSV file
        with open(os.path.join(self.app_workpath, "-palette.csv"), mode='w', newline='') as csvfile:
            csv_writer = csv.writer(csvfile, delimiter=';')
            # CSV header
            headers = ["Name (string)", "RGB.R [0..255]", "RGB.G [0..255]", "RGB.B [0..255]",
                       "RGB.hexa (string)", "percentage [0..100]", "sRGB.R [0..255]", "sRGB.G [0..255]",
                       "sRGB.B [0..255]", "HSV.H [0..359]", "HSV.S [0..100]", "HSV.V [0..100]",
                       "HSL.H [0..359]", "HSL.S [0..100]", "HSL.L [0..100]", "XYZ.X [0..100]",
                       "XYZ.Y [0..100]", "XYZ.Z [0..100]", "Lab.L [0..100]", "Lab.a [-128..127]",
                       "Lab.b [-128..127]", "LCHab.L [0..100]", "LCHab.C [0..100+]", "LCHab.H [0..359]",
                       "CMYK.C [0..100]", "CMYK.M [0..100]", "CMYK.Y [0..100]", "CMYK.K [0..100]"]
        # Assuming `palettes` is a list of objects or dictionaries with color information
        # and `nb_palettes` is the number of colors in the palette

        # Write palette to CSV file
        csv_filename = "palette.csv"
        with open(csv_filename, mode='w', newline='') as csv_file:
            csv_writer = csv.writer(csv_file, delimiter=';')
            # Write CSV header
            csv_writer.writerow([
                "Name (string)", "RGB.R [0..255]", "RGB.G [0..255]", "RGB.B [0..255]", "RGB.hexa (string)",
                "percentage [0..100]", "sRGB.R [0..255]", "sRGB.G [0..255]", "sRGB.B [0..255]",
                # ... add the rest of the header fields
            ])
            # Write palette colors to CSV
            for n in range(self.nb_palettes):
                palette = self.palettes[n]
                # Compute additional color information
                # ... gamma correction, standard RGB, HSV, HSL, XYZ, Lab, LCHab, CMYK

                # Write color information to CSV
                csv_writer.writerow([
                    palette['name'], palette['R'], palette['G'], palette['B'], palette['hexa'],
                    round(palette['percentage'] * 100),
                    # ... add the rest of the color information fields
                ])

        # Write palette to .ACT file (Adobe Photoshop and Illustrator)
        act_filename = "palette-adobe.act"
        with open(act_filename, mode='wb') as act_file:
            buffer = bytearray(772)
            for n in range(self.nb_palettes):
                buffer[n * 3 + 0] = self.palettes[n]['R']
                buffer[n * 3 + 1] = self.palettes[n]['G']
                buffer[n * 3 + 2] = self.palettes[n]['B']
            # Set the number of colors in the palette and transparency index
            buffer[768:770] = struct.pack('>H', self.nb_palettes)  # '>H' denotes big-endian unsigned short
            buffer[770:772] = struct.pack('>H', 255)  # Transparency index
            act_file.write(buffer)

        # Note: You will need to replace the comments with actual code that performs the color computations
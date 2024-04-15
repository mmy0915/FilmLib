#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QSizeGrip>
#include <QGridLayout>
#include <QDesktopWidget>
#include <QCursor>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>
#include <QWhatsThis>

#include <fstream>

#include "mat-image-tools.h"
#include "dominant-colors.h"
#include "color-spaces.h"
#include "angles.h"

void main(){
    if (!loaded) { // nothing loaded yet = get out
        return;
    }

    /*QApplication::setOverrideCursor(Qt::WaitCursor); // wait cursor
    timer.start(); // reinit timer
    ShowTimer(true); // show it
    qApp->processEvents();
    */
    //QT有关的都注释掉，此处需要单独计算一下运行时间，先放着

    cv::Mat imageCopy; // work on a copy of the image, because gray colors can be filtered
    image.copyTo(imageCopy);

    long double H, S, L;
    /* 转为黑白照片分析，删除
    if (ui->checkBox_filter_grays->isChecked()) { // filter whites, blacks and grays if gray filter is set 
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
    }
    */

    nb_palettes = ui->spinBox_nb_palettes->value(); // how many dominant colors 自行指定
    int nb_palettes_asked = nb_palettes; // save asked number of colors for later
    //ui->spinBox_nb_palettes->setStyleSheet("QSpinBox{color:black;background-color: white;}"); // show number of colors in black (in case it was red before)

    /* 分析黑白照片分析，删除
    if (ui->checkBox_filter_grays->isChecked()) { // if grays and blacks and whites are filtered
        cv::Mat1b black_mask;
        cv::inRange(imageCopy, cv::Vec3b(0, 0, 0), cv::Vec3b(0, 0, 0), black_mask); // extract black pixels from image (= whites and blacks and grays)
        if ((cv::sum(black_mask) != cv::Scalar(0, 0, 0))) // image contains black pixels ?
            nb_palettes++; // add one color to asked number of colors in palette, to remove it later and only get colors
    }
    */

    // set all palette values to dummy values
    for (int n = 0; n < nb_palettes_max; n++) {
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

    //if (ui->radioButton_mean_shift->isChecked()) { // mean-shift algorithm checked : intermediate number of colors unknown 目前只有meanshift算法，删除if选择结构
        cv::Mat temp = ImgRGBtoLab(imageCopy); // convert image to CIELab

        MeanShift MSProc(ui->horizontalSlider_mean_shift_spatial->value(), ui->horizontalSlider_mean_shift_color->value()); // create instance of Mean-shift 这里需要修改ui参数
        MSProc.MeanShiftFilteringCIELab(temp); // Mean-shift filtering
        MSProc.MeanShiftSegmentationCIELab(temp); // Mean-shift segmentation
        quantized = ImgLabToRGB(temp); // convert image back to RGB

        // palette from quantized image
        struct struct_colors { // color index and count
            cv::Vec3b RGB;
            int count;
        };
        int nb_count = CountRGBUniqueValues(quantized); // number of colors in quantized image : we don't know how many
        struct_colors color[nb_count]; // temp palette

        int nbColor = 0;
        for (int x = 0; x < quantized.cols; x++) // parse quantized image
            for (int y = 0; y < quantized.rows; y++) {
                cv::Vec3b col = quantized.at<cv::Vec3b>(y, x); // current pixel
                bool found = false; // indicates if color was known
                for (int i = 0; i < nbColor; i++) // parse colors index
                    if (col == color[i].RGB) { // if color already registered
                        found = true; // we found it !
                        color[i].count++; // add 1 pixel to the count of this color
                        break; // stop
                    }
                if (!found) { // if color was not found
                    color[nbColor].RGB = col; // add it to the index
                    color[nbColor].count = 1; // 1 pixel found for the moment
                    nbColor++; // increase number of colors found
                }
            }
        std::sort(color, color + nbColor,
            [](const struct_colors& a, const struct_colors& b) {return a.count > b.count; }); // sort colors by count, descending

        int total = quantized.rows * quantized.cols; // number of pixels in image
        // clean insignificant colors by percentage
        while ((nbColor > 1) and (double(color[nbColor - 1].count) / total < 0.005)) // is the last color percentage an insignificant value ?
            nbColor--; // one less color to consider

        if (nbColor > nb_count) // number of asked colors could be inferior to the real number of colors in quantized image
            nbColor = nb_count;
        nb_palettes = nbColor; // real number of colors in palette

        for (int n = 0; n < nbColor; n++) { // for all colors in Mean-shift palette
            palettes[n].R = color[n].RGB[2]; // copy RGB values to global palette
            palettes[n].G = color[n].RGB[1];
            palettes[n].B = color[n].RGB[0];
            totalMean += color[n].count; // compute total number of pixels for this color
        }
    //}

}

void MainWindow::ShowResults() // display result images in GUI
{
    if (!image.empty()) { // is there an image to display ?
        if (zoom) { // zoomed ?
            ui->label_image->setGeometry(0, 0, image.cols, image.rows); // adapt size of label
            ui->label_image->setPixmap(Mat2QPixmap(image)); // show Image
        }
        else {
            ui->label_image->setGeometry(0, 0, ui->scrollArea_image->viewport()->width(), ui->scrollArea_image->viewport()->height()); // adapt size of label to thumbnail size
            ui->label_image->setPixmap(Mat2QPixmapResized(image, ui->scrollArea_image->viewport()->width(), ui->scrollArea_image->viewport()->height(), false)); // show thumbnail
        }
    }
    else { // no image to display
        ui->label_image->setGeometry(0, 0, 1, 1); // 1 pixel size
        ui->label_image->setPixmap(QPixmap()); // display empty image
    }

    if (!quantized.empty()) { // quantized image
        if (pickedColor != cv::Vec3b(-1, -1, -1)) { // overlay picked color on Quantized ?
            cv::Mat mask;
            cv::inRange(quantized, pickedColor, pickedColor, mask); // extract picked color pixels from Quantized

            cv::Mat quantized_result;
            quantized.copyTo(quantized_result); // work on a copy of Quantized

            quantized_result.setTo(cv::Vec3b(255, 255, 255), mask); // overlay Quantized with white pixels to show current picked color

            if (zoom) { // zoom Quantized ?
                ui->label_quantized->setGeometry(0, 0, quantized_result.cols, quantized_result.rows); // adapt size of label
                ui->label_quantized->setPixmap(Mat2QPixmap(quantized_result)); // show Quantized image
            }
            else { // not zoomed
                ui->label_quantized->setGeometry(0, 0, ui->scrollArea_quantized->viewport()->width(), ui->scrollArea_quantized->viewport()->height()); // adapt label to thumbnail size
                ui->label_quantized->setPixmap(Mat2QPixmapResized(quantized_result, ui->scrollArea_quantized->viewport()->width(), ui->scrollArea_quantized->viewport()->height(), false)); // show thumbnailed Quantized
            }
        }
        else { // don't overlay Quantized
            if (zoom) { // show it zoomed ?
                ui->label_quantized->setGeometry(0, 0, quantized.cols, quantized.rows); // adapt size of label
                ui->label_quantized->setPixmap(Mat2QPixmap(quantized)); // show Quantized image
            }
            else { // not zoomed
                ui->label_quantized->setGeometry(0, 0, ui->scrollArea_quantized->viewport()->width(), ui->scrollArea_quantized->viewport()->height()); // adapt label size to thumbnailed view
                ui->label_quantized->setPixmap(Mat2QPixmapResized(quantized, ui->scrollArea_quantized->viewport()->width(), ui->scrollArea_quantized->viewport()->height(), false)); // show thumbnailed Quantized
            }
        }
    }
    else { // no Quantized to display
        ui->label_quantized->setGeometry(0, 0, 1, 1); // 1 pixel size
        ui->label_quantized->setPixmap(QPixmap()); // display empty image
    }

    if (!palette.empty()) { // palette image
        if (pickedColor != cv::Vec3b(-1, -1, -1)) { // overlay Palette with picked color ?
            cv::Mat mask;
            cv::inRange(palette, pickedColor, pickedColor, mask); // extract picked color pixels from image

            std::vector<std::vector<cv::Point>> contours;
            std::vector<cv::Vec4i> hierarchy;
            cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // find contours of picked color

            cv::Mat palette_result; // work on a copy of Palette image
            palette.copyTo(palette_result);

            drawContours(palette_result, contours, -1, cv::Vec3b(255, 255, 255), 8, 8, hierarchy); // draw contour of picked color in Palette image

            ui->label_palette->setPixmap(Mat2QPixmapResized(palette_result, ui->label_palette->width(), ui->label_palette->height(), false)); // show Palette image
        }
        else // don't overlay Palette image
            ui->label_palette->setPixmap(Mat2QPixmapResized(palette, ui->label_palette->width(), ui->label_palette->height(), false)); // show Palette image
    }
    else // no Palette image to display
        ui->label_palette->setPixmap(QPixmap()); // show empty Palette image

    if (zoom) { // zoomed view for Image and Quantized ?
        ui->scrollArea_quantized->verticalScrollBar()->setValue(ui->scrollArea_image->verticalScrollBar()->value()); // synchronize their scroll bars
        ui->scrollArea_quantized->horizontalScrollBar()->setValue(ui->scrollArea_image->horizontalScrollBar()->value());
    }
}
#include <QtWidgets>

#include "imageviewer.h"
#include <math.h>

ImageViewer::ImageViewer()
   : imageLabel(new QLabel)
   , scrollArea(new QScrollArea)
   , scaleFactor(1)
{
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);

    createActions();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}


bool ImageViewer::loadFile(const QString &fileName) {
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    setImage(newImage);

    setWindowFilePath(fileName);

    const QString message = tr("Path: \"%1\", Size: %2x%3")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}

void ImageViewer::calculateAndShowBasicIndicators() {
    if (!image.isNull()) {
        int imageWidth = image.width();
        int imageHeight = image.height();

        int minimum = 255;
        int maximum = 0;
        int sum = 0;

        int colorInt = 0;
        QColor rgb;

        for (int i = 0; i < imageWidth; i++) {
            for (int j = 0; j < imageHeight; j++) {
                rgb = image.pixelColor(i,j);
                colorInt = rgb.red();
                sum += colorInt;
                if (colorInt < minimum) {
                    minimum = colorInt;
                }
                if (colorInt > maximum) {
                    maximum = colorInt;
                }
            }
        }
        int pixelsImage = imageWidth*imageHeight;
        double average = sum / pixelsImage;
        double powVariance;
        double sumOfPow = 0;
        double variance;
        for (int i = 0; i < imageWidth; i++) {
            for (int j = 0; j < imageHeight; j++) {
                rgb = image.pixelColor(i,j);
                colorInt = rgb.red();
                variance = colorInt - average;
                powVariance = pow(variance, 2);
                sumOfPow += powVariance;
            }
        }
        double stdDeviation = sqrt(sumOfPow / sum);
        QMessageBox msgBox;
        QString message = tr("Min: %1, Max: %2, Average: %3, Std.Deviation: %4")
                    .arg(minimum).arg(maximum).arg(average).arg(stdDeviation);
        msgBox.setText(message);
        msgBox.exec();
    } else {
        QMessageBox msgBox;
        msgBox.setText("First you have to open an image.");
        msgBox.exec();
    }
}

//Does not work!
void ImageViewer::imageSmoothing() {
    bool ok;
    int n = QInputDialog::getInt(this,tr("Smooth Level"), tr(""), 0, 0, 9999, 1, &ok);
    if (ok) {
        QImage smoothedImage = QImage(image.size(),image.format());
        int step = 0;
        if (n % 2 == 0) {
            step = (n-1) / 2;
        } else {
            step = (n-2) / 2;
        }

        QColor averagedColor = QColor(0, 0, 0);

        int imageWidth = image.width();
        int imageHeight = image.height();
        int colorSum = 0;
        QColor actualColor;

        for (int i = 0; i < imageWidth; i++) {
            for (int j = 0; j < imageHeight; j++) {
                colorSum = 0;
                int numberOFNeighbours = 0;
                int minX = i - step;
                int minY = j - step;
                int maxX = i + step;
                int maxY = j + step;
                if (minX < 0) {
                    minX = 0;
                }
                if (minY < 0) {
                    minY = 0;
                }
                if (maxX > imageWidth -1) {
                    maxX = imageWidth-1;
                }
                if (maxY > imageHeight - 1) {
                    maxY = imageHeight - 1;
                }
                int actualX = minX;
                int actualY = minY;

                do{
                    do{
                        numberOFNeighbours++;
                        actualColor = image.pixelColor(actualX,actualY);
                        colorSum += actualColor.red();
                        actualX++;
                    }while(actualX <= maxX);
                    actualX = minX;
                    actualY++;
                }while(actualY <= maxY);

                int averageColorInt = colorSum / numberOFNeighbours;
                averagedColor.setRed(averageColorInt);
                averagedColor.setGreen(averageColorInt);
                averagedColor.setBlue(averageColorInt);

                smoothedImage.setPixelColor(i, j, averagedColor);
            }
        }
        setImage(smoothedImage);
    }
}

void ImageViewer::imageThresholding() {
    bool ok;
    int n = QInputDialog::getInt(this,tr("Threshold Level"),tr("0-255"), 0, 0, 255, 1, &ok);
    if (ok) {
        QImage newImage = QImage(image.size(),image.format());

        QColor* black = new QColor(0,0,0);
        QColor* white = new QColor(255,255,255);
        int actualColorNr = 0;
        QColor actualColor;

        for (int i = 0; i < image.width(); i++) {
            for (int j = 0; j < image.height(); j++) {
                actualColor = image.pixelColor(i,j);
                actualColorNr = actualColor.red();
                if(actualColorNr <= n) {
                    newImage.setPixelColor(i,j,*black);
                } else if (actualColorNr > n) {
                    newImage.setPixelColor(i,j,*white);
                }
            }
        }
        setImage(newImage);
    }
}

void ImageViewer::calculateHorizontalGradient() {
    QImage newImage = image;
    QColor pixel;
    int leftPixel = 0;
    int rightPixel = 0;
    int colorValue = 0;

    QColor newColor = QColor(colorValue, colorValue, colorValue);

    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            if (i == 0) {
                pixel = image.pixelColor(i + 1,j);
                rightPixel = pixel.red();
                pixel = image.pixelColor(i,j);
                colorValue = (pixel.red() - rightPixel) / 2;
            } else if (i == image.width() -1) {
                pixel = image.pixelColor(i - 1,j);
                leftPixel = pixel.red();
                pixel = image.pixelColor(i,j);
                colorValue = (pixel.red()- leftPixel) / 2;
            } else {
                pixel = image.pixelColor(i+1,j);
                rightPixel = pixel.red();
                pixel = image.pixelColor(i-1,j);
                leftPixel = pixel.red();
                colorValue = (rightPixel-leftPixel) /2;
            }
            if (colorValue < 0) {
                colorValue *= -1;
            }
            newColor.setRed(colorValue);
            newColor.setBlue(colorValue);
            newColor.setGreen(colorValue);

            newImage.setPixelColor(i, j, newColor);

        }
    }
    setImage(newImage);
}

void ImageViewer::calculateVerticalGradient() {
    QImage newImage = image;

    QColor pixel;
    int upperPixel = 0;
    int lowerPixel = 0;
    int colorValue = 0;

    QColor newColor = QColor(colorValue, colorValue, colorValue);

    for (int i = 0; i < image.width(); i++) {
        for (int j = 0; j < image.height(); j++) {
            if (j == 0) {
                pixel = image.pixelColor(i,j + 1);
                lowerPixel = pixel.red();
                pixel = image.pixelColor(i,j);
                colorValue = (pixel.red() - lowerPixel) / 2;
            } else if (j == image.height() -1) {
                pixel = image.pixelColor(i,j-1);
                upperPixel = pixel.red();
                pixel = image.pixelColor(i,j);
                colorValue = (pixel.red()- upperPixel) / 2;
            } else {
                pixel = image.pixelColor(i,j+1);
                lowerPixel = pixel.red();
                pixel = image.pixelColor(i,j-1);
                upperPixel = pixel.red();
                colorValue = (lowerPixel-upperPixel) /2;
            }
            if (colorValue < 0) {
                colorValue *= -1;
            }
            newColor.setRed(colorValue);
            newColor.setBlue(colorValue);
            newColor.setGreen(colorValue);

            newImage.setPixelColor(i, j, newColor);
        }
    }
    setImage(newImage);
}

void ImageViewer::calculateMagnitudeGradient() {

}

void ImageViewer::calculateDirectionGradient() {

}

void ImageViewer::setImage(QImage &newImage) {
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    scrollArea->setVisible(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();
}


bool ImageViewer::saveFile(const QString &fileName) {
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode) {
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

void ImageViewer::open() {
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::saveAs() {
    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::copy() {
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage() {
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void ImageViewer::paste() {
#ifndef QT_NO_CLIPBOARD
    QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void ImageViewer::zoomIn() {
    scaleImage(1.25);
}

void ImageViewer::zoomOut() {
    scaleImage(0.8);
}

void ImageViewer::normalSize() {
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow() {
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow)
        normalSize();
    updateActions();
}

void ImageViewer::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &ImageViewer::open);
    openAct->setShortcut(QKeySequence::Open);

    saveAsAct = fileMenu->addAction(tr("&Save As..."), this, &ImageViewer::saveAs);
    saveAsAct->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    copyAct = editMenu->addAction(tr("&Copy"), this, &ImageViewer::copy);
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &ImageViewer::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    calculateIndicators = editMenu->addAction(tr("&Show Indicators"), this, &ImageViewer::calculateAndShowBasicIndicators);
    thresholdingImage = editMenu->addAction(tr("&Thresholding"), this, &ImageViewer::imageThresholding);
    smoothingImage = editMenu->addAction(tr("&Smoothing"), this, &ImageViewer::imageSmoothing);
    horizontalGradient = editMenu->addAction(tr("&Show Horizontal Gradient"), this, &ImageViewer::calculateHorizontalGradient);
    verticalGradient = editMenu->addAction(tr("&Show Vertical Gradient"), this, &ImageViewer::calculateVerticalGradient);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &ImageViewer::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setEnabled(false);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &ImageViewer::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setEnabled(false);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &ImageViewer::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);

    viewMenu->addSeparator();

    fitToWindowAct = viewMenu->addAction(tr("&Fit to Window"), this, &ImageViewer::fitToWindow);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
}

void ImageViewer::updateActions() {
    saveAsAct->setEnabled(!image.isNull());
    copyAct->setEnabled(!image.isNull());
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor) {
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor) {
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

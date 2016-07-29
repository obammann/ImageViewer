#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QImage>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer();
    bool loadFile(const QString &);

private slots:
    void open();
    void saveAs();
    void copy();
    void paste();
    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void calculateAndShowBasicIndicators();
    void imageSmoothing();
    void imageThresholding();
    void calculateHorizontalGradient();
    void calculateVerticalGradient();
    void calculateMagnitudeGradient();
    void calculateDirectionGradient();

private:
    void createActions();
    void createMenus();
    void updateActions();
    bool saveFile(const QString &fileName);
    void setImage(QImage &newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;

    QAction *saveAsAct;
    QAction *copyAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *calculateIndicators;
    QAction *thresholdingImage;
    QAction *smoothingImage;
    QAction *imageGradients;
    QAction *horizontalGradient;
    QAction *verticalGradient;
};

#endif

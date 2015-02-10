#ifndef NEWWATERFALLPLOT_H
#define NEWWATERFALLPLOT_H

#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_matrix_raster_data.h>
/*class NewWaterfallplot
{
public:
    NewWaterfallplot();
};

#endif // NEWWATERFALLPLOT_H
*/
class NewWaterfallplot : public QwtPlotSpectrogram
{
public:
NewWaterfallplot(const QString &title = QString::null) :
QwtPlotSpectrogram(title)
{
}

virtual ~NewWaterfallplot()
{
}

protected:
virtual QImage renderImage(const QwtScaleMap &xMap,
const QwtScaleMap &yMap,
const QRectF &rect) const
{
QRect rect = transform(xMap, yMap, area);

// I don't know why this adjustment is necessary, but
// without it qwt plots a different area than we cache
rect.adjust(-2, -2, -3, -3);
QwtScaleMap xxMap = xMap;
QwtScaleMap yyMap = yMap;

const QSize res(QwtPlotSpectrogram::rasterHint(area));
if ( res.isValid() )
{
rect.setSize(rect.size().boundedTo(res));
}

int px1 = rect.x();
int px2 = rect.x() + rect.width();
if ( xMap.p1() > xMap.p2() )
qSwap(px1, px2);

double sx1 = area.x();
double sx2 = area.x() + area.width();
if ( xMap.s1() > xMap.s2() )
qSwap(sx1, sx2);

int py1 = rect.y();
int py2 = rect.y() + rect.height();
if ( yMap.p1() > yMap.p2() )
qSwap(py1, py2);

double sy1 = area.y();
double sy2 = area.y() + area.height();
if ( yMap.s1() > yMap.s2() )
qSwap(sy1, sy2);

xxMap.setPaintInterval(px1, px2);
xxMap.setScaleInterval(sx1, sx2);
yyMap.setPaintInterval(py1, py2);
yyMap.setScaleInterval(sy1, sy2);

// Redraw everything if the scale or size changed
if (cachedImage_.isNull()
|| xMap.pDist() != cachedXMap_.pDist()
|| xMap.sDist() != cachedXMap_.sDist()
|| yMap.pDist() != cachedYMap_.pDist()
|| yMap.sDist() != cachedYMap_.sDist()
|| area.width() != cachedArea_.width()
|| area.height() != cachedArea_.height())
{
cachedImage_ = QwtPlotSpectrogram::renderImage(xMap, yMap, area);
cachedXShift_ = 0.0;
cachedYShift_ = 0.0;
}
else
{
QRect xRect, yRect;

// Shift the cached image to compensate for the area change
// and calculate the x and y boxes to redraw
//
// 0
// +------------------------+ ---
// | -yRect | | yShift (negative)
// +---+----------------+---+ ---
// | - | | + | |
// | x | | x | |
// | R | Cached/shifted | R | | height - abs(yShift)
// | e | image | e | |
// | c | | c | |
// | t | | t | |
// | | | | |
// +---+----------------+---+ ---
// | +yRect | | yShift (positive)
// +------------------------+ ---
//
// |---| |---|
// xShift (negative) xShift (positive)

double xScale = xxMap.pDist() / xxMap.sDist();
double yScale = yyMap.pDist() / yyMap.sDist();
double xShift = -(cachedArea_.left() - area.left()) * xScale + cachedXShift_;
double yShift = (cachedArea_.top() - area.top()) * yScale + cachedYShift_;
cachedXShift_ = xShift - ceil(xShift);
cachedYShift_ = yShift - ceil(yShift);
xShift = ceil(xShift);
yShift = ceil(yShift);

// Just redraw everything if the shift size is greater than the image
if (abs(xShift) > rect.width() || abs(yShift) > rect.height())
{
cachedImage_ = QwtPlotSpectrogram::renderImage(xMap, yMap, area);
cachedXShift_ = 0.0;
cachedYShift_ = 0.0;
}
else
{
if (yShift < 0)
{
yRect.setCoords(rect.left(), rect.top(), rect.right(), rect.top() - yShift);

if (xShift < 0)
{
xRect.setCoords(rect.left(), yRect.bottom(), rect.left() - xShift, rect.bottom());
}
else if (xShift > 0)
{
xRect.setCoords(rect.right() - xShift, yRect.bottom(), rect.right(), rect.bottom());
}
}
else if (yShift > 0)
{
yRect.setCoords(rect.left(), rect.bottom() - yShift, rect.right(), rect.bottom());
if (xShift < 0)
{
xRect.setCoords(rect.left(), rect.top(), rect.left() - xShift, yRect.top());
}
else if (xShift > 0)
{
xRect.setCoords(rect.right() - xShift, rect.top(), rect.right(), yRect.top());
}
}
else
{
if (xShift < 0)
{
xRect.setCoords(rect.left(), rect.top(), rect.left() - xShift, rect.bottom());
}
else if (xShift > 0)
{
xRect.setCoords(rect.right() - xShift, rect.top(), rect.right(), rect.bottom());
}
}

// Without this re-adjustment the scale is off by 1 pixel in each direction
rect.adjust(0, 0, 1, 1);
if (!yRect.isNull() || !xRect.isNull())
{
QImage newImage(rect.size(), cachedImage_.format());
QPainter painter(&newImage);
newImage.fill(0);

if (!yRect.isNull())
{
QImage Image;
QRectF sArea = invTransform(xxMap, yyMap, yRect);
Image = QwtPlotSpectrogram::renderImage(xxMap, yyMap, sArea);
painter.drawImage(yRect, Image);
}

if (!xRect.isNull())
{
QImage image;
QRectF sArea = invTransform(xxMap, yyMap, xRect);
image = QwtPlotSpectrogram::renderImage(xxMap, yyMap, sArea);
painter.drawImage(xRect, image);
}

painter.drawImage(rect.translated(-xShift, -yShift), cachedImage_, rect);
cachedImage_ = newImage;
}
}
}

cachedXMap_ = xMap;
cachedYMap_ = yMap;
cachedArea_ = area;
cachedRect_ = rect;

return cachedImage_;
}

private:
mutable QImage cachedImage_;
mutable QwtScaleMap cachedXMap_;
mutable QwtScaleMap cachedYMap_;
mutable QRectF cachedArea_;
mutable QRect cachedRect_;
mutable double cachedXShift_;
mutable double cachedYShift_;
};
#endif // NEWWATERFALLPLOT_H

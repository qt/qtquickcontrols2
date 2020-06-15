/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickcommonstyle.h"
#include "qquickcommonstyle_p.h"
#include "qquickcommonstylepixmaps_p.h"

#include "qquickstyleoption.h"
#include "qquickdrawutil.h"
#include "qquickstylehelper_p.h"

#include <QtGui/QWindow>
#include <qfile.h>
#include <private/qguiapplication_p.h>
#include <qpa/qplatformtheme.h>
#include <qbitmap.h>
#include <qcache.h>
#include <qmath.h>
#include <qpainter.h>
#include <qpaintengine.h>
#include <qpainterpath.h>
#include <private/qmath_p.h>
#include <qdebug.h>
#include <qtextformat.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qsettings.h>
#include <qvariant.h>
#include <qpixmapcache.h>
#include <qmatrix4x4.h>

#include <limits.h>

#include <private/qtextengine_p.h>
#include <QtGui/private/qhexstring_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCommonStyle
    \brief The QCommonStyle class encapsulates the common Look and Feel of a GUI.

    \ingroup appearance
    \inmodule QtWidgets

    This abstract class implements some of the widget's look and feel
    that is common to all GUI styles provided and shipped as part of
    Qt.

    Since QCommonStyle inherits QStyle, all of its functions are fully documented
    in the QStyle documentation.
    \omit
    , although the
    extra functions that QCommonStyle provides, e.g.
    drawComplexControl(), drawControl(), drawPrimitive(),
    hitTestComplexControl(), subControlRect(), sizeFromContents(), and
    subElementRect() are documented here.
    \endomit

    \sa QStyle, QProxyStyle
*/

namespace QQC2 {

QCommonStyle::QCommonStyle()
    : QStyle(*new QCommonStylePrivate)
{ }

QCommonStyle::QCommonStyle(QCommonStylePrivate &dd)
    : QStyle(dd)
{ }

QCommonStyle::~QCommonStyle()
{ }

void QCommonStyle::drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p) const
{
    Q_D(const QCommonStyle);
    switch (pe) {
    case PE_FrameButtonBevel:
    case PE_FrameButtonTool:
        qDrawShadeRect(p, opt->rect, opt->palette,
                       opt->state & (State_Sunken | State_On), 1, 0);
        break;
    case PE_PanelButtonCommand:
    case PE_PanelButtonBevel:
    case PE_PanelButtonTool:
    case PE_IndicatorButtonDropDown:
        qDrawShadePanel(p, opt->rect, opt->palette,
                        opt->state & (State_Sunken | State_On), 1,
                        &opt->palette.brush(QPalette::Button));
        break;
    case PE_IndicatorItemViewItemCheck:
        proxy()->drawPrimitive(PE_IndicatorCheckBox, opt, p);
        break;
    case PE_IndicatorCheckBox:
        if (opt->state & State_NoChange) {
            p->setPen(opt->palette.windowText().color());
            p->fillRect(opt->rect, opt->palette.brush(QPalette::Button));
            p->drawRect(opt->rect);
            p->drawLine(opt->rect.topLeft(), opt->rect.bottomRight());
        } else {
            qDrawShadePanel(p, opt->rect.x(), opt->rect.y(), opt->rect.width(), opt->rect.height(),
                            opt->palette, opt->state & (State_Sunken | State_On), 1,
                            &opt->palette.brush(QPalette::Button));
        }
        break;
    case PE_IndicatorRadioButton: {
        QRect ir = opt->rect;
        p->setPen(opt->palette.dark().color());
        p->drawArc(opt->rect, 0, 5760);
        if (opt->state & (State_Sunken | State_On)) {
            ir.adjust(2, 2, -2, -2);
            p->setBrush(opt->palette.windowText());
            bool oldQt4CompatiblePainting = p->testRenderHint(QPainter::Qt4CompatiblePainting);
            p->setRenderHint(QPainter::Qt4CompatiblePainting);
            p->drawEllipse(ir);
            p->setRenderHint(QPainter::Qt4CompatiblePainting, oldQt4CompatiblePainting);
        }
        break; }
    case PE_FrameFocusRect:
        if (const QStyleOptionFocusRect *fropt = qstyleoption_cast<const QStyleOptionFocusRect *>(opt)) {
            QColor bg = fropt->backgroundColor;
            QPen oldPen = p->pen();
            if (bg.isValid()) {
                int h, s, v;
                bg.getHsv(&h, &s, &v);
                if (v >= 128)
                    p->setPen(Qt::black);
                else
                    p->setPen(Qt::white);
            } else {
                p->setPen(opt->palette.windowText().color());
            }
            QRect focusRect = opt->rect.adjusted(1, 1, -1, -1);
            p->drawRect(focusRect.adjusted(0, 0, -1, -1)); //draw pen inclusive
            p->setPen(oldPen);
        }
        break;
    case PE_IndicatorMenuCheckMark: {
        const int markW = opt->rect.width() > 7 ? 7 : opt->rect.width();
        const int markH = markW;
        int posX = opt->rect.x() + (opt->rect.width() - markW)/2 + 1;
        int posY = opt->rect.y() + (opt->rect.height() - markH)/2;

        QVector<QLineF> a;
        a.reserve(markH);

        int i, xx, yy;
        xx = posX;
        yy = 3 + posY;
        for (i = 0; i < markW/2; ++i) {
            a << QLineF(xx, yy, xx, yy + 2);
            ++xx;
            ++yy;
        }
        yy -= 2;
        for (; i < markH; ++i) {
            a << QLineF(xx, yy, xx, yy + 2);
            ++xx;
            --yy;
        }
        if (!(opt->state & State_Enabled) && !(opt->state & State_On)) {
            p->save();
            p->translate(1, 1);
            p->setPen(opt->palette.light().color());
            p->drawLines(a);
            p->restore();
        }
        p->setPen((opt->state & State_On) ? opt->palette.highlightedText().color() : opt->palette.text().color());
        p->drawLines(a);
        break; }
    case PE_Frame:
    case PE_FrameMenu:
        if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            if (pe == PE_FrameMenu || (frame->state & State_Sunken) || (frame->state & State_Raised)) {
                qDrawShadePanel(p, frame->rect, frame->palette, frame->state & State_Sunken,
                                frame->lineWidth);
            } else {
                qDrawPlainRect(p, frame->rect, frame->palette.windowText().color(), frame->lineWidth);
            }
        }
        break;
    case PE_PanelMenuBar:
        if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(opt)){
            qDrawShadePanel(p, frame->rect, frame->palette, false, frame->lineWidth,
                            &frame->palette.brush(QPalette::Button));

        }
        else if (const QStyleOptionToolBar *frame = qstyleoption_cast<const QStyleOptionToolBar *>(opt)){
            qDrawShadePanel(p, frame->rect, frame->palette, false, frame->lineWidth,
                            &frame->palette.brush(QPalette::Button));
        }

        break;
    case PE_PanelMenu:
        break;
    case PE_PanelToolBar:
       break;
    case PE_IndicatorProgressChunk:
        {
            p->fillRect(opt->rect.x(), opt->rect.y() + 3, opt->rect.width() -2, opt->rect.height() - 6,
                        opt->palette.brush(QPalette::Highlight));
        }
        break;
    case PE_IndicatorBranch: {
        static const int decoration_size = 9;
        int mid_h = opt->rect.x() + opt->rect.width() / 2;
        int mid_v = opt->rect.y() + opt->rect.height() / 2;
        int bef_h = mid_h;
        int bef_v = mid_v;
        int aft_h = mid_h;
        int aft_v = mid_v;
        if (opt->state & State_Children) {
            int delta = decoration_size / 2;
            bef_h -= delta;
            bef_v -= delta;
            aft_h += delta;
            aft_v += delta;
            p->drawLine(bef_h + 2, bef_v + 4, bef_h + 6, bef_v + 4);
            if (!(opt->state & State_Open))
                p->drawLine(bef_h + 4, bef_v + 2, bef_h + 4, bef_v + 6);
            QPen oldPen = p->pen();
            p->setPen(opt->palette.dark().color());
            p->drawRect(bef_h, bef_v, decoration_size - 1, decoration_size - 1);
            p->setPen(oldPen);
        }
        QBrush brush(opt->palette.dark().color(), Qt::Dense4Pattern);
        if (opt->state & State_Item) {
            if (opt->direction == Qt::RightToLeft)
                p->fillRect(opt->rect.left(), mid_v, bef_h - opt->rect.left(), 1, brush);
            else
                p->fillRect(aft_h, mid_v, opt->rect.right() - aft_h + 1, 1, brush);
        }
        if (opt->state & State_Sibling)
            p->fillRect(mid_h, aft_v, 1, opt->rect.bottom() - aft_v + 1, brush);
        if (opt->state & (State_Open | State_Children | State_Item | State_Sibling))
            p->fillRect(mid_h, opt->rect.y(), 1, bef_v - opt->rect.y(), brush);
        break; }
    case PE_FrameStatusBarItem:
        qDrawShadeRect(p, opt->rect, opt->palette, true, 1, 0, nullptr);
        break;
    case PE_IndicatorHeaderArrow:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            QPen oldPen = p->pen();
            if (header->sortIndicator & QStyleOptionHeader::SortUp) {
                p->setPen(QPen(opt->palette.light(), 0));
                p->drawLine(opt->rect.x() + opt->rect.width(), opt->rect.y(),
                            opt->rect.x() + opt->rect.width() / 2, opt->rect.y() + opt->rect.height());
                p->setPen(QPen(opt->palette.dark(), 0));
                const QPoint points[] = {
                    QPoint(opt->rect.x() + opt->rect.width() / 2, opt->rect.y() + opt->rect.height()),
                    QPoint(opt->rect.x(), opt->rect.y()),
                    QPoint(opt->rect.x() + opt->rect.width(), opt->rect.y()),
                };
                p->drawPolyline(points, sizeof points / sizeof *points);
            } else if (header->sortIndicator & QStyleOptionHeader::SortDown) {
                p->setPen(QPen(opt->palette.light(), 0));
                const QPoint points[] = {
                    QPoint(opt->rect.x(), opt->rect.y() + opt->rect.height()),
                    QPoint(opt->rect.x() + opt->rect.width(), opt->rect.y() + opt->rect.height()),
                    QPoint(opt->rect.x() + opt->rect.width() / 2, opt->rect.y()),
                };
                p->drawPolyline(points, sizeof points / sizeof *points);
                p->setPen(QPen(opt->palette.dark(), 0));
                p->drawLine(opt->rect.x(), opt->rect.y() + opt->rect.height(),
                            opt->rect.x() + opt->rect.width() / 2, opt->rect.y());
            }
            p->setPen(oldPen);
        }
        break;
    case PE_FrameTabBarBase:
        if (const QStyleOptionTabBarBase *tbb
                = qstyleoption_cast<const QStyleOptionTabBarBase *>(opt)) {
            p->save();
            switch (tbb->shape) {
            case QStyleOptionTab::RoundedNorth:
            case QStyleOptionTab::TriangularNorth:
                p->setPen(QPen(tbb->palette.light(), 0));
                p->drawLine(tbb->rect.topLeft(), tbb->rect.topRight());
                break;
            case QStyleOptionTab::RoundedWest:
            case QStyleOptionTab::TriangularWest:
                p->setPen(QPen(tbb->palette.light(), 0));
                p->drawLine(tbb->rect.topLeft(), tbb->rect.bottomLeft());
                break;
            case QStyleOptionTab::RoundedSouth:
            case QStyleOptionTab::TriangularSouth:
                p->setPen(QPen(tbb->palette.shadow(), 0));
                p->drawLine(tbb->rect.left(), tbb->rect.bottom(),
                            tbb->rect.right(), tbb->rect.bottom());
                p->setPen(QPen(tbb->palette.dark(), 0));
                p->drawLine(tbb->rect.left(), tbb->rect.bottom() - 1,
                            tbb->rect.right() - 1, tbb->rect.bottom() - 1);
                break;
            case QStyleOptionTab::RoundedEast:
            case QStyleOptionTab::TriangularEast:
                p->setPen(QPen(tbb->palette.dark(), 0));
                p->drawLine(tbb->rect.topRight(), tbb->rect.bottomRight());
                break;
            }
            p->restore();
        }
        break;
    case PE_IndicatorTabClose: {
        if (d->tabBarcloseButtonIcon.isNull()) {
            d->tabBarcloseButtonIcon.addPixmap(QPixmap(
                        QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-closetab-16.png")),
                        QIcon::Normal, QIcon::Off);
            d->tabBarcloseButtonIcon.addPixmap(QPixmap(
                        QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-closetab-down-16.png")),
                        QIcon::Normal, QIcon::On);
            d->tabBarcloseButtonIcon.addPixmap(QPixmap(
                        QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-closetab-hover-16.png")),
                        QIcon::Active, QIcon::Off);
        }

        int size = proxy()->pixelMetric(QStyle::PM_SmallIconSize);
        QIcon::Mode mode = opt->state & State_Enabled ?
                            (opt->state & State_Raised ? QIcon::Active : QIcon::Normal)
                            : QIcon::Disabled;
        if (!(opt->state & State_Raised)
            && !(opt->state & State_Sunken)
            && !(opt->state & QStyle::State_Selected))
            mode = QIcon::Disabled;

        QIcon::State state = opt->state & State_Sunken ? QIcon::On : QIcon::Off;
        QPixmap pixmap = d->tabBarcloseButtonIcon.pixmap(opt->window, QSize(size, size), mode, state);
        proxy()->drawItemPixmap(p, opt->rect, Qt::AlignCenter, pixmap);
        break;
    }
    case PE_FrameTabWidget:
    case PE_FrameWindow:
        qDrawWinPanel(p, opt->rect, opt->palette, false, nullptr);
        break;
    case PE_FrameLineEdit:
        proxy()->drawPrimitive(PE_Frame, opt, p);
        break;
    case PE_FrameGroupBox:
        if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            if (frame->features & QStyleOptionFrame::Flat) {
                QRect fr = frame->rect;
                QPoint p1(fr.x(), fr.y() + 1);
                QPoint p2(fr.x() + fr.width(), p1.y());
                qDrawShadeLine(p, p1, p2, frame->palette, true,
                               frame->lineWidth, frame->midLineWidth);
            } else {
                qDrawShadeRect(p, frame->rect.x(), frame->rect.y(), frame->rect.width(),
                               frame->rect.height(), frame->palette, true,
                               frame->lineWidth, frame->midLineWidth);
            }
        }
        break;
    case PE_FrameDockWidget:
        if (const QStyleOptionFrame *frame = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            int lw = frame->lineWidth;
            if (lw <= 0)
                lw = proxy()->pixelMetric(PM_DockWidgetFrameWidth);

            qDrawShadePanel(p, frame->rect, frame->palette, false, lw);
        }
        break;
    case PE_IndicatorToolBarHandle:
        p->save();
        p->translate(opt->rect.x(), opt->rect.y());
        if (opt->state & State_Horizontal) {
            int x = opt->rect.width() / 3;
            if (opt->direction == Qt::RightToLeft)
                x -= 2;
            if (opt->rect.height() > 4) {
                qDrawShadePanel(p, x, 2, 3, opt->rect.height() - 4,
                                opt->palette, false, 1, nullptr);
                qDrawShadePanel(p, x+3, 2, 3, opt->rect.height() - 4,
                                opt->palette, false, 1, nullptr);
            }
        } else {
            if (opt->rect.width() > 4) {
                int y = opt->rect.height() / 3;
                qDrawShadePanel(p, 2, y, opt->rect.width() - 4, 3,
                                opt->palette, false, 1, nullptr);
                qDrawShadePanel(p, 2, y+3, opt->rect.width() - 4, 3,
                                opt->palette, false, 1, nullptr);
            }
        }
        p->restore();
        break;
    case PE_IndicatorToolBarSeparator:
        {
            QPoint p1, p2;
            if (opt->state & State_Horizontal) {
                p1 = QPoint(opt->rect.width()/2, 0);
                p2 = QPoint(p1.x(), opt->rect.height());
            } else {
                p1 = QPoint(0, opt->rect.height()/2);
                p2 = QPoint(opt->rect.width(), p1.y());
            }
            qDrawShadeLine(p, p1, p2, opt->palette, 1, 1, 0);
            break;
        }
    case PE_IndicatorSpinPlus:
    case PE_IndicatorSpinMinus: {
        QRect r = opt->rect;
        int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt);
        QRect br = r.adjusted(fw, fw, -fw, -fw);

        int offset = (opt->state & State_Sunken) ? 1 : 0;
        int step = (br.width() + 4) / 5;
        p->fillRect(br.x() + offset, br.y() + offset +br.height() / 2 - step / 2,
                    br.width(), step,
                    opt->palette.buttonText());
        if (pe == PE_IndicatorSpinPlus)
            p->fillRect(br.x() + br.width() / 2 - step / 2 + offset, br.y() + offset,
                        step, br.height(),
                        opt->palette.buttonText());

        break; }
    case PE_IndicatorSpinUp:
    case PE_IndicatorSpinDown: {
        QRect r = opt->rect;
        int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt);
        // QRect br = r.adjusted(fw, fw, -fw, -fw);
        int x = r.x(), y = r.y(), w = r.width(), h = r.height();
        int sw = w-4;
        if (sw < 3)
            break;
        else if (!(sw & 1))
            sw--;
        sw -= (sw / 7) * 2;        // Empty border
        int sh = sw/2 + 2;      // Must have empty row at foot of arrow

        int sx = x + w / 2 - sw / 2;
        int sy = y + h / 2 - sh / 2;

        if (pe == PE_IndicatorSpinUp && fw)
            --sy;

        int bsx = 0;
        int bsy = 0;
        if (opt->state & State_Sunken) {
            bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal);
            bsy = proxy()->pixelMetric(PM_ButtonShiftVertical);
        }
        p->save();
        p->translate(sx + bsx, sy + bsy);
        p->setPen(opt->palette.buttonText().color());
        p->setBrush(opt->palette.buttonText());
        p->setRenderHint(QPainter::Qt4CompatiblePainting);
        if (pe == PE_IndicatorSpinDown) {
            const QPoint points[] = { QPoint(0, 1), QPoint(sw-1, 1), QPoint(sh-2, sh-1) };
            p->drawPolygon(points, sizeof points / sizeof *points);
        } else {
            const QPoint points[] = { QPoint(0, sh-1), QPoint(sw-1, sh-1), QPoint(sh-2, 1) };
            p->drawPolygon(points, sizeof points / sizeof *points);
        }
        p->restore();
        break; }
    case PE_PanelTipLabel: {
        const QBrush brush(opt->palette.toolTipBase());
        qDrawPlainRect(p, opt->rect, opt->palette.toolTipText().color(), 1, &brush);
        break;
    }
    case PE_IndicatorTabTear:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            bool rtl = tab->direction == Qt::RightToLeft;
            const bool horizontal = tab->rect.height() > tab->rect.width();
            const int margin = 4;
            QPainterPath path;

            if (horizontal) {
                QRect rect = tab->rect.adjusted(rtl ? margin : 0, 0, rtl ? 1 : -margin, 0);
                rect.setTop(rect.top() + ((tab->state & State_Selected) ? 1 : 3));
                rect.setBottom(rect.bottom() - ((tab->state & State_Selected) ? 0 : 2));

                path.moveTo(QPoint(rtl ? rect.right() : rect.left(), rect.top()));
                int count = 4;
                for (int jags = 1; jags <= count; ++jags, rtl = !rtl)
                    path.lineTo(QPoint(rtl ? rect.left() : rect.right(), rect.top() + jags * rect.height()/count));
            } else {
                QRect rect = tab->rect.adjusted(0, 0, 0, -margin);
                rect.setLeft(rect.left() + ((tab->state & State_Selected) ? 1 : 3));
                rect.setRight(rect.right() - ((tab->state & State_Selected) ? 0 : 2));

                path.moveTo(QPoint(rect.left(), rect.top()));
                int count = 4;
                for (int jags = 1; jags <= count; ++jags, rtl = !rtl)
                    path.lineTo(QPoint(rect.left() + jags * rect.width()/count, rtl ? rect.top() : rect.bottom()));
            }

            p->setPen(QPen(tab->palette.dark(), qreal(.8)));
            p->setBrush(tab->palette.window());
            p->setRenderHint(QPainter::Antialiasing);
            p->drawPath(path);
        }
        break;
    case PE_PanelLineEdit:
        if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            p->fillRect(panel->rect.adjusted(panel->lineWidth, panel->lineWidth, -panel->lineWidth, -panel->lineWidth),
                        panel->palette.brush(QPalette::Base));

            if (panel->lineWidth > 0)
                proxy()->drawPrimitive(PE_FrameLineEdit, panel, p);
        }
        break;
    case PE_IndicatorColumnViewArrow: {
    if (const QStyleOptionViewItem *viewOpt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
        bool reverse = (viewOpt->direction == Qt::RightToLeft);
        p->save();
        QPainterPath path;
        int x = viewOpt->rect.x() + 1;
        int offset = (viewOpt->rect.height() / 3);
        int height = (viewOpt->rect.height()) - offset * 2;
        if (height % 2 == 1)
            --height;
        int x2 = x + height - 1;
        if (reverse) {
            x = viewOpt->rect.x() + viewOpt->rect.width() - 1;
            x2 = x - height + 1;
        }
        path.moveTo(x, viewOpt->rect.y() + offset);
        path.lineTo(x, viewOpt->rect.y() + offset + height);
        path.lineTo(x2, viewOpt->rect.y() + offset+height/2);
        path.closeSubpath();
        if (viewOpt->state & QStyle::State_Selected ) {
            if (viewOpt->showDecorationSelected) {
                QColor color = viewOpt->palette.color(QPalette::Active, QPalette::HighlightedText);
                p->setPen(color);
                p->setBrush(color);
            } else {
                QColor color = viewOpt->palette.color(QPalette::Active, QPalette::WindowText);
                p->setPen(color);
                p->setBrush(color);
            }

        } else {
            QColor color = viewOpt->palette.color(QPalette::Active, QPalette::Mid);
            p->setPen(color);
            p->setBrush(color);
        }
        p->drawPath(path);

        // draw the vertical and top triangle line
        if (!(viewOpt->state & QStyle::State_Selected)) {
            QPainterPath lines;
            lines.moveTo(x, viewOpt->rect.y() + offset);
            lines.lineTo(x, viewOpt->rect.y() + offset + height);
            lines.moveTo(x, viewOpt->rect.y() + offset);
            lines.lineTo(x2, viewOpt->rect.y() + offset+height/2);
            QColor color = viewOpt->palette.color(QPalette::Active, QPalette::Dark);
            p->setPen(color);
            p->drawPath(lines);
        }
        p->restore();
    }
    break; }
    case PE_IndicatorItemViewItemDrop: {
        QRect rect = opt->rect;
        if (opt->rect.height() == 0)
            p->drawLine(rect.topLeft(), rect.topRight());
        else
            p->drawRect(rect);
        break; }
    case PE_PanelItemViewRow:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            QPalette::ColorGroup cg = vopt->state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
                cg = QPalette::Inactive;

            if ((vopt->state & QStyle::State_Selected) &&  proxy()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, opt))
                p->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::Highlight));
            else if (vopt->features & QStyleOptionViewItem::Alternate)
                p->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::AlternateBase));
        }
        break;
    case PE_PanelItemViewItem:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            QPalette::ColorGroup cg = vopt->state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
                cg = QPalette::Inactive;

            if (vopt->showDecorationSelected && (vopt->state & QStyle::State_Selected)) {
                p->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::Highlight));
            } else {
                if (vopt->backgroundBrush.style() != Qt::NoBrush) {
                    QPointF oldBO = p->brushOrigin();
                    p->setBrushOrigin(vopt->rect.topLeft());
                    p->fillRect(vopt->rect, vopt->backgroundBrush);
                    p->setBrushOrigin(oldBO);
                }

                if (vopt->state & QStyle::State_Selected) {
                    QRect textRect = subElementRect(QStyle::SE_ItemViewItemText,  opt);
                    p->fillRect(textRect, vopt->palette.brush(cg, QPalette::Highlight));
                }
            }
        }
        break;
    case PE_PanelScrollAreaCorner: {
        const QBrush brush(opt->palette.brush(QPalette::Window));
        p->fillRect(opt->rect, brush);
        } break;
    case PE_IndicatorArrowUp:
    case PE_IndicatorArrowDown:
    case PE_IndicatorArrowRight:
    case PE_IndicatorArrowLeft:
        {
            if (opt->rect.width() <= 1 || opt->rect.height() <= 1)
                break;
            QRect r = opt->rect;
            int size = qMin(r.height(), r.width());
            QPixmap pixmap;
            QString pixmapName = QStyleHelper::uniqueName(QLatin1String("$qt_ia-")
                                                          % QLatin1String(metaObject()->className()), opt, QSize(size, size))
                                 % HexString<uint>(pe);
            if (!QPixmapCache::find(pixmapName, &pixmap)) {
                qreal pixelRatio = p->device()->devicePixelRatioF();
                int border = qRound(pixelRatio*(size/5));
                int sqsize = qRound(pixelRatio*(2*(size/2)));
                QImage image(sqsize, sqsize, QImage::Format_ARGB32_Premultiplied);
                image.fill(0);
                QPainter imagePainter(&image);

                QPolygon a;
                switch (pe) {
                    case PE_IndicatorArrowUp:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize - border, sqsize/2);
                        break;
                    case PE_IndicatorArrowDown:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, sqsize - border,  sqsize - border, sqsize/2);
                        break;
                    case PE_IndicatorArrowRight:
                        a.setPoints(3, sqsize - border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
                        break;
                    case PE_IndicatorArrowLeft:
                        a.setPoints(3, border, sqsize/2,  sqsize/2, border,  sqsize/2, sqsize - border);
                        break;
                    default:
                        break;
                }

                int bsx = 0;
                int bsy = 0;

                if (opt->state & State_Sunken) {
                    bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt);
                    bsy = proxy()->pixelMetric(PM_ButtonShiftVertical, opt);
                }

                QRect bounds = a.boundingRect();
                int sx = sqsize / 2 - bounds.center().x() - 1;
                int sy = sqsize / 2 - bounds.center().y() - 1;
                imagePainter.translate(sx + bsx, sy + bsy);
                imagePainter.setPen(opt->palette.buttonText().color());
                imagePainter.setBrush(opt->palette.buttonText());
                imagePainter.setRenderHint(QPainter::Qt4CompatiblePainting);

                if (!(opt->state & State_Enabled)) {
                    imagePainter.translate(1, 1);
                    imagePainter.setBrush(opt->palette.light().color());
                    imagePainter.setPen(opt->palette.light().color());
                    imagePainter.drawPolygon(a);
                    imagePainter.translate(-1, -1);
                    imagePainter.setBrush(opt->palette.mid().color());
                    imagePainter.setPen(opt->palette.mid().color());
                }

                imagePainter.drawPolygon(a);
                imagePainter.end();
                pixmap = QPixmap::fromImage(image);
                pixmap.setDevicePixelRatio(pixelRatio);
                QPixmapCache::insert(pixmapName, pixmap);
            }
            int xOffset = r.x() + (r.width() - size)/2;
            int yOffset = r.y() + (r.height() - size)/2;
            p->drawPixmap(xOffset, yOffset, pixmap);
        }
        break;
    default:
        break;
    }
}

static void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton,
                      const QRect &rect, QPainter *painter)
{
    QStyle::PrimitiveElement pe;
    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;
    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;
    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;
    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;
    default:
        return;
    }
    QStyleOption arrowOpt = *toolbutton;
    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter);
}

static QSizeF viewItemTextLayout(QTextLayout &textLayout, int lineWidth, int maxHeight = -1, int *lastVisibleLine = nullptr)
{
    if (lastVisibleLine)
        *lastVisibleLine = -1;
    qreal height = 0;
    qreal widthUsed = 0;
    textLayout.beginLayout();
    int i = 0;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;
        line.setLineWidth(lineWidth);
        line.setPosition(QPointF(0, height));
        height += line.height();
        widthUsed = qMax(widthUsed, line.naturalTextWidth());
        // we assume that the height of the next line is the same as the current one
        if (maxHeight > 0 && lastVisibleLine && height + line.height() > maxHeight) {
            const QTextLine nextLine = textLayout.createLine();
            *lastVisibleLine = nextLine.isValid() ? i : -1;
            break;
        }
        ++i;
    }
    textLayout.endLayout();
    return QSizeF(widthUsed, height);
}

QString QCommonStylePrivate::calculateElidedText(const QString &text, const QTextOption &textOption,
                                                 const QFont &font, const QRect &textRect, const Qt::Alignment valign,
                                                 Qt::TextElideMode textElideMode, int flags,
                                                 bool lastVisibleLineShouldBeElided, QPointF *paintStartPosition) const
{
    QTextLayout textLayout(text, font);
    textLayout.setTextOption(textOption);

    // In AlignVCenter mode when more than one line is displayed and the height only allows
    // some of the lines it makes no sense to display those. From a users perspective it makes
    // more sense to see the start of the text instead something inbetween.
    const bool vAlignmentOptimization = paintStartPosition && valign.testFlag(Qt::AlignVCenter);

    int lastVisibleLine = -1;
    viewItemTextLayout(textLayout, textRect.width(), vAlignmentOptimization ? textRect.height() : -1, &lastVisibleLine);

    const QRectF boundingRect = textLayout.boundingRect();
    // don't care about LTR/RTL here, only need the height
    const QRect layoutRect = QStyle::alignedRect(Qt::LayoutDirectionAuto, valign,
                                                 boundingRect.size().toSize(), textRect);

    if (paintStartPosition)
        *paintStartPosition = QPointF(textRect.x(), layoutRect.top());

    QString ret;
    qreal height = 0;
    const int lineCount = textLayout.lineCount();
    for (int i = 0; i < lineCount; ++i) {
        const QTextLine line = textLayout.lineAt(i);
        height += line.height();

        // above visible rect
        if (height + layoutRect.top() <= textRect.top()) {
            if (paintStartPosition)
                paintStartPosition->ry() += line.height();
            continue;
        }

        const int start = line.textStart();
        const int length = line.textLength();
        const bool drawElided = line.naturalTextWidth() > textRect.width();
        bool elideLastVisibleLine = lastVisibleLine == i;
        if (!drawElided && i + 1 < lineCount && lastVisibleLineShouldBeElided) {
            const QTextLine nextLine = textLayout.lineAt(i + 1);
            const int nextHeight = height + nextLine.height() / 2;
            // elide when less than the next half line is visible
            if (nextHeight + layoutRect.top() > textRect.height() + textRect.top())
                elideLastVisibleLine = true;
        }

        QString text = textLayout.text().mid(start, length);
        if (drawElided || elideLastVisibleLine) {
            if (elideLastVisibleLine) {
                if (text.endsWith(QChar::LineSeparator))
                    text.chop(1);
                text += QChar(0x2026);
            }
            const QStackTextEngine engine(text, font);
            ret += engine.elidedText(textElideMode, textRect.width(), flags);

            // no newline for the last line (last visible or real)
            // sometimes drawElided is true but no eliding is done so the text ends
            // with QChar::LineSeparator - don't add another one. This happened with
            // arabic text in the testcase for QTBUG-72805
            if (i < lineCount - 1 &&
                !ret.endsWith(QChar::LineSeparator))
                ret += QChar::LineSeparator;
        } else {
            ret += text;
        }

        // below visible text, can stop
        if ((height + layoutRect.top() >= textRect.bottom()) ||
                (lastVisibleLine >= 0 && lastVisibleLine == i))
            break;
    }
    return ret;
}

QSize QCommonStylePrivate::viewItemSize(const QStyleOptionViewItem *option, int role) const
{
    switch (role) {
    case Qt::CheckStateRole:
        if (option->features & QStyleOptionViewItem::HasCheckIndicator)
            return QSize(proxyStyle->pixelMetric(QStyle::PM_IndicatorWidth, option),
                         proxyStyle->pixelMetric(QStyle::PM_IndicatorHeight, option));
        break;
    case Qt::DisplayRole:
        if (option->features & QStyleOptionViewItem::HasDisplay) {
            QTextOption textOption;
            textOption.setWrapMode(QTextOption::WordWrap);
            QTextLayout textLayout(option->text, option->font);
            textLayout.setTextOption(textOption);
            const bool wrapText = option->features & QStyleOptionViewItem::WrapText;
            const int textMargin = proxyStyle->pixelMetric(QStyle::PM_FocusFrameHMargin, option) + 1;
            QRect bounds = option->rect;
            switch (option->decorationPosition) {
            case QStyleOptionViewItem::Left:
            case QStyleOptionViewItem::Right: {
                if (wrapText && bounds.isValid()) {
                    int width = bounds.width() - 2 * textMargin;
                    if (option->features & QStyleOptionViewItem::HasDecoration)
                        width -= option->decorationSize.width() + 2 * textMargin;
                    bounds.setWidth(width);
                } else
                    bounds.setWidth(QFIXED_MAX);
                break;
            }
            case QStyleOptionViewItem::Top:
            case QStyleOptionViewItem::Bottom:
                if (wrapText)
                    bounds.setWidth(bounds.isValid() ? bounds.width() - 2 * textMargin : option->decorationSize.width());
                else
                    bounds.setWidth(QFIXED_MAX);
                break;
            default:
                break;
            }

            if (wrapText && option->features & QStyleOptionViewItem::HasCheckIndicator)
                bounds.setWidth(bounds.width() - proxyStyle->pixelMetric(QStyle::PM_IndicatorWidth) - 2 * textMargin);

            const int lineWidth = bounds.width();
            const QSizeF size = viewItemTextLayout(textLayout, lineWidth);
            return QSize(qCeil(size.width()) + 2 * textMargin, qCeil(size.height()));
        }
        break;
    case Qt::DecorationRole:
        if (option->features & QStyleOptionViewItem::HasDecoration) {
            return option->decorationSize;
        }
        break;
    default:
        break;
    }

    return QSize(0, 0);
}

void QCommonStylePrivate::viewItemDrawText(QPainter *p, const QStyleOptionViewItem *option, const QRect &rect) const
{
    const int textMargin = proxyStyle->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr) + 1;

    QRect textRect = rect.adjusted(textMargin, 0, -textMargin, 0); // remove width padding
    const bool wrapText = option->features & QStyleOptionViewItem::WrapText;
    QTextOption textOption;
    textOption.setWrapMode(wrapText ? QTextOption::WordWrap : QTextOption::ManualWrap);
    textOption.setTextDirection(option->direction);
    textOption.setAlignment(QStyle::visualAlignment(option->direction, option->displayAlignment));

    QPointF paintPosition;
    const QString newText = calculateElidedText(option->text, textOption,
                                                option->font, textRect, option->displayAlignment,
                                                option->textElideMode, 0,
                                                true, &paintPosition);

    QTextLayout textLayout(newText, option->font);
    textLayout.setTextOption(textOption);
    viewItemTextLayout(textLayout, textRect.width());
    textLayout.draw(p, paintPosition);
}

/*! \internal
    compute the position for the different component of an item (pixmap, text, checkbox)

    Set sizehint to false to layout the elements inside opt->rect. Set sizehint to true to ignore
   opt->rect and return rectangles in infinite space

    Code duplicated in QItemDelegate::doLayout
*/
void QCommonStylePrivate::viewItemLayout(const QStyleOptionViewItem *opt,  QRect *checkRect,
                                         QRect *pixmapRect, QRect *textRect, bool sizehint) const
{
    Q_ASSERT(checkRect && pixmapRect && textRect);
    *pixmapRect = QRect(QPoint(0, 0), viewItemSize(opt, Qt::DecorationRole));
    *textRect = QRect(QPoint(0, 0), viewItemSize(opt, Qt::DisplayRole));
    *checkRect = QRect(QPoint(0, 0), viewItemSize(opt, Qt::CheckStateRole));

    const bool hasCheck = checkRect->isValid();
    const bool hasPixmap = pixmapRect->isValid();
    const bool hasText = textRect->isValid();
    const bool hasMargin = (hasText | hasPixmap | hasCheck);
    const int frameHMargin = hasMargin ?
                proxyStyle->pixelMetric(QStyle::PM_FocusFrameHMargin, opt) + 1 : 0;
    const int textMargin = hasText ? frameHMargin : 0;
    const int pixmapMargin = hasPixmap ? frameHMargin : 0;
    const int checkMargin = hasCheck ? frameHMargin : 0;
    const int x = opt->rect.left();
    const int y = opt->rect.top();
    int w, h;

    if (textRect->height() == 0 && (!hasPixmap || !sizehint)) {
        //if there is no text, we still want to have a decent height for the item sizeHint and the editor size
        textRect->setHeight(opt->fontMetrics.height());
    }

    QSize pm(0, 0);
    if (hasPixmap) {
        pm = pixmapRect->size();
        pm.rwidth() += 2 * pixmapMargin;
    }
    if (sizehint) {
        h = qMax(checkRect->height(), qMax(textRect->height(), pm.height()));
        if (opt->decorationPosition == QStyleOptionViewItem::Left
            || opt->decorationPosition == QStyleOptionViewItem::Right) {
            w = textRect->width() + pm.width();
        } else {
            w = qMax(textRect->width(), pm.width());
        }
    } else {
        w = opt->rect.width();
        h = opt->rect.height();
    }

    int cw = 0;
    QRect check;
    if (hasCheck) {
        cw = checkRect->width() + 2 * checkMargin;
        if (sizehint) w += cw;
        if (opt->direction == Qt::RightToLeft) {
            check.setRect(x + w - cw, y, cw, h);
        } else {
            check.setRect(x, y, cw, h);
        }
    }

    QRect display;
    QRect decoration;
    switch (opt->decorationPosition) {
    case QStyleOptionViewItem::Top: {
        if (hasPixmap)
            pm.setHeight(pm.height() + pixmapMargin); // add space
        h = sizehint ? textRect->height() : h - pm.height();

        if (opt->direction == Qt::RightToLeft) {
            decoration.setRect(x, y, w - cw, pm.height());
            display.setRect(x, y + pm.height(), w - cw, h);
        } else {
            decoration.setRect(x + cw, y, w - cw, pm.height());
            display.setRect(x + cw, y + pm.height(), w - cw, h);
        }
        break; }
    case QStyleOptionViewItem::Bottom: {
        if (hasText)
            textRect->setHeight(textRect->height() + textMargin); // add space
        h = sizehint ? textRect->height() + pm.height() : h;

        if (opt->direction == Qt::RightToLeft) {
            display.setRect(x, y, w - cw, textRect->height());
            decoration.setRect(x, y + textRect->height(), w - cw, h - textRect->height());
        } else {
            display.setRect(x + cw, y, w - cw, textRect->height());
            decoration.setRect(x + cw, y + textRect->height(), w - cw, h - textRect->height());
        }
        break; }
    case QStyleOptionViewItem::Left: {
        if (opt->direction == Qt::LeftToRight) {
            decoration.setRect(x + cw, y, pm.width(), h);
            display.setRect(decoration.right() + 1, y, w - pm.width() - cw, h);
        } else {
            display.setRect(x, y, w - pm.width() - cw, h);
            decoration.setRect(display.right() + 1, y, pm.width(), h);
        }
        break; }
    case QStyleOptionViewItem::Right: {
        if (opt->direction == Qt::LeftToRight) {
            display.setRect(x + cw, y, w - pm.width() - cw, h);
            decoration.setRect(display.right() + 1, y, pm.width(), h);
        } else {
            decoration.setRect(x, y, pm.width(), h);
            display.setRect(decoration.right() + 1, y, w - pm.width() - cw, h);
        }
        break; }
    default:
        qWarning("doLayout: decoration position is invalid");
        decoration = *pixmapRect;
        break;
    }

    if (!sizehint) { // we only need to do the internal layout if we are going to paint
        *checkRect = QStyle::alignedRect(opt->direction, Qt::AlignCenter,
                                         checkRect->size(), check);
        *pixmapRect = QStyle::alignedRect(opt->direction, opt->decorationAlignment,
                                          pixmapRect->size(), decoration);
        // the text takes up all available space, unless the decoration is not shown as selected
        if (opt->showDecorationSelected)
            *textRect = display;
        else
            *textRect = QStyle::alignedRect(opt->direction, opt->displayAlignment,
                                            textRect->size().boundedTo(display.size()), display);
    } else {
        *checkRect = check;
        *pixmapRect = decoration;
        *textRect = display;
    }
}

QString QCommonStylePrivate::toolButtonElideText(const QStyleOptionToolButton *option,
                                                 const QRect &textRect, int flags) const
{
    if (option->fontMetrics.horizontalAdvance(option->text) <= textRect.width())
        return option->text;

    QString text = option->text;
    text.replace(QLatin1Char('\n'), QChar::LineSeparator);
    QTextOption textOption;
    textOption.setWrapMode(QTextOption::ManualWrap);
    textOption.setTextDirection(option->direction);

    return calculateElidedText(text, textOption,
                               option->font, textRect, Qt::AlignTop,
                               Qt::ElideMiddle, flags,
                               false, nullptr);
}

/*! \internal
    Compute the textRect and the pixmapRect from the opt rect

    Uses the same computation than in QTabBar::tabSizeHint
 */
void QCommonStylePrivate::tabLayout(const QStyleOptionTab *opt, QRect *textRect, QRect *iconRect) const
{
    Q_ASSERT(textRect);
    Q_ASSERT(iconRect);
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QStyleOptionTab::RoundedEast
                        || opt->shape == QStyleOptionTab::RoundedWest
                        || opt->shape == QStyleOptionTab::TriangularEast
                        || opt->shape == QStyleOptionTab::TriangularWest;
    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width()); // 0, 0 as we will have a translate transform

    int verticalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt);
    int horizontalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt);
    int hpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabHSpace, opt) / 2;
    int vpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabVSpace, opt) / 2;
    if (opt->shape == QStyleOptionTab::RoundedSouth || opt->shape == QStyleOptionTab::TriangularSouth)
        verticalShift = -verticalShift;
    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);
    bool selected = opt->state & QStyle::State_Selected;
    if (selected) {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty()) {
        tr.setLeft(tr.left() + 4 +
            (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty()) {
        tr.setRight(tr.right() - 4 -
            (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull()) {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid()) {
            int iconExtent = proxyStyle->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize = opt->icon.actualSize(iconSize,
                        (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                        (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);
        // High-dpi icons do not need adjustment; make sure tabIconSize is not larger than iconSize
        tabIconSize = QSize(qMin(tabIconSize.width(), iconSize.width()), qMin(tabIconSize.height(), iconSize.height()));

        const int offsetX = (iconSize.width() - tabIconSize.width()) / 2;
        *iconRect = QRect(tr.left() + offsetX, tr.center().y() - tabIconSize.height() / 2,
                          tabIconSize.width(), tabIconSize.height());
        if (!verticalTabs)
            *iconRect = QStyle::visualRect(opt->direction, opt->rect, *iconRect);
        tr.setLeft(tr.left() + tabIconSize.width() + 4);
    }

    if (!verticalTabs)
        tr = QStyle::visualRect(opt->direction, opt->rect, tr);

    *textRect = tr;
}

/*!
  \reimp
*/
void QCommonStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p) const
{
    Q_D(const QCommonStyle);

    // TODO: Set opt->window manually for now before calling any of the drawing functions. opt->window is
    // pulled of the widget is QStyle. But now that we have no widget, we need some other
    // solution.
    Q_ASSERT(opt->window);

    switch (element) {

    case CE_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            proxy()->drawControl(CE_PushButtonBevel, btn, p);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(SE_PushButtonContents, btn);
            proxy()->drawControl(CE_PushButtonLabel, &subopt, p);
            if (btn->state & State_HasFocus) {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*btn);
                fropt.rect = subElementRect(SE_PushButtonFocusRect, btn);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, p);
            }
        }
        break;
    case CE_PushButtonBevel:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QRect br = btn->rect;
            int dbi = proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn);
            if (btn->features & QStyleOptionButton::DefaultButton)
                proxy()->drawPrimitive(PE_FrameDefaultButton, opt, p);
            if (btn->features & QStyleOptionButton::AutoDefaultButton)
                br.setCoords(br.left() + dbi, br.top() + dbi, br.right() - dbi, br.bottom() - dbi);
            if (!(btn->features & (QStyleOptionButton::Flat | QStyleOptionButton::CommandLinkButton))
                || btn->state & (State_Sunken | State_On)
                || (btn->features & QStyleOptionButton::CommandLinkButton && btn->state & State_MouseOver)) {
                QStyleOptionButton tmpBtn = *btn;
                tmpBtn.rect = br;
                proxy()->drawPrimitive(PE_PanelButtonCommand, &tmpBtn, p);
            }
            if (btn->features & QStyleOptionButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, btn);
                QRect ir = btn->rect;
                QStyleOptionButton newBtn = *btn;
                newBtn.rect = QRect(ir.right() - mbi + 2, ir.height()/2 - mbi/2 + 3, mbi - 6, mbi - 6);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p);
            }
        }
        break;
 case CE_PushButtonLabel:
        if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            QRect textRect = button->rect;
            uint tf = Qt::AlignVCenter | Qt::TextShowMnemonic;
            if (!proxy()->styleHint(SH_UnderlineShortcut, button))
                tf |= Qt::TextHideMnemonic;

            if (!button->icon.isNull()) {
                //Center both icon and text
                QIcon::Mode mode = button->state & State_Enabled ? QIcon::Normal : QIcon::Disabled;
                if (mode == QIcon::Normal && button->state & State_HasFocus)
                    mode = QIcon::Active;
                QIcon::State state = QIcon::Off;
                if (button->state & State_On)
                    state = QIcon::On;

                QPixmap pixmap = button->icon.pixmap(opt->window, button->iconSize, mode, state);
                int pixmapWidth = pixmap.width() / pixmap.devicePixelRatio();
                int pixmapHeight = pixmap.height() / pixmap.devicePixelRatio();
                int labelWidth = pixmapWidth;
                int labelHeight = pixmapHeight;
                int iconSpacing = 4;//### 4 is currently hardcoded in QPushButton::sizeHint()
                if (!button->text.isEmpty()) {
                    int textWidth = button->fontMetrics.boundingRect(opt->rect, tf, button->text).width();
                    labelWidth += (textWidth + iconSpacing);
                }

                QRect iconRect = QRect(textRect.x() + (textRect.width() - labelWidth) / 2,
                                       textRect.y() + (textRect.height() - labelHeight) / 2,
                                       pixmapWidth, pixmapHeight);

                iconRect = visualRect(button->direction, textRect, iconRect);

                if (button->direction == Qt::RightToLeft) {
                    tf |= Qt::AlignRight;
                    textRect.setRight(iconRect.left() - iconSpacing);
                } else {
                    tf |= Qt::AlignLeft; //left align, we adjust the text-rect instead
                    textRect.setLeft(iconRect.left() + iconRect.width() + iconSpacing);
                }

                if (button->state & (State_On | State_Sunken))
                    iconRect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt),
                                       proxy()->pixelMetric(PM_ButtonShiftVertical, opt));
                p->drawPixmap(iconRect, pixmap);
            } else {
                tf |= Qt::AlignHCenter;
            }
            if (button->state & (State_On | State_Sunken))
                textRect.translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, opt),
                             proxy()->pixelMetric(PM_ButtonShiftVertical, opt));

            if (button->features & QStyleOptionButton::HasMenu) {
                int indicatorSize = proxy()->pixelMetric(PM_MenuButtonIndicator, button);
                if (button->direction == Qt::LeftToRight)
                    textRect = textRect.adjusted(0, 0, -indicatorSize, 0);
                else
                    textRect = textRect.adjusted(indicatorSize, 0, 0, 0);
            }
            proxy()->drawItemText(p, textRect, tf, button->palette, (button->state & State_Enabled),
                         button->text, QPalette::ButtonText);
        }
        break;
    case CE_RadioButton:
    case CE_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            bool isRadio = (element == CE_RadioButton);
            QStyleOptionButton subopt = *btn;
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonIndicator
                                                 : SE_CheckBoxIndicator, btn);
            proxy()->drawPrimitive(isRadio ? PE_IndicatorRadioButton : PE_IndicatorCheckBox,
                          &subopt, p);
            subopt.rect = subElementRect(isRadio ? SE_RadioButtonContents
                                                 : SE_CheckBoxContents, btn);
            proxy()->drawControl(isRadio ? CE_RadioButtonLabel : CE_CheckBoxLabel, &subopt, p);
            if (btn->state & State_HasFocus) {
                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*btn);
                fropt.rect = subElementRect(isRadio ? SE_RadioButtonFocusRect
                                                    : SE_CheckBoxFocusRect, btn);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, p);
            }
        }
        break;
    case CE_RadioButtonLabel:
    case CE_CheckBoxLabel:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);

            if (!proxy()->styleHint(SH_UnderlineShortcut, btn))
                alignment |= Qt::TextHideMnemonic;
            QPixmap pix;
            QRect textRect = btn->rect;
            if (!btn->icon.isNull()) {
                pix = btn->icon.pixmap(opt->window, btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
                proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
                if (btn->direction == Qt::RightToLeft)
                    textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
                else
                    textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
            }
            if (!btn->text.isEmpty()){
                proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic,
                    btn->palette, btn->state & State_Enabled, btn->text, QPalette::WindowText);
            }
        }
        break;
    case CE_MenuScroller: {
        QStyleOption arrowOpt = *opt;
        arrowOpt.state |= State_Enabled;
        proxy()->drawPrimitive(((opt->state & State_DownArrow) ? PE_IndicatorArrowDown : PE_IndicatorArrowUp), &arrowOpt, p);
        break; }
    case CE_MenuTearoff:
        if (opt->state & State_Selected)
            p->fillRect(opt->rect, opt->palette.brush(QPalette::Highlight));
        else
            p->fillRect(opt->rect, opt->palette.brush(QPalette::Button));
        p->setPen(QPen(opt->palette.dark().color(), 1, Qt::DashLine));
        p->drawLine(opt->rect.x() + 2, opt->rect.y() + opt->rect.height() / 2 - 1,
                    opt->rect.x() + opt->rect.width() - 4,
                    opt->rect.y() + opt->rect.height() / 2 - 1);
        p->setPen(QPen(opt->palette.light().color(), 1, Qt::DashLine));
        p->drawLine(opt->rect.x() + 2, opt->rect.y() + opt->rect.height() / 2,
                    opt->rect.x() + opt->rect.width() - 4, opt->rect.y() + opt->rect.height() / 2);
        break;
    case CE_MenuBarItem:
        if (const QStyleOptionMenuItem *mbi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            uint alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextDontClip
                            | Qt::TextSingleLine;
            if (!proxy()->styleHint(SH_UnderlineShortcut, mbi))
                alignment |= Qt::TextHideMnemonic;
            int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
            QPixmap pix = mbi->icon.pixmap(opt->window, QSize(iconExtent, iconExtent), (mbi->state & State_Enabled) ? QIcon::Normal : QIcon::Disabled);
            if (!pix.isNull())
                proxy()->drawItemPixmap(p,mbi->rect, alignment, pix);
            else
                proxy()->drawItemText(p, mbi->rect, alignment, mbi->palette, mbi->state & State_Enabled,
                             mbi->text, QPalette::ButtonText);
        }
        break;
    case CE_MenuBarEmptyArea:
        break;
    case CE_ProgressBar:
        if (const QStyleOptionProgressBar *pb
                = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            QStyleOptionProgressBar subopt = *pb;
            subopt.rect = subElementRect(SE_ProgressBarGroove, pb);
            proxy()->drawControl(CE_ProgressBarGroove, &subopt, p);
            subopt.rect = subElementRect(SE_ProgressBarContents, pb);
            proxy()->drawControl(CE_ProgressBarContents, &subopt, p);
            if (pb->textVisible) {
                subopt.rect = subElementRect(SE_ProgressBarLabel, pb);
                proxy()->drawControl(CE_ProgressBarLabel, &subopt, p);
            }
        }
        break;
    case CE_ProgressBarGroove:
        if (opt->rect.isValid())
            qDrawShadePanel(p, opt->rect, opt->palette, true, 1,
                            &opt->palette.brush(QPalette::Window));
        break;
    case CE_ProgressBarLabel:
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            QPalette::ColorRole textRole = QPalette::NoRole;
            if ((pb->textAlignment & Qt::AlignCenter) && pb->textVisible
                    && ((qint64(pb->progress) - qint64(pb->minimum)) * 2 >= (qint64(pb->maximum) - qint64(pb->minimum)))) {
                textRole = QPalette::HighlightedText;
                //Draw text shadow, This will increase readability when the background of same color
                QRect shadowRect(pb->rect);
                shadowRect.translate(1,1);
                QColor shadowColor = (pb->palette.color(textRole).value() <= 128)
                        ? QColor(255,255,255,160) : QColor(0,0,0,160);
                QPalette shadowPalette = pb->palette;
                shadowPalette.setColor(textRole, shadowColor);
                proxy()->drawItemText(p, shadowRect, Qt::AlignCenter | Qt::TextSingleLine, shadowPalette,
                                      pb->state & State_Enabled, pb->text, textRole);
            }
            proxy()->drawItemText(p, pb->rect, Qt::AlignCenter | Qt::TextSingleLine, pb->palette,
                                  pb->state & State_Enabled, pb->text, textRole);
        }
        break;
    case CE_ProgressBarContents:
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {

            QRect rect = pb->rect;
            const bool inverted = pb->invertedAppearance;
            qint64 minimum = qint64(pb->minimum);
            qint64 maximum = qint64(pb->maximum);
            qint64 progress = qint64(pb->progress);

            QPalette pal2 = pb->palette;
            // Correct the highlight color if it is the same as the background
            if (pal2.highlight() == pal2.window())
                pal2.setColor(QPalette::Highlight, pb->palette.color(QPalette::Active,
                                                                     QPalette::Highlight));
            bool reverse = pb->direction == Qt::RightToLeft;
            if (inverted)
                reverse = !reverse;
            int w = rect.width();
            if (pb->minimum == 0 && pb->maximum == 0) {
                // draw busy indicator
                int x = (progress - minimum) % (w * 2);
                if (x > w)
                    x = 2 * w - x;
                x = reverse ? rect.right() - x : x + rect.x();
                p->setPen(QPen(pal2.highlight().color(), 4));
                p->drawLine(x, rect.y(), x, rect.height());
            } else {
                const int unit_width = proxy()->pixelMetric(PM_ProgressBarChunkWidth, pb);
                if (!unit_width)
                    return;

                int u;
                if (unit_width > 1)
                    u = ((rect.width() + unit_width) / unit_width);
                else
                    u = w / unit_width;
                qint64 p_v = progress - minimum;
                qint64 t_s = (maximum - minimum) ? (maximum - minimum) : qint64(1);

                if (u > 0 && p_v >= INT_MAX / u && t_s >= u) {
                    // scale down to something usable.
                    p_v /= u;
                    t_s /= u;
                }

                // nu < tnu, if last chunk is only a partial chunk
                int tnu, nu;
                tnu = nu = p_v * u / t_s;

                if (nu * unit_width > w)
                    --nu;

                // Draw nu units out of a possible u of unit_width
                // width, each a rectangle bordered by background
                // color, all in a sunken panel with a percentage text
                // display at the end.
                int x = 0;
                int x0 = reverse ? rect.right() - ((unit_width > 1) ? unit_width : 0)
                                 : rect.x();

                QStyleOptionProgressBar pbBits = *pb;
                pbBits.rect = rect;
                pbBits.palette = pal2;
                int myY = pbBits.rect.y();
                int myHeight = pbBits.rect.height();
                pbBits.state = State_None;
                QMatrix4x4 m;
                for (int i = 0; i < nu; ++i) {
                    pbBits.rect.setRect(x0 + x, myY, unit_width, myHeight);
                    pbBits.rect = m.mapRect(QRectF(pbBits.rect)).toRect();
                    proxy()->drawPrimitive(PE_IndicatorProgressChunk, &pbBits, p);
                    x += reverse ? -unit_width : unit_width;
                }

                // Draw the last partial chunk to fill up the
                // progress bar entirely
                if (nu < tnu) {
                    int pixels_left = w - (nu * unit_width);
                    int offset = reverse ? x0 + x + unit_width-pixels_left : x0 + x;
                    pbBits.rect.setRect(offset, myY, pixels_left, myHeight);
                    pbBits.rect = m.mapRect(QRectF(pbBits.rect)).toRect();
                    proxy()->drawPrimitive(PE_IndicatorProgressChunk, &pbBits, p);
                }
            }
        }
        break;
    case CE_HeaderLabel:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            QRect rect = header->rect;
            if (!header->icon.isNull()) {
                int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                QPixmap pixmap
                    = header->icon.pixmap(opt->window, QSize(iconExtent, iconExtent), (header->state & State_Enabled) ? QIcon::Normal : QIcon::Disabled);
                int pixw = pixmap.width() / pixmap.devicePixelRatio();

                QRect aligned = alignedRect(header->direction, QFlag(header->iconAlignment), pixmap.size() / pixmap.devicePixelRatio(), rect);
                QRect inter = aligned.intersected(rect);
                p->drawPixmap(inter.x(), inter.y(), pixmap,
                              inter.x() - aligned.x(), inter.y() - aligned.y(),
                              aligned.width() * pixmap.devicePixelRatio(),
                              pixmap.height() * pixmap.devicePixelRatio());

                const int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, opt);
                if (header->direction == Qt::LeftToRight)
                    rect.setLeft(rect.left() + pixw + margin);
                else
                    rect.setRight(rect.right() - pixw - margin);
            }
            if (header->state & QStyle::State_On) {
                QFont fnt = p->font();
                fnt.setBold(true);
                p->setFont(fnt);
            }
            proxy()->drawItemText(p, rect, header->textAlignment, header->palette,
                         (header->state & State_Enabled), header->text, QPalette::ButtonText);
        }
        break;
    case CE_ToolButtonLabel:
        if (const QStyleOptionToolButton *toolbutton
                = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect rect = toolbutton->rect;
            int shiftX = 0;
            int shiftY = 0;
            if (toolbutton->state & (State_Sunken | State_On)) {
                shiftX = proxy()->pixelMetric(PM_ButtonShiftHorizontal, toolbutton);
                shiftY = proxy()->pixelMetric(PM_ButtonShiftVertical, toolbutton);
            }
            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty())
                || toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {
                int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!proxy()->styleHint(SH_UnderlineShortcut, opt))
                    alignment |= Qt::TextHideMnemonic;
                rect.translate(shiftX, shiftY);
                p->setFont(toolbutton->font);
                proxy()->drawItemText(p, rect, alignment, toolbutton->palette,
                             opt->state & State_Enabled, toolbutton->text,
                             QPalette::ButtonText);
            } else {
                QPixmap pm;
                QSize pmSize = toolbutton->iconSize;
                if (!toolbutton->icon.isNull()) {
                    QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                    QIcon::Mode mode;
                    if (!(toolbutton->state & State_Enabled))
                        mode = QIcon::Disabled;
                    else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                        mode = QIcon::Active;
                    else
                        mode = QIcon::Normal;
                    pm = toolbutton->icon.pixmap(opt->window, toolbutton->rect.size().boundedTo(toolbutton->iconSize), mode, state);
                    pmSize = pm.size() / pm.devicePixelRatio();
                }

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) {
                    p->setFont(toolbutton->font);
                    QRect pr = rect,
                    tr = rect;
                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, opt))
                        alignment |= Qt::TextHideMnemonic;

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                        pr.setHeight(pmSize.height() + 4); //### 4 is currently hardcoded in QToolButton::sizeHint()
                        tr.adjust(0, pr.height() - 1, 0, -1);
                        pr.translate(shiftX, shiftY);
                        if (!hasArrow) {
                            proxy()->drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                        } else {
                            drawArrow(proxy(), toolbutton, pr, p);
                        }
                        alignment |= Qt::AlignCenter;
                    } else {
                        pr.setWidth(pmSize.width() + 4); //### 4 is currently hardcoded in QToolButton::sizeHint()
                        tr.adjust(pr.width(), 0, 0, 0);
                        pr.translate(shiftX, shiftY);
                        if (!hasArrow) {
                            proxy()->drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                        } else {
                            drawArrow(proxy(), toolbutton, pr, p);
                        }
                        alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                    }
                    tr.translate(shiftX, shiftY);
                    const QString text = d->toolButtonElideText(toolbutton, tr, alignment);
                    proxy()->drawItemText(p, QStyle::visualRect(opt->direction, rect, tr), alignment, toolbutton->palette,
                                 toolbutton->state & State_Enabled, text,
                                 QPalette::ButtonText);
                } else {
                    rect.translate(shiftX, shiftY);
                    if (hasArrow) {
                        drawArrow(proxy(), toolbutton, rect, p);
                    } else {
                        proxy()->drawItemPixmap(p, rect, Qt::AlignCenter, pm);
                    }
                }
            }
        }
        break;
    case CE_ToolBoxTab:
        if (const QStyleOptionToolBox *tb = qstyleoption_cast<const QStyleOptionToolBox *>(opt)) {
            proxy()->drawControl(CE_ToolBoxTabShape, tb, p);
            proxy()->drawControl(CE_ToolBoxTabLabel, tb, p);
        }
        break;
    case CE_ToolBoxTabShape:
        if (const QStyleOptionToolBox *tb = qstyleoption_cast<const QStyleOptionToolBox *>(opt)) {
            p->setPen(tb->palette.mid().color().darker(150));
            bool oldQt4CompatiblePainting = p->testRenderHint(QPainter::Qt4CompatiblePainting);
            p->setRenderHint(QPainter::Qt4CompatiblePainting);
            int d = 20 + tb->rect.height() - 3;
            if (tb->direction != Qt::RightToLeft) {
                const QPoint points[] = {
                    QPoint(-1, tb->rect.height() + 1),
                    QPoint(-1, 1),
                    QPoint(tb->rect.width() - d, 1),
                    QPoint(tb->rect.width() - 20, tb->rect.height() - 2),
                    QPoint(tb->rect.width() - 1, tb->rect.height() - 2),
                    QPoint(tb->rect.width() - 1, tb->rect.height() + 1),
                    QPoint(-1, tb->rect.height() + 1),
                };
                p->drawPolygon(points, sizeof points / sizeof *points);
            } else {
                const QPoint points[] = {
                    QPoint(tb->rect.width(), tb->rect.height() + 1),
                    QPoint(tb->rect.width(), 1),
                    QPoint(d - 1, 1),
                    QPoint(20 - 1, tb->rect.height() - 2),
                    QPoint(0, tb->rect.height() - 2),
                    QPoint(0, tb->rect.height() + 1),
                    QPoint(tb->rect.width(), tb->rect.height() + 1),
                };
                p->drawPolygon(points, sizeof points / sizeof *points);
            }
            p->setRenderHint(QPainter::Qt4CompatiblePainting, oldQt4CompatiblePainting);
            p->setPen(tb->palette.light().color());
            if (tb->direction != Qt::RightToLeft) {
                p->drawLine(0, 2, tb->rect.width() - d, 2);
                p->drawLine(tb->rect.width() - d - 1, 2, tb->rect.width() - 21, tb->rect.height() - 1);
                p->drawLine(tb->rect.width() - 20, tb->rect.height() - 1,
                            tb->rect.width(), tb->rect.height() - 1);
            } else {
                p->drawLine(tb->rect.width() - 1, 2, d - 1, 2);
                p->drawLine(d, 2, 20, tb->rect.height() - 1);
                p->drawLine(19, tb->rect.height() - 1,
                            -1, tb->rect.height() - 1);
            }
            p->setBrush(Qt::NoBrush);
        }
        break;
    case CE_TabBarTab:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            proxy()->drawControl(CE_TabBarTabShape, tab, p);
            proxy()->drawControl(CE_TabBarTabLabel, tab, p);
        }
        break;
    case CE_TabBarTabShape:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            p->save();

            QRect rect(tab->rect);
            bool selected = tab->state & State_Selected;
            bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
            int tabOverlap = onlyOne ? 0 : proxy()->pixelMetric(PM_TabBarTabOverlap, opt);

            if (!selected) {
                switch (tab->shape) {
                case QStyleOptionTab::TriangularNorth:
                    rect.adjust(0, 0, 0, -tabOverlap);
                    if(!selected)
                        rect.adjust(1, 1, -1, 0);
                    break;
                case QStyleOptionTab::TriangularSouth:
                    rect.adjust(0, tabOverlap, 0, 0);
                    if(!selected)
                        rect.adjust(1, 0, -1, -1);
                    break;
                case QStyleOptionTab::TriangularEast:
                    rect.adjust(tabOverlap, 0, 0, 0);
                    if(!selected)
                        rect.adjust(0, 1, -1, -1);
                    break;
                case QStyleOptionTab::TriangularWest:
                    rect.adjust(0, 0, -tabOverlap, 0);
                    if(!selected)
                        rect.adjust(1, 1, 0, -1);
                    break;
                default:
                    break;
                }
            }

            p->setPen(QPen(tab->palette.windowText(), 0));
            if (selected) {
                p->setBrush(tab->palette.base());
            } else {
                p->setBrush(tab->palette.window());
            }

            int y;
            int x;
            QPolygon a(10);
            switch (tab->shape) {
            case QStyleOptionTab::TriangularNorth:
            case QStyleOptionTab::TriangularSouth: {
                a.setPoint(0, 0, -1);
                a.setPoint(1, 0, 0);
                y = rect.height() - 2;
                x = y / 3;
                a.setPoint(2, x++, y - 1);
                ++x;
                a.setPoint(3, x++, y++);
                a.setPoint(4, x, y);

                int i;
                int right = rect.width() - 1;
                for (i = 0; i < 5; ++i)
                    a.setPoint(9 - i, right - a.point(i).x(), a.point(i).y());
                if (tab->shape == QStyleOptionTab::TriangularNorth)
                    for (i = 0; i < 10; ++i)
                        a.setPoint(i, a.point(i).x(), rect.height() - 1 - a.point(i).y());

                a.translate(rect.left(), rect.top());
                p->setRenderHint(QPainter::Antialiasing);
                p->translate(0, 0.5);

                QPainterPath path;
                path.addPolygon(a);
                p->drawPath(path);
                break; }
            case QStyleOptionTab::TriangularEast:
            case QStyleOptionTab::TriangularWest: {
                a.setPoint(0, -1, 0);
                a.setPoint(1, 0, 0);
                x = rect.width() - 2;
                y = x / 3;
                a.setPoint(2, x - 1, y++);
                ++y;
                a.setPoint(3, x++, y++);
                a.setPoint(4, x, y);
                int i;
                int bottom = rect.height() - 1;
                for (i = 0; i < 5; ++i)
                    a.setPoint(9 - i, a.point(i).x(), bottom - a.point(i).y());
                if (tab->shape == QStyleOptionTab::TriangularWest)
                    for (i = 0; i < 10; ++i)
                        a.setPoint(i, rect.width() - 1 - a.point(i).x(), a.point(i).y());
                a.translate(rect.left(), rect.top());
                p->setRenderHint(QPainter::Antialiasing);
                p->translate(0.5, 0);
                QPainterPath path;
                path.addPolygon(a);
                p->drawPath(path);
                break; }
            default:
                break;
            }
            p->restore();
        }
        break;
    case CE_ToolBoxTabLabel:
        if (const QStyleOptionToolBox *tb = qstyleoption_cast<const QStyleOptionToolBox *>(opt)) {
            bool enabled = tb->state & State_Enabled;
            bool selected = tb->state & State_Selected;
            int iconExtent = proxy()->pixelMetric(QStyle::PM_SmallIconSize, tb);
            QPixmap pm = tb->icon.pixmap(opt->window, QSize(iconExtent, iconExtent),
                                         enabled ? QIcon::Normal : QIcon::Disabled);

            QRect cr = subElementRect(QStyle::SE_ToolBoxTabContents, tb);
            QRect tr, ir;
            int ih = 0;
            if (pm.isNull()) {
                tr = cr;
                tr.adjust(4, 0, -8, 0);
            } else {
                int iw = pm.width() / pm.devicePixelRatio() + 4;
                ih = pm.height()/ pm.devicePixelRatio();
                ir = QRect(cr.left() + 4, cr.top(), iw + 2, ih);
                tr = QRect(ir.right(), cr.top(), cr.width() - ir.right() - 4, cr.height());
            }

            if (selected && proxy()->styleHint(QStyle::SH_ToolBox_SelectedPageTitleBold, tb)) {
                QFont f(p->font());
                f.setBold(true);
                p->setFont(f);
            }

            QString txt = tb->fontMetrics.elidedText(tb->text, Qt::ElideRight, tr.width());

            if (ih)
                p->drawPixmap(ir.left(), (tb->rect.height() - ih) / 2, pm);

            int alignment = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic;
            if (!proxy()->styleHint(QStyle::SH_UnderlineShortcut, tb))
                alignment |= Qt::TextHideMnemonic;
            proxy()->drawItemText(p, tr, alignment, tb->palette, enabled, txt, QPalette::ButtonText);

            if (!txt.isEmpty() && opt->state & State_HasFocus) {
                QStyleOptionFocusRect opt;
                opt.rect = tr;
                opt.palette = tb->palette;
                opt.state = QStyle::State_None;
                proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &opt, p);
            }
        }
        break;
    case CE_TabBarTabLabel:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            QRect tr = tab->rect;
            bool verticalTabs = tab->shape == QStyleOptionTab::RoundedEast
                                || tab->shape == QStyleOptionTab::RoundedWest
                                || tab->shape == QStyleOptionTab::TriangularEast
                                || tab->shape == QStyleOptionTab::TriangularWest;

            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
            if (!proxy()->styleHint(SH_UnderlineShortcut, opt))
                alignment |= Qt::TextHideMnemonic;

            if (verticalTabs) {
                p->save();
                int newX, newY, newRot;
                if (tab->shape == QStyleOptionTab::RoundedEast || tab->shape == QStyleOptionTab::TriangularEast) {
                    newX = tr.width() + tr.x();
                    newY = tr.y();
                    newRot = 90;
                } else {
                    newX = tr.x();
                    newY = tr.y() + tr.height();
                    newRot = -90;
                }
                QTransform m = QTransform::fromTranslate(newX, newY);
                m.rotate(newRot);
                p->setTransform(m, true);
            }
            QRect iconRect;
            d->tabLayout(tab, &tr, &iconRect);
            tr = proxy()->subElementRect(SE_TabBarTabText, opt); //we compute tr twice because the style may override subElementRect

            if (!tab->icon.isNull()) {
                QPixmap tabIcon = tab->icon.pixmap(opt->window, tab->iconSize,
                                                   (tab->state & State_Enabled) ? QIcon::Normal
                                                                                : QIcon::Disabled,
                                                   (tab->state & State_Selected) ? QIcon::On
                                                                                 : QIcon::Off);
                p->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
            }

            proxy()->drawItemText(p, tr, alignment, tab->palette, tab->state & State_Enabled, tab->text, QPalette::WindowText);
            if (verticalTabs)
                p->restore();

            if (tab->state & State_HasFocus) {
                const int OFFSET = 1 + pixelMetric(PM_DefaultFrameWidth);

                int x1, x2;
                x1 = tab->rect.left();
                x2 = tab->rect.right() - 1;

                QStyleOptionFocusRect fropt;
                fropt.QStyleOption::operator=(*tab);
                fropt.rect.setRect(x1 + 1 + OFFSET, tab->rect.y() + OFFSET,
                                   x2 - x1 - 2*OFFSET, tab->rect.height() - 2*OFFSET);
                drawPrimitive(PE_FrameFocusRect, &fropt, p);
            }
        }
        break;
    case CE_SizeGrip: {
        p->save();
        int x, y, w, h;
        opt->rect.getRect(&x, &y, &w, &h);

        int sw = qMin(h, w);
        if (h > w)
            p->translate(0, h - w);
        else
            p->translate(w - h, 0);

        int sx = x;
        int sy = y;
        int s = sw / 3;

        Qt::Corner corner;
        if (const QStyleOptionSizeGrip *sgOpt = qstyleoption_cast<const QStyleOptionSizeGrip *>(opt))
            corner = sgOpt->corner;
        else if (opt->direction == Qt::RightToLeft)
            corner = Qt::BottomLeftCorner;
        else
            corner = Qt::BottomRightCorner;

        if (corner == Qt::BottomLeftCorner) {
            sx = x + sw;
            for (int i = 0; i < 4; ++i) {
                p->setPen(QPen(opt->palette.light().color(), 1));
                p->drawLine(x, sy - 1 , sx + 1, sw);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(x, sy, sx, sw);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(x, sy + 1, sx - 1, sw);
                sx -= s;
                sy += s;
            }
        } else if (corner == Qt::BottomRightCorner) {
            for (int i = 0; i < 4; ++i) {
                p->setPen(QPen(opt->palette.light().color(), 1));
                p->drawLine(sx - 1, sw, sw, sy - 1);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(sx, sw, sw, sy);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(sx + 1, sw, sw, sy + 1);
                sx += s;
                sy += s;
            }
        } else if (corner == Qt::TopRightCorner) {
            sy = y + sw;
            for (int i = 0; i < 4; ++i) {
                p->setPen(QPen(opt->palette.light().color(), 1));
                p->drawLine(sx - 1, y, sw, sy + 1);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(sx, y, sw, sy);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(sx + 1, y, sw, sy - 1);
                sx += s;
                sy -= s;
            }
        } else if (corner == Qt::TopLeftCorner) {
            for (int i = 0; i < 4; ++i) {
                p->setPen(QPen(opt->palette.light().color(), 1));
                p->drawLine(x, sy - 1, sx - 1, y);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(x, sy, sx, y);
                p->setPen(QPen(opt->palette.dark().color(), 1));
                p->drawLine(x, sy + 1, sx + 1, y);
                sx += s;
                sy += s;
            }
        }
        p->restore();
        break; }
    case CE_RubberBand: {
        if (const QStyleOptionRubberBand *rbOpt = qstyleoption_cast<const QStyleOptionRubberBand *>(opt)) {
            QPixmap tiledPixmap(16, 16);
            QPainter pixmapPainter(&tiledPixmap);
            pixmapPainter.setPen(Qt::NoPen);
            pixmapPainter.setBrush(Qt::Dense4Pattern);
            pixmapPainter.setBackground(QBrush(opt->palette.base()));
            pixmapPainter.setBackgroundMode(Qt::OpaqueMode);
            pixmapPainter.drawRect(0, 0, tiledPixmap.width(), tiledPixmap.height());
            pixmapPainter.end();
            // ### workaround for borked XRENDER
            tiledPixmap = QPixmap::fromImage(tiledPixmap.toImage());

            p->save();
            QRect r = opt->rect;
            QStyleHintReturnMask mask;
            if (proxy()->styleHint(QStyle::SH_RubberBand_Mask, opt, &mask))
                p->setClipRegion(mask.region);
            p->drawTiledPixmap(r.x(), r.y(), r.width(), r.height(), tiledPixmap);
            p->setPen(opt->palette.color(QPalette::Active, QPalette::WindowText));
            p->setBrush(Qt::NoBrush);
            p->drawRect(r.adjusted(0, 0, -1, -1));
            if (rbOpt->shape == QStyleOptionRubberBand::Rectangle)
                p->drawRect(r.adjusted(3, 3, -4, -4));
            p->restore();
        }
        break; }
    case CE_DockWidgetTitle:
        if (const QStyleOptionDockWidget *dwOpt = qstyleoption_cast<const QStyleOptionDockWidget *>(opt)) {
            QRect r = dwOpt->rect.adjusted(0, 0, -1, -1);
            if (dwOpt->movable) {
                p->setPen(dwOpt->palette.color(QPalette::Dark));
                p->drawRect(r);
            }

            if (!dwOpt->title.isEmpty()) {
                const bool verticalTitleBar = dwOpt->verticalTitleBar;

                if (verticalTitleBar) {
                    r = r.transposed();

                    p->save();
                    p->translate(r.left(), r.top() + r.width());
                    p->rotate(-90);
                    p->translate(-r.left(), -r.top());
                }

                const int indent = p->fontMetrics().descent();
                proxy()->drawItemText(p, r.adjusted(indent + 1, 1, -indent - 1, -1),
                              Qt::AlignLeft | Qt::AlignVCenter, dwOpt->palette,
                              dwOpt->state & State_Enabled, dwOpt->title,
                              QPalette::WindowText);

                if (verticalTitleBar)
                    p->restore();
            }
        }
        break;
    case CE_Header:
        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            QRegion clipRegion = p->clipRegion();
            p->setClipRect(opt->rect);
            proxy()->drawControl(CE_HeaderSection, header, p);
            QStyleOptionHeader subopt = *header;
            subopt.rect = subElementRect(SE_HeaderLabel, header);
            if (subopt.rect.isValid())
                proxy()->drawControl(CE_HeaderLabel, &subopt, p);
            if (header->sortIndicator != QStyleOptionHeader::None) {
                subopt.rect = subElementRect(SE_HeaderArrow, opt);
                proxy()->drawPrimitive(PE_IndicatorHeaderArrow, &subopt, p);
            }
            p->setClipRegion(clipRegion);
        }
        break;
    case CE_FocusFrame:
            p->fillRect(opt->rect, opt->palette.windowText());
        break;
    case CE_HeaderSection:
            qDrawShadePanel(p, opt->rect, opt->palette,
                        opt->state & State_Sunken, 1,
                        &opt->palette.brush(QPalette::Button));
        break;
    case CE_HeaderEmptyArea:
            p->fillRect(opt->rect, opt->palette.window());
        break;
    case CE_ComboBoxLabel:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            QRect editRect = proxy()->subControlRect(CC_ComboBox, cb, SC_ComboBoxEditField);
            p->save();
            p->setClipRect(editRect);
            if (!cb->currentIcon.isNull()) {
                QIcon::Mode mode = cb->state & State_Enabled ? QIcon::Normal
                                                             : QIcon::Disabled;
                QPixmap pixmap = cb->currentIcon.pixmap(opt->window, cb->iconSize, mode);
                QRect iconRect(editRect);
                iconRect.setWidth(cb->iconSize.width() + 4);
                iconRect = alignedRect(cb->direction,
                                       Qt::AlignLeft | Qt::AlignVCenter,
                                       iconRect.size(), editRect);
                if (cb->editable)
                    p->fillRect(iconRect, opt->palette.brush(QPalette::Base));
                proxy()->drawItemPixmap(p, iconRect, Qt::AlignCenter, pixmap);

                if (cb->direction == Qt::RightToLeft)
                    editRect.translate(-4 - cb->iconSize.width(), 0);
                else
                    editRect.translate(cb->iconSize.width() + 4, 0);
            }
            if (!cb->currentText.isEmpty() && !cb->editable) {
                proxy()->drawItemText(p, editRect.adjusted(1, 0, -1, 0),
                             visualAlignment(cb->direction, Qt::AlignLeft | Qt::AlignVCenter),
                             cb->palette, cb->state & State_Enabled, cb->currentText);
            }
            p->restore();
        }
        break;
    case CE_ToolBar:
        if (const QStyleOptionToolBar *toolBar = qstyleoption_cast<const QStyleOptionToolBar *>(opt)) {
            // Compatibility with styles that use PE_PanelToolBar
            QStyleOptionFrame frame;
            frame.QStyleOption::operator=(*toolBar);
            frame.lineWidth = toolBar->lineWidth;
            frame.midLineWidth = toolBar->midLineWidth;
            proxy()->drawPrimitive(PE_PanelToolBar, opt, p);

            qDrawShadePanel(p, toolBar->rect, toolBar->palette, false, toolBar->lineWidth,
                            &toolBar->palette.brush(QPalette::Button));
        }
        break;
    case CE_ColumnViewGrip: {
        // draw background gradients
        QLinearGradient g(0, 0, opt->rect.width(), 0);
        g.setColorAt(0, opt->palette.color(QPalette::Active, QPalette::Mid));
        g.setColorAt(0.5, Qt::white);
        p->fillRect(QRect(0, 0, opt->rect.width(), opt->rect.height()), g);

        // draw the two lines
        QPen pen(p->pen());
        pen.setWidth(opt->rect.width()/20);
        pen.setColor(opt->palette.color(QPalette::Active, QPalette::Dark));
        p->setPen(pen);

        int line1starting = opt->rect.width()*8 / 20;
        int line2starting = opt->rect.width()*13 / 20;
        int top = opt->rect.height()*20/75;
        int bottom = opt->rect.height() - 1 - top;
        p->drawLine(line1starting, top, line1starting, bottom);
        p->drawLine(line2starting, top, line2starting, bottom);
        }
        break;
    case CE_ItemViewItem:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            p->save();
            p->setClipRect(opt->rect);

            QRect checkRect = proxy()->subElementRect(SE_ItemViewItemCheckIndicator, vopt);
            QRect iconRect = proxy()->subElementRect(SE_ItemViewItemDecoration, vopt);
            QRect textRect = proxy()->subElementRect(SE_ItemViewItemText, vopt);

            // draw the background
            proxy()->drawPrimitive(PE_PanelItemViewItem, opt, p);

            // draw the check mark
            if (vopt->features & QStyleOptionViewItem::HasCheckIndicator) {
                QStyleOptionViewItem option(*vopt);
                option.rect = checkRect;
                option.state = option.state & ~QStyle::State_HasFocus;

                switch (vopt->checkState) {
                case Qt::Unchecked:
                    option.state |= QStyle::State_Off;
                    break;
                case Qt::PartiallyChecked:
                    option.state |= QStyle::State_NoChange;
                    break;
                case Qt::Checked:
                    option.state |= QStyle::State_On;
                    break;
                }
                proxy()->drawPrimitive(QStyle::PE_IndicatorItemViewItemCheck, &option, p);
            }

            // draw the icon
            QIcon::Mode mode = QIcon::Normal;
            if (!(vopt->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if (vopt->state & QStyle::State_Selected)
                mode = QIcon::Selected;
            QIcon::State state = vopt->state & QStyle::State_Open ? QIcon::On : QIcon::Off;
            vopt->icon.paint(p, iconRect, vopt->decorationAlignment, mode, state);

            // draw the text
            if (!vopt->text.isEmpty()) {
                QPalette::ColorGroup cg = vopt->state & QStyle::State_Enabled
                                      ? QPalette::Normal : QPalette::Disabled;
                if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
                    cg = QPalette::Inactive;

                if (vopt->state & QStyle::State_Selected) {
                    p->setPen(vopt->palette.color(cg, QPalette::HighlightedText));
                } else {
                    p->setPen(vopt->palette.color(cg, QPalette::Text));
                }
                if (vopt->state & QStyle::State_Editing) {
                    p->setPen(vopt->palette.color(cg, QPalette::Text));
                    p->drawRect(textRect.adjusted(0, 0, -1, -1));
                }

                d->viewItemDrawText(p, vopt, textRect);
            }

            // draw the focus rect
             if (vopt->state & QStyle::State_HasFocus) {
                QStyleOptionFocusRect o;
                o.QStyleOption::operator=(*vopt);
                o.rect = proxy()->subElementRect(SE_ItemViewItemFocusRect, vopt);
                o.state |= QStyle::State_KeyboardFocusChange;
                o.state |= QStyle::State_Item;
                QPalette::ColorGroup cg = (vopt->state & QStyle::State_Enabled)
                              ? QPalette::Normal : QPalette::Disabled;
                o.backgroundColor = vopt->palette.color(cg, (vopt->state & QStyle::State_Selected)
                                             ? QPalette::Highlight : QPalette::Window);
                proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &o, p);
            }

             p->restore();
        }
        break;
    case CE_ShapedFrame:
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            int frameShape  = f->frameShape;
            int frameShadow = QStyleOptionFrame::Plain;
            if (f->state & QStyle::State_Sunken) {
                frameShadow = QStyleOptionFrame::Sunken;
            } else if (f->state & QStyle::State_Raised) {
                frameShadow = QStyleOptionFrame::Raised;
            }

            int lw = f->lineWidth;
            int mlw = f->midLineWidth;
            QPalette::ColorRole foregroundRole = QPalette::WindowText;

            switch (frameShape) {
            case QStyleOptionFrame::Box:
                if (frameShadow == QStyleOptionFrame::Plain) {
                    qDrawPlainRect(p, f->rect, f->palette.color(foregroundRole), lw);
                } else {
                    qDrawShadeRect(p, f->rect, f->palette, frameShadow == QStyleOptionFrame::Sunken, lw, mlw);
                }
                break;
            case QStyleOptionFrame::StyledPanel:
                proxy()->drawPrimitive(QStyle::PE_Frame, opt, p);
                break;
            case QStyleOptionFrame::Panel:
                if (frameShadow == QStyleOptionFrame::Plain) {
                    qDrawPlainRect(p, f->rect, f->palette.color(foregroundRole), lw);
                } else {
                    qDrawShadePanel(p, f->rect, f->palette, frameShadow == QStyleOptionFrame::Sunken, lw);
                }
                break;
            case QStyleOptionFrame::WinPanel:
                if (frameShadow == QStyleOptionFrame::Plain) {
                    qDrawPlainRect(p, f->rect, f->palette.color(foregroundRole), lw);
                } else {
                    qDrawWinPanel(p, f->rect, f->palette, frameShadow == QStyleOptionFrame::Sunken);
                }
                break;
            case QStyleOptionFrame::HLine:
            case QStyleOptionFrame::VLine: {
                QPoint p1, p2;
                if (frameShape == QStyleOptionFrame::HLine) {
                    p1 = QPoint(opt->rect.x(), opt->rect.y() + opt->rect.height() / 2);
                    p2 = QPoint(opt->rect.x() + opt->rect.width(), p1.y());
                } else {
                    p1 = QPoint(opt->rect.x() + opt->rect.width() / 2, opt->rect.y());
                    p2 = QPoint(p1.x(), p1.y() + opt->rect.height());
                }
                if (frameShadow == QStyleOptionFrame::Plain) {
                    QPen oldPen = p->pen();
                    p->setPen(QPen(opt->palette.brush(foregroundRole), lw));
                    p->drawLine(p1, p2);
                    p->setPen(oldPen);
                } else {
                    qDrawShadeLine(p, p1, p2, f->palette, frameShadow == QStyleOptionFrame::Sunken, lw, mlw);
                }
                break;
                }
            }
        }
        break;
    default:
        break;
    }
}

QRect QCommonStyle::subElementRect(SubElement sr, const QStyleOption *opt) const
{
    Q_D(const QCommonStyle);
    QRect r;
    switch (sr) {
    case SE_PushButtonContents:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            int dx1, dx2;
            dx1 = proxy()->pixelMetric(PM_DefaultFrameWidth, btn);
            if (btn->features & QStyleOptionButton::AutoDefaultButton)
                dx1 += proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn);
            dx2 = dx1 * 2;
            r.setRect(opt->rect.x() + dx1, opt->rect.y() + dx1, opt->rect.width() - dx2,
                      opt->rect.height() - dx2);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_PushButtonFocusRect:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            int dbw1 = 0, dbw2 = 0;
            if (btn->features & QStyleOptionButton::AutoDefaultButton){
                dbw1 = proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn);
                dbw2 = dbw1 * 2;
            }

            int dfw1 = proxy()->pixelMetric(PM_DefaultFrameWidth, btn) + 1,
                dfw2 = dfw1 * 2;

            r.setRect(btn->rect.x() + dfw1 + dbw1, btn->rect.y() + dfw1 + dbw1,
                      btn->rect.width() - dfw2 - dbw2, btn->rect.height()- dfw2 - dbw2);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_CheckBoxIndicator:
        {
            int h = proxy()->pixelMetric(PM_IndicatorHeight, opt);
            r.setRect(opt->rect.x(), opt->rect.y() + ((opt->rect.height() - h) / 2),
                      proxy()->pixelMetric(PM_IndicatorWidth, opt), h);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;

    case SE_CheckBoxContents:
        {
            // Deal with the logical first, then convert it back to screen coords.
            QRect ir = visualRect(opt->direction, opt->rect,
                                  subElementRect(SE_CheckBoxIndicator, opt));
            int spacing = proxy()->pixelMetric(PM_CheckBoxLabelSpacing, opt);
            r.setRect(ir.right() + spacing, opt->rect.y(), opt->rect.width() - ir.width() - spacing,
                      opt->rect.height());
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;

    case SE_CheckBoxFocusRect:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            if (btn->icon.isNull() && btn->text.isEmpty()) {
                r = subElementRect(SE_CheckBoxIndicator, opt);
                r.adjust(1, 1, -1, -1);
                break;
            }
            // As above, deal with the logical first, then convert it back to screen coords.
            QRect cr = visualRect(btn->direction, btn->rect, subElementRect(SE_CheckBoxContents, btn));

            QRect iconRect, textRect;
            if (!btn->text.isEmpty()) {
                textRect = itemTextRect(opt->fontMetrics, cr, Qt::AlignAbsolute | Qt::AlignLeft
                                        | Qt::AlignVCenter | Qt::TextShowMnemonic,
                                        btn->state & State_Enabled, btn->text);
            }
            if (!btn->icon.isNull()) {
                iconRect = itemPixmapRect(cr, Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter
                                        | Qt::TextShowMnemonic,
                                   btn->icon.pixmap(opt->window, btn->iconSize, QIcon::Normal));
                if (!textRect.isEmpty())
                    textRect.translate(iconRect.right() + 4, 0);
            }
            r = iconRect | textRect;
            r.adjust(-3, -2, 3, 2);
            r = r.intersected(btn->rect);
            r = visualRect(btn->direction, btn->rect, r);
        }
        break;

    case SE_RadioButtonIndicator:
        {
            int h = proxy()->pixelMetric(PM_ExclusiveIndicatorHeight, opt);
            r.setRect(opt->rect.x(), opt->rect.y() + ((opt->rect.height() - h) / 2),
                    proxy()->pixelMetric(PM_ExclusiveIndicatorWidth, opt), h);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;

    case SE_RadioButtonContents:
        {
            QRect ir = visualRect(opt->direction, opt->rect,
                                  subElementRect(SE_RadioButtonIndicator, opt));
            int spacing = proxy()->pixelMetric(PM_RadioButtonLabelSpacing, opt);
            r.setRect(ir.left() + ir.width() + spacing, opt->rect.y(), opt->rect.width() - ir.width() - spacing,
                      opt->rect.height());
            r = visualRect(opt->direction, opt->rect, r);
            break;
        }

    case SE_RadioButtonFocusRect:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            if (btn->icon.isNull() && btn->text.isEmpty()) {
                r = subElementRect(SE_RadioButtonIndicator, opt);
                r.adjust(1, 1, -1, -1);
                break;
            }
            QRect cr = visualRect(btn->direction, btn->rect, subElementRect(SE_RadioButtonContents, opt));

            QRect iconRect, textRect;
            if (!btn->text.isEmpty()){
                textRect = itemTextRect(opt->fontMetrics, cr, Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter
                                 | Qt::TextShowMnemonic, btn->state & State_Enabled, btn->text);
            }
            if (!btn->icon.isNull()) {
                iconRect = itemPixmapRect(cr, Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic,
                                   btn->icon.pixmap(opt->window, btn->iconSize, QIcon::Normal));
                if (!textRect.isEmpty())
                    textRect.translate(iconRect.right() + 4, 0);
            }
            r = iconRect | textRect;
            r.adjust(-3, -2, 3, 2);
            r = r.intersected(btn->rect);
            r = visualRect(btn->direction, btn->rect, r);
        }
        break;
    case SE_SliderFocusRect:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            int tickOffset = proxy()->pixelMetric(PM_SliderTickmarkOffset, slider);
            int thickness  = proxy()->pixelMetric(PM_SliderControlThickness, slider);
            if (slider->orientation == Qt::Horizontal)
                r.setRect(0, tickOffset - 1, slider->rect.width(), thickness + 2);
            else
                r.setRect(tickOffset - 1, 0, thickness + 2, slider->rect.height());
            r = r.intersected(slider->rect);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_ProgressBarGroove:
    case SE_ProgressBarContents:
    case SE_ProgressBarLabel:
        if (const QStyleOptionProgressBar *pb = qstyleoption_cast<const QStyleOptionProgressBar *>(opt)) {
            int textw = 0;
            if (pb->textVisible)
                textw = qMax(pb->fontMetrics.horizontalAdvance(pb->text), pb->fontMetrics.horizontalAdvance(QLatin1String("100%"))) + 6;

            if ((pb->textAlignment & Qt::AlignCenter) == 0) {
                if (sr != SE_ProgressBarLabel)
                    r.setCoords(pb->rect.left(), pb->rect.top(),
                                pb->rect.right() - textw, pb->rect.bottom());
                else
                    r.setCoords(pb->rect.right() - textw, pb->rect.top(),
                                pb->rect.right(), pb->rect.bottom());
            } else {
                r = pb->rect;
            }
            r = visualRect(pb->direction, pb->rect, r);
        }
        break;
    case SE_ComboBoxFocusRect:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            int margin = cb->frame ? 3 : 0;
            r.setRect(opt->rect.left() + margin, opt->rect.top() + margin,
                      opt->rect.width() - 2*margin - 16, opt->rect.height() - 2*margin);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_ToolBoxTabContents:
        r = opt->rect;
        r.adjust(0, 0, -30, 0);
        break;
    case SE_HeaderLabel: {
        int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, opt);
        r.setRect(opt->rect.x() + margin, opt->rect.y() + margin,
                  opt->rect.width() - margin * 2, opt->rect.height() - margin * 2);

        if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            // Subtract width needed for arrow, if there is one
            if (header->sortIndicator != QStyleOptionHeader::None) {
                if (opt->state & State_Horizontal)
                    r.setWidth(r.width() - (opt->rect.height() / 2) - (margin * 2));
                else
                    r.setHeight(r.height() - (opt->rect.width() / 2) - (margin * 2));
            }
        }
        r = visualRect(opt->direction, opt->rect, r);
        break; }
    case SE_HeaderArrow: {
        int h = opt->rect.height();
        int w = opt->rect.width();
        int x = opt->rect.x();
        int y = opt->rect.y();
        int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, opt);

        if (opt->state & State_Horizontal) {
            int horiz_size = h / 2;
            r.setRect(x + w - margin * 2 - horiz_size, y + 5,
                      horiz_size, h - margin * 2 - 5);
        } else {
            int vert_size = w / 2;
            r.setRect(x + 5, y + h - margin * 2 - vert_size,
                      w - margin * 2 - 5, vert_size);
        }
        r = visualRect(opt->direction, opt->rect, r);
        break; }

    case SE_RadioButtonClickRect:
        r = subElementRect(SE_RadioButtonFocusRect, opt);
        r |= subElementRect(SE_RadioButtonIndicator, opt);
        break;
    case SE_CheckBoxClickRect:
        r = subElementRect(SE_CheckBoxFocusRect, opt);
        r |= subElementRect(SE_CheckBoxIndicator, opt);
        break;
    case SE_TabWidgetTabBar:
        if (const QStyleOptionTabWidgetFrame *twf
                = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt)) {
            r.setSize(twf->tabBarSize);
            const uint alingMask = Qt::AlignLeft | Qt::AlignRight | Qt::AlignHCenter;
            switch (twf->shape) {
            case QStyleOptionTab::RoundedNorth:
            case QStyleOptionTab::TriangularNorth:
                // Constrain the size now, otherwise, center could get off the page
                // This of course repeated for all the other directions
                r.setWidth(qMin(r.width(), twf->rect.width()
                                            - twf->leftCornerWidgetSize.width()
                                            - twf->rightCornerWidgetSize.width()));
                switch (proxy()->styleHint(SH_TabBar_Alignment, twf) & alingMask) {
                default:
                case Qt::AlignLeft:
                    r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(), 0));
                    break;
                case Qt::AlignHCenter:
                    r.moveTopLeft(QPoint(twf->rect.center().x() - qRound(r.width() / 2.0f)
                                         + (twf->leftCornerWidgetSize.width() / 2)
                                         - (twf->rightCornerWidgetSize.width() / 2), 0));
                    break;
                case Qt::AlignRight:
                    r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width()
                                         - twf->rightCornerWidgetSize.width(), 0));
                    break;
                }
                r = visualRect(twf->direction, twf->rect, r);
                break;
            case QStyleOptionTab::RoundedSouth:
            case QStyleOptionTab::TriangularSouth:
                r.setWidth(qMin(r.width(), twf->rect.width()
                                            - twf->leftCornerWidgetSize.width()
                                            - twf->rightCornerWidgetSize.width()));
                switch (proxy()->styleHint(SH_TabBar_Alignment, twf) & alingMask) {
                default:
                case Qt::AlignLeft:
                    r.moveTopLeft(QPoint(twf->leftCornerWidgetSize.width(),
                                         twf->rect.height() - twf->tabBarSize.height()));
                    break;
                case Qt::AlignHCenter:
                    r.moveTopLeft(QPoint(twf->rect.center().x() - qRound(r.width() / 2.0f)
                                         + (twf->leftCornerWidgetSize.width() / 2)
                                         - (twf->rightCornerWidgetSize.width() / 2),
                                         twf->rect.height() - twf->tabBarSize.height()));
                    break;
                case Qt::AlignRight:
                    r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width()
                                         - twf->rightCornerWidgetSize.width(),
                                         twf->rect.height() - twf->tabBarSize.height()));
                    break;
                }
                r = visualRect(twf->direction, twf->rect, r);
                break;
            case QStyleOptionTab::RoundedEast:
            case QStyleOptionTab::TriangularEast:
                r.setHeight(qMin(r.height(), twf->rect.height()
                                            - twf->leftCornerWidgetSize.height()
                                            - twf->rightCornerWidgetSize.height()));
                switch (proxy()->styleHint(SH_TabBar_Alignment, twf) & alingMask) {
                default:
                case Qt::AlignLeft:
                    r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width(),
                                         twf->leftCornerWidgetSize.height()));
                    break;
                case Qt::AlignHCenter:
                    r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width(),
                                         twf->rect.center().y() - r.height() / 2));
                    break;
                case Qt::AlignRight:
                    r.moveTopLeft(QPoint(twf->rect.width() - twf->tabBarSize.width(),
                                         twf->rect.height() - twf->tabBarSize.height()
                                         - twf->rightCornerWidgetSize.height()));
                    break;
                }
                break;
            case QStyleOptionTab::RoundedWest:
            case QStyleOptionTab::TriangularWest:
                r.setHeight(qMin(r.height(), twf->rect.height()
                                             - twf->leftCornerWidgetSize.height()
                                             - twf->rightCornerWidgetSize.height()));
                switch (proxy()->styleHint(SH_TabBar_Alignment, twf) & alingMask) {
                default:
                case Qt::AlignLeft:
                    r.moveTopLeft(QPoint(0, twf->leftCornerWidgetSize.height()));
                    break;
                case Qt::AlignHCenter:
                    r.moveTopLeft(QPoint(0, twf->rect.center().y() - r.height() / 2));
                    break;
                case Qt::AlignRight:
                    r.moveTopLeft(QPoint(0, twf->rect.height() - twf->tabBarSize.height()
                                         - twf->rightCornerWidgetSize.height()));
                    break;
                }
                break;
            }
        }
        break;
    case SE_TabWidgetTabPane:
    case SE_TabWidgetTabContents:
        if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt)) {
            QStyleOptionTab tabopt;
            tabopt.shape = twf->shape;
            int overlap = proxy()->pixelMetric(PM_TabBarBaseOverlap, &tabopt);
            if (twf->lineWidth == 0)
                overlap = 0;
            switch (twf->shape) {
            case QStyleOptionTab::RoundedNorth:
            case QStyleOptionTab::TriangularNorth:
                r = QRect(QPoint(0,qMax(twf->tabBarSize.height() - overlap, 0)),
                          QSize(twf->rect.width(), qMin(twf->rect.height() - twf->tabBarSize.height() + overlap, twf->rect.height())));
                break;
            case QStyleOptionTab::RoundedSouth:
            case QStyleOptionTab::TriangularSouth:
                r = QRect(QPoint(0,0), QSize(twf->rect.width(), qMin(twf->rect.height() - twf->tabBarSize.height() + overlap, twf->rect.height())));
                break;
            case QStyleOptionTab::RoundedEast:
            case QStyleOptionTab::TriangularEast:
                r = QRect(QPoint(0, 0), QSize(qMin(twf->rect.width() - twf->tabBarSize.width() + overlap, twf->rect.width()), twf->rect.height()));
                break;
            case QStyleOptionTab::RoundedWest:
            case QStyleOptionTab::TriangularWest:
                r = QRect(QPoint(qMax(twf->tabBarSize.width() - overlap, 0), 0),
                          QSize(qMin(twf->rect.width() - twf->tabBarSize.width() + overlap, twf->rect.width()), twf->rect.height()));
                break;
            }
            if (sr == SE_TabWidgetTabContents && twf->lineWidth > 0)
               r.adjust(2, 2, -2, -2);
        }
        break;
    case SE_TabWidgetLeftCorner:
        if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt)) {
            QRect paneRect = subElementRect(SE_TabWidgetTabPane, twf);
            switch (twf->shape) {
            case QStyleOptionTab::RoundedNorth:
            case QStyleOptionTab::TriangularNorth:
                r = QRect(QPoint(paneRect.x(), paneRect.y() - twf->leftCornerWidgetSize.height()),
                          twf->leftCornerWidgetSize);
                break;
            case QStyleOptionTab::RoundedSouth:
            case QStyleOptionTab::TriangularSouth:
                r = QRect(QPoint(paneRect.x(), paneRect.height()), twf->leftCornerWidgetSize);
               break;
            default:
               break;
            }
           r = visualRect(twf->direction, twf->rect, r);
        }
        break;
   case SE_TabWidgetRightCorner:
       if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(opt)) {
           QRect paneRect = subElementRect(SE_TabWidgetTabPane, twf);
           switch (twf->shape) {
           case QStyleOptionTab::RoundedNorth:
           case QStyleOptionTab::TriangularNorth:
                r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(),
                                 paneRect.y() - twf->rightCornerWidgetSize.height()),
                          twf->rightCornerWidgetSize);
               break;
           case QStyleOptionTab::RoundedSouth:
           case QStyleOptionTab::TriangularSouth:
                r = QRect(QPoint(paneRect.width() - twf->rightCornerWidgetSize.width(),
                                 paneRect.height()), twf->rightCornerWidgetSize);
               break;
           default:
               break;
           }
           r = visualRect(twf->direction, twf->rect, r);
        }
        break;
    case SE_TabBarTabText:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            QRect dummyIconRect;
            d->tabLayout(tab, &r, &dummyIconRect);
        }
        break;
    case SE_TabBarTabLeftButton:
    case SE_TabBarTabRightButton:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            bool selected = tab->state & State_Selected;
            int verticalShift = proxy()->pixelMetric(QStyle::PM_TabBarTabShiftVertical, tab);
            int horizontalShift = proxy()->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, tab);
            int hpadding = proxy()->pixelMetric(QStyle::PM_TabBarTabHSpace, opt) / 2;
            hpadding = qMax(hpadding, 4); //workaround KStyle returning 0 because they workaround an old bug in Qt

            bool verticalTabs = tab->shape == QStyleOptionTab::RoundedEast
                    || tab->shape == QStyleOptionTab::RoundedWest
                    || tab->shape == QStyleOptionTab::TriangularEast
                    || tab->shape == QStyleOptionTab::TriangularWest;

            QRect tr = tab->rect;
            if (tab->shape == QStyleOptionTab::RoundedSouth || tab->shape == QStyleOptionTab::TriangularSouth)
                verticalShift = -verticalShift;
            if (verticalTabs) {
                qSwap(horizontalShift, verticalShift);
                horizontalShift *= -1;
                verticalShift *= -1;
            }
            if (tab->shape == QStyleOptionTab::RoundedWest || tab->shape == QStyleOptionTab::TriangularWest)
                horizontalShift = -horizontalShift;

            tr.adjust(0, 0, horizontalShift, verticalShift);
            if (selected)
            {
                tr.setBottom(tr.bottom() - verticalShift);
                tr.setRight(tr.right() - horizontalShift);
            }

            QSize size = (sr == SE_TabBarTabLeftButton) ? tab->leftButtonSize : tab->rightButtonSize;
            int w = size.width();
            int h = size.height();
            int midHeight = static_cast<int>(qCeil(float(tr.height() - h) / 2));
            int midWidth = ((tr.width() - w) / 2);

            bool atTheTop = true;
            switch (tab->shape) {
            case QStyleOptionTab::RoundedWest:
            case QStyleOptionTab::TriangularWest:
                atTheTop = (sr == SE_TabBarTabLeftButton);
                break;
            case QStyleOptionTab::RoundedEast:
            case QStyleOptionTab::TriangularEast:
                atTheTop = (sr == SE_TabBarTabRightButton);
                break;
            default:
                if (sr == SE_TabBarTabLeftButton)
                    r = QRect(tab->rect.x() + hpadding, midHeight, w, h);
                else
                    r = QRect(tab->rect.right() - w - hpadding, midHeight, w, h);
                r = visualRect(tab->direction, tab->rect, r);
            }
            if (verticalTabs) {
                if (atTheTop)
                    r = QRect(midWidth, tr.y() + tab->rect.height() - hpadding - h, w, h);
                else
                    r = QRect(midWidth, tr.y() + hpadding, w, h);
            }
        }

        break;
    case SE_TabBarTearIndicator:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            switch (tab->shape) {
            case QStyleOptionTab::RoundedNorth:
            case QStyleOptionTab::TriangularNorth:
            case QStyleOptionTab::RoundedSouth:
            case QStyleOptionTab::TriangularSouth:
                r.setRect(tab->rect.left(), tab->rect.top(), 8, opt->rect.height());
                break;
            case QStyleOptionTab::RoundedWest:
            case QStyleOptionTab::TriangularWest:
            case QStyleOptionTab::RoundedEast:
            case QStyleOptionTab::TriangularEast:
                r.setRect(tab->rect.left(), tab->rect.top(), opt->rect.width(), 8);
                break;
            default:
                break;
            }
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_TabBarScrollLeftButton: {
        const bool vertical = opt->rect.width() < opt->rect.height();
        const Qt::LayoutDirection ld = opt->direction;
        const int buttonWidth = proxy()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, nullptr);
        const int buttonOverlap = proxy()->pixelMetric(QStyle::PM_TabBar_ScrollButtonOverlap, nullptr);

        r = vertical ? QRect(0, opt->rect.height() - (buttonWidth * 2) + buttonOverlap, opt->rect.width(), buttonWidth)
            : QStyle::visualRect(ld, opt->rect, QRect(opt->rect.width() - (buttonWidth * 2) + buttonOverlap, 0, buttonWidth, opt->rect.height()));
        break; }
    case SE_TabBarScrollRightButton: {
        const bool vertical = opt->rect.width() < opt->rect.height();
        const Qt::LayoutDirection ld = opt->direction;
        const int buttonWidth = proxy()->pixelMetric(QStyle::PM_TabBarScrollButtonWidth, nullptr);

        r = vertical ? QRect(0, opt->rect.height() - buttonWidth, opt->rect.width(), buttonWidth)
            : QStyle::visualRect(ld, opt->rect, QRect(opt->rect.width() - buttonWidth, 0, buttonWidth, opt->rect.height()));
        break; }
    case SE_TreeViewDisclosureItem:
        r = opt->rect;
        break;
    case SE_LineEditContents:
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            r = f->rect.adjusted(f->lineWidth, f->lineWidth, -f->lineWidth, -f->lineWidth);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_FrameContents:
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, f);
            r = opt->rect.adjusted(fw, fw, -fw, -fw);
            r = visualRect(opt->direction, opt->rect, r);
        }
        break;
    case SE_ShapedFrameContents:
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            int frameShape  = f->frameShape;
            int frameShadow = QStyleOptionFrame::Plain;
            if (f->state & QStyle::State_Sunken) {
                frameShadow = QStyleOptionFrame::Sunken;
            } else if (f->state & QStyle::State_Raised) {
                frameShadow = QStyleOptionFrame::Raised;
            }

            int frameWidth = 0;

            switch (frameShape) {
            case QStyleOptionFrame::NoFrame:
                frameWidth = 0;
                break;

            case QStyleOptionFrame::Box:
            case QStyleOptionFrame::HLine:
            case QStyleOptionFrame::VLine:
                switch (frameShadow) {
                case QStyleOptionFrame::Plain:
                    frameWidth = f->lineWidth;
                    break;
                case QStyleOptionFrame::Raised:
                case QStyleOptionFrame::Sunken:
                    frameWidth = (short)(f->lineWidth*2 + f->midLineWidth);
                    break;
                }
                break;

            case QStyleOptionFrame::StyledPanel:
                //keep the compatibility with Qt 4.4 if there is a proxy style.
                //be sure to call drawPrimitive(QStyle::SE_FrameContents) on the proxy style
                return subElementRect(QStyle::SE_FrameContents, opt);

            case QStyleOptionFrame::WinPanel:
                frameWidth = 2;
                break;

            case QStyleOptionFrame::Panel:
                switch (frameShadow) {
                case QStyleOptionFrame::Plain:
                case QStyleOptionFrame::Raised:
                case QStyleOptionFrame::Sunken:
                    frameWidth = f->lineWidth;
                    break;
                }
                break;
            }
            r = f->rect.adjusted(frameWidth, frameWidth, -frameWidth, -frameWidth);
        }
        break;
    case SE_DockWidgetCloseButton:
    case SE_DockWidgetFloatButton:
    case SE_DockWidgetTitleBarText:
    case SE_DockWidgetIcon: {
        int iconSize = proxy()->pixelMetric(PM_SmallIconSize, opt);
        int buttonMargin = proxy()->pixelMetric(PM_DockWidgetTitleBarButtonMargin, opt);
        QRect rect = opt->rect;

        const QStyleOptionDockWidget *dwOpt
            = qstyleoption_cast<const QStyleOptionDockWidget*>(opt);
        bool canClose = dwOpt == nullptr ? true : dwOpt->closable;
        bool canFloat = dwOpt == nullptr ? false : dwOpt->floatable;

        const bool verticalTitleBar = dwOpt && dwOpt->verticalTitleBar;

        // If this is a vertical titlebar, we transpose and work as if it was
        // horizontal, then transpose again.

        if (verticalTitleBar)
            rect = rect.transposed();

        do {
            int right = rect.right();
            int left = rect.left();

            QRect closeRect;
            if (canClose) {
                QSize sz = proxy()->standardIcon(QStyle::SP_TitleBarCloseButton,
                                        opt).actualSize(QSize(iconSize, iconSize));
                sz += QSize(buttonMargin, buttonMargin);
                if (verticalTitleBar)
                    sz = sz.transposed();
                closeRect = QRect(right - sz.width(),
                                    rect.center().y() - sz.height()/2,
                                    sz.width(), sz.height());
                right = closeRect.left() - 1;
            }
            if (sr == SE_DockWidgetCloseButton) {
                r = closeRect;
                break;
            }

            QRect floatRect;
            if (canFloat) {
                QSize sz = proxy()->standardIcon(QStyle::SP_TitleBarNormalButton,
                                        opt).actualSize(QSize(iconSize, iconSize));
                sz += QSize(buttonMargin, buttonMargin);
                if (verticalTitleBar)
                    sz = sz.transposed();
                floatRect = QRect(right - sz.width(),
                                    rect.center().y() - sz.height()/2,
                                    sz.width(), sz.height());
                right = floatRect.left() - 1;
            }
            if (sr == SE_DockWidgetFloatButton) {
                r = floatRect;
                break;
            }

            QRect iconRect;
            if (sr == SE_DockWidgetIcon) {
                r = iconRect;
                break;
            }

            QRect textRect = QRect(left, rect.top(),
                                    right - left, rect.height());
            if (sr == SE_DockWidgetTitleBarText) {
                r = textRect;
                break;
            }

        } while (false);

        if (verticalTitleBar) {
            r = QRect(rect.left() + r.top() - rect.top(),
                        rect.top() + rect.right() - r.right(),
                        r.height(), r.width());
        } else {
            r = visualRect(opt->direction, rect, r);
        }
        break;
    }
    case SE_ItemViewItemCheckIndicator:
        if (!qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            r = subElementRect(SE_CheckBoxIndicator, opt);
            break;
        }
        Q_FALLTHROUGH();
    case SE_ItemViewItemDecoration:
    case SE_ItemViewItemText:
    case SE_ItemViewItemFocusRect:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            if (!d->isViewItemCached(*vopt)) {
                d->viewItemLayout(vopt, &d->checkRect, &d->decorationRect, &d->displayRect, false);
                if (d->cachedOption) {
                    delete d->cachedOption;
                    d->cachedOption = nullptr;
                }
                d->cachedOption = new QStyleOptionViewItem(*vopt);
            }
            if (sr == SE_ItemViewItemCheckIndicator)
                r = d->checkRect;
            else if (sr == SE_ItemViewItemDecoration)
                r = d->decorationRect;
            else if (sr == SE_ItemViewItemText || sr == SE_ItemViewItemFocusRect)
                r = d->displayRect;
                               }
        break;
    case SE_ToolBarHandle:
        if (const QStyleOptionToolBar *tbopt = qstyleoption_cast<const QStyleOptionToolBar *>(opt)) {
            if (tbopt->features & QStyleOptionToolBar::Movable) {
                ///we need to access the widget here because the style option doesn't
                //have all the information we need (ie. the layout's margin)
                const QMargins margins(2, 2, 2, 2);
                const int handleExtent = proxy()->pixelMetric(QStyle::PM_ToolBarHandleExtent, opt);
                if (tbopt->state & QStyle::State_Horizontal) {
                    r = QRect(margins.left(), margins.top(),
                              handleExtent,
                              tbopt->rect.height() - (margins.top() + margins.bottom()));
                    r = QStyle::visualRect(tbopt->direction, tbopt->rect, r);
                } else {
                    r = QRect(margins.left(), margins.top(),
                              tbopt->rect.width() - (margins.left() + margins.right()),
                              handleExtent);
                }
            }
        }
        break;
    default:
        break;
    }
    return r;
}

// in lieu of std::array, minimal API
template <int N>
struct StaticPolygonF
{
    QPointF data[N];

    Q_DECL_CONSTEXPR int size() const { return N; }
    Q_DECL_CONSTEXPR const QPointF *cbegin() const { return data; }
    Q_DECL_CONSTEXPR const QPointF &operator[](int idx) const { return data[idx]; }
};

static StaticPolygonF<3> calcArrow(const QStyleOptionSlider *dial, qreal &a)
{
    int width = dial->rect.width();
    int height = dial->rect.height();
    int r = qMin(width, height) / 2;
    int currentSliderPosition = dial->upsideDown ? dial->sliderPosition : (dial->maximum - dial->sliderPosition);

    if (dial->maximum == dial->minimum)
        a = Q_PI / 2;
    else if (dial->dialWrapping)
        a = Q_PI * 3 / 2 - (currentSliderPosition - dial->minimum) * 2 * Q_PI
            / (dial->maximum - dial->minimum);
    else
        a = (Q_PI * 8 - (currentSliderPosition - dial->minimum) * 10 * Q_PI
            / (dial->maximum - dial->minimum)) / 6;

    int xc = width / 2;
    int yc = height / 2;

    int len = r - QStyleHelper::calcBigLineSize(r) - 5;
    if (len < 5)
        len = 5;
    int back = len / 2;

    StaticPolygonF<3> arrow = {{
        QPointF(0.5 + xc + len * qCos(a),
                0.5 + yc - len * qSin(a)),
        QPointF(0.5 + xc + back * qCos(a + Q_PI * 5 / 6),
                0.5 + yc - back * qSin(a + Q_PI * 5 / 6)),
        QPointF(0.5 + xc + back * qCos(a - Q_PI * 5 / 6),
                0.5 + yc - back * qSin(a - Q_PI * 5 / 6)),
    }};
    return arrow;
}

void QCommonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p) const
{
    switch (cc) {
    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            if (slider->subControls == SC_SliderTickmarks) {
                int tickOffset = proxy()->pixelMetric(PM_SliderTickmarkOffset, slider);
                int ticks = slider->tickPosition;
                int thickness = proxy()->pixelMetric(PM_SliderControlThickness, slider);
                int len = proxy()->pixelMetric(PM_SliderLength, slider);
                int available = proxy()->pixelMetric(PM_SliderSpaceAvailable, slider);
                int interval = slider->tickInterval;
                if (interval <= 0) {
                    interval = slider->singleStep;
                    if (QStyle::sliderPositionFromValue(slider->minimum, slider->maximum, interval,
                                                        available)
                        - QStyle::sliderPositionFromValue(slider->minimum, slider->maximum,
                                                          0, available) < 3)
                        interval = slider->pageStep;
                }
                if (!interval)
                    interval = 1;
                int fudge = len / 2;
                int pos;
                // Since there is no subrect for tickmarks do a translation here.
                p->save();
                p->translate(slider->rect.x(), slider->rect.y());
                p->setPen(slider->palette.windowText().color());
                int v = slider->minimum;
                while (v <= slider->maximum + 1) {
                    if (v == slider->maximum + 1 && interval == 1)
                        break;
                    const int v_ = qMin(v, slider->maximum);
                    pos = QStyle::sliderPositionFromValue(slider->minimum, slider->maximum,
                                                          v_, available) + fudge;
                    if (slider->orientation == Qt::Horizontal) {
                        if (ticks & QStyleOptionSlider::TicksAbove)
                            p->drawLine(pos, 0, pos, tickOffset - 2);
                        if (ticks & QStyleOptionSlider::TicksBelow)
                            p->drawLine(pos, tickOffset + thickness + 1, pos,
                                        slider->rect.height()-1);
                    } else {
                        if (ticks & QStyleOptionSlider::TicksAbove)
                            p->drawLine(0, pos, tickOffset - 2, pos);
                        if (ticks & QStyleOptionSlider::TicksBelow)
                            p->drawLine(tickOffset + thickness + 1, pos,
                                        slider->rect.width()-1, pos);
                    }
                    // in the case where maximum is max int
                    int nextInterval = v + interval;
                    if (nextInterval < v)
                        break;
                    v = nextInterval;
                }
                p->restore();
            }
        }
        break;
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            // Make a copy here and reset it for each primitive.
            QStyleOptionSlider newScrollbar = *scrollbar;
            State saveFlags = scrollbar->state;

            if (scrollbar->subControls & SC_ScrollBarSubLine) {
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarSubLine);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarSubLine))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarSubLine, &newScrollbar, p);
                }
            }
            if (scrollbar->subControls & SC_ScrollBarAddLine) {
                newScrollbar.rect = scrollbar->rect;
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarAddLine);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarAddLine))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarAddLine, &newScrollbar, p);
                }
            }
            if (scrollbar->subControls & SC_ScrollBarSubPage) {
                newScrollbar.rect = scrollbar->rect;
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarSubPage);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarSubPage))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarSubPage, &newScrollbar, p);
                }
            }
            if (scrollbar->subControls & SC_ScrollBarAddPage) {
                newScrollbar.rect = scrollbar->rect;
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarAddPage);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarAddPage))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarAddPage, &newScrollbar, p);
                }
            }
            if (scrollbar->subControls & SC_ScrollBarFirst) {
                newScrollbar.rect = scrollbar->rect;
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarFirst);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarFirst))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarFirst, &newScrollbar, p);
                }
            }
            if (scrollbar->subControls & SC_ScrollBarLast) {
                newScrollbar.rect = scrollbar->rect;
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarLast);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarLast))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarLast, &newScrollbar, p);
                }
            }
            if (scrollbar->subControls & SC_ScrollBarSlider) {
                newScrollbar.rect = scrollbar->rect;
                newScrollbar.state = saveFlags;
                newScrollbar.rect = proxy()->subControlRect(cc, &newScrollbar, SC_ScrollBarSlider);
                if (newScrollbar.rect.isValid()) {
                    if (!(scrollbar->activeSubControls & SC_ScrollBarSlider))
                        newScrollbar.state &= ~(State_Sunken | State_MouseOver);
                    proxy()->drawControl(CE_ScrollBarSlider, &newScrollbar, p);

                    if (scrollbar->state & State_HasFocus) {
                        QStyleOptionFocusRect fropt;
                        fropt.QStyleOption::operator=(newScrollbar);
                        fropt.rect.setRect(newScrollbar.rect.x() + 2, newScrollbar.rect.y() + 2,
                                           newScrollbar.rect.width() - 5,
                                           newScrollbar.rect.height() - 5);
                        proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, p);
                    }
                }
            }
        }
        break;
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
            QStyleOptionSpinBox copy = *sb;
            PrimitiveElement pe;

            if (sb->frame && (sb->subControls & SC_SpinBoxFrame)) {
                QRect r = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxFrame);
                qDrawWinPanel(p, r, sb->palette, true);
            }

            if (sb->subControls & SC_SpinBoxUp) {
                copy.subControls = SC_SpinBoxUp;
                QPalette pal2 = sb->palette;
                if (!(sb->stepEnabled & QStyleOptionSpinBox::StepUpEnabled)) {
                    pal2.setCurrentColorGroup(QPalette::Disabled);
                    copy.state &= ~State_Enabled;
                }

                copy.palette = pal2;

                if (sb->activeSubControls == SC_SpinBoxUp && (sb->state & State_Sunken)) {
                    copy.state |= State_On;
                    copy.state |= State_Sunken;
                } else {
                    copy.state |= State_Raised;
                    copy.state &= ~State_Sunken;
                }
                pe = (sb->buttonSymbols == QStyleOptionSpinBox::PlusMinus ? PE_IndicatorSpinPlus
                      : PE_IndicatorSpinUp);

                copy.rect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxUp);
                proxy()->drawPrimitive(PE_PanelButtonBevel, &copy, p);
                copy.rect.adjust(3, 0, -4, 0);
                proxy()->drawPrimitive(pe, &copy, p);
            }

            if (sb->subControls & SC_SpinBoxDown) {
                copy.subControls = SC_SpinBoxDown;
                copy.state = sb->state;
                QPalette pal2 = sb->palette;
                if (!(sb->stepEnabled & QStyleOptionSpinBox::StepDownEnabled)) {
                    pal2.setCurrentColorGroup(QPalette::Disabled);
                    copy.state &= ~State_Enabled;
                }
                copy.palette = pal2;

                if (sb->activeSubControls == SC_SpinBoxDown && (sb->state & State_Sunken)) {
                    copy.state |= State_On;
                    copy.state |= State_Sunken;
                } else {
                    copy.state |= State_Raised;
                    copy.state &= ~State_Sunken;
                }
                pe = (sb->buttonSymbols == QStyleOptionSpinBox::PlusMinus ? PE_IndicatorSpinMinus
                      : PE_IndicatorSpinDown);

                copy.rect = proxy()->subControlRect(CC_SpinBox, sb, SC_SpinBoxDown);
                proxy()->drawPrimitive(PE_PanelButtonBevel, &copy, p);
                copy.rect.adjust(3, 0, -4, 0);
                proxy()->drawPrimitive(pe, &copy, p);
            }
        }
        break;
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolbutton
            = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect button, menuarea;
            button = proxy()->subControlRect(cc, toolbutton, SC_ToolButton);
            menuarea = proxy()->subControlRect(cc, toolbutton, SC_ToolButtonMenu);

            State bflags = toolbutton->state & ~State_Sunken;

            if (bflags & State_AutoRaise) {
                if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) {
                    bflags &= ~State_Raised;
                }
            }
            State mflags = bflags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    bflags |= State_Sunken;
                mflags |= State_Sunken;
            }

            QStyleOption tool = *toolbutton;
            if (toolbutton->subControls & SC_ToolButton) {
                if (bflags & (State_Sunken | State_On | State_Raised)) {
                    tool.rect = button;
                    tool.state = bflags;
                    proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);
                }
            }

            if (toolbutton->state & State_HasFocus) {
                QStyleOptionFocusRect fr;
                fr.QStyleOption::operator=(*toolbutton);
                fr.rect.adjust(3, 3, -3, -3);
                if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
                    fr.rect.adjust(0, 0, -proxy()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                                                      toolbutton), 0);
                proxy()->drawPrimitive(PE_FrameFocusRect, &fr, p);
            }
            QStyleOptionToolButton label = *toolbutton;
            label.state = bflags;
            int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt);
            label.rect = button.adjusted(fw, fw, -fw, -fw);
            proxy()->drawControl(CE_ToolButtonLabel, &label, p);

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                tool.rect = menuarea;
                tool.state = mflags;
                if (mflags & (State_Sunken | State_On | State_Raised))
                    proxy()->drawPrimitive(PE_IndicatorButtonDropDown, &tool, p);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p);
            } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton);
                QRect ir = toolbutton->rect;
                QStyleOptionToolButton newBtn = *toolbutton;
                newBtn.rect = QRect(ir.right() + 5 - mbi, ir.y() + ir.height() - mbi + 4, mbi - 6, mbi - 6);
                newBtn.rect = visualRect(toolbutton->direction, button, newBtn.rect);
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p);
            }
        }
        break;
    case CC_TitleBar:
        if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) {
            QRect ir;
            if (opt->subControls & SC_TitleBarLabel) {
                QColor left = tb->palette.highlight().color();
                QColor right = tb->palette.base().color();

                QBrush fillBrush(left);
                if (left != right) {
                    QPoint p1(tb->rect.x(), tb->rect.top() + tb->rect.height()/2);
                    QPoint p2(tb->rect.right(), tb->rect.top() + tb->rect.height()/2);
                    QLinearGradient lg(p1, p2);
                    lg.setColorAt(0, left);
                    lg.setColorAt(1, right);
                    fillBrush = lg;
                }

                p->fillRect(opt->rect, fillBrush);

                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarLabel);

                p->setPen(tb->palette.highlightedText().color());
                p->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(),
                            Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
            }

            bool down = false;
            QPixmap pm;

            QStyleOption tool = *tb;
            if (tb->subControls & SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarCloseButton);
                down = tb->activeSubControls & SC_TitleBarCloseButton && (opt->state & State_Sunken);
                if ((tb->titleBarFlags & Qt::WindowType_Mask) == Qt::Tool)
                    pm = proxy()->standardIcon(SP_DockWidgetCloseButton, &tool).pixmap(opt->window, QSize(10, 10));
                else
                    pm = proxy()->standardIcon(SP_TitleBarCloseButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);

                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }

            if (tb->subControls & SC_TitleBarMaxButton
                    && tb->titleBarFlags & Qt::WindowMaximizeButtonHint
                    && !(tb->titleBarState & Qt::WindowMaximized)) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarMaxButton);

                down = tb->activeSubControls & SC_TitleBarMaxButton && (opt->state & State_Sunken);
                pm = proxy()->standardIcon(SP_TitleBarMaxButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);

                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }

            if (tb->subControls & SC_TitleBarMinButton
                    && tb->titleBarFlags & Qt::WindowMinimizeButtonHint
                    && !(tb->titleBarState & Qt::WindowMinimized)) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarMinButton);
                down = tb->activeSubControls & SC_TitleBarMinButton && (opt->state & State_Sunken);
                pm = proxy()->standardIcon(SP_TitleBarMinButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);

                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }

            bool drawNormalButton = (tb->subControls & SC_TitleBarNormalButton)
                                    && (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
                                    && (tb->titleBarState & Qt::WindowMinimized))
                                    || ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
                                    && (tb->titleBarState & Qt::WindowMaximized)));

            if (drawNormalButton) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarNormalButton);
                down = tb->activeSubControls & SC_TitleBarNormalButton && (opt->state & State_Sunken);
                pm = proxy()->standardIcon(SP_TitleBarNormalButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);

                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }

            if (tb->subControls & SC_TitleBarShadeButton
                    && tb->titleBarFlags & Qt::WindowShadeButtonHint
                    && !(tb->titleBarState & Qt::WindowMinimized)) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarShadeButton);
                down = (tb->activeSubControls & SC_TitleBarShadeButton && (opt->state & State_Sunken));
                pm = proxy()->standardIcon(SP_TitleBarShadeButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);
                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }

            if (tb->subControls & SC_TitleBarUnshadeButton
                    && tb->titleBarFlags & Qt::WindowShadeButtonHint
                    && tb->titleBarState & Qt::WindowMinimized) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarUnshadeButton);

                down = tb->activeSubControls & SC_TitleBarUnshadeButton  && (opt->state & State_Sunken);
                pm = proxy()->standardIcon(SP_TitleBarUnshadeButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);
                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }
            if (tb->subControls & SC_TitleBarContextHelpButton
                    && tb->titleBarFlags & Qt::WindowContextHelpButtonHint) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarContextHelpButton);

                down = tb->activeSubControls & SC_TitleBarContextHelpButton  && (opt->state & State_Sunken);
                pm = proxy()->standardIcon(SP_TitleBarContextHelpButton, &tool).pixmap(opt->window, QSize(10, 10));
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p);
                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb),
                                 proxy()->pixelMetric(PM_ButtonShiftVertical, tb));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }
            if (tb->subControls & SC_TitleBarSysMenu && tb->titleBarFlags & Qt::WindowSystemMenuHint) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarSysMenu);
                if (!tb->icon.isNull()) {
                    tb->icon.paint(p, ir);
                } else {
                    int iconSize = proxy()->pixelMetric(PM_SmallIconSize, tb);
                    pm = proxy()->standardIcon(SP_TitleBarMenuButton, &tool).pixmap(opt->window, QSize(iconSize, iconSize));
                    tool.rect = ir;
                    p->save();
                    proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                    p->restore();
                }
            }
        }
        break;
    case CC_Dial:
        if (const QStyleOptionSlider *dial = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            // OK, this is more a port of things over
            p->save();

            // avoid dithering
            if (p->paintEngine()->hasFeature(QPaintEngine::Antialiasing))
                p->setRenderHint(QPainter::Antialiasing);

            int width = dial->rect.width();
            int height = dial->rect.height();
            qreal r = qMin(width, height) / 2;
            qreal d_ = r / 6;
            qreal dx = dial->rect.x() + d_ + (width - 2 * r) / 2 + 1;
            qreal dy = dial->rect.y() + d_ + (height - 2 * r) / 2 + 1;
            QRect br = QRect(int(dx), int(dy), int(r * 2 - 2 * d_ - 2), int(r * 2 - 2 * d_ - 2));

            QPalette pal = opt->palette;
            // draw notches
            if (dial->subControls & QStyle::SC_DialTickmarks) {
                p->setPen(pal.windowText().color());
                p->drawLines(QStyleHelper::calcLines(dial));
            }

            if (dial->state & State_Enabled) {
                p->setBrush(pal.brush(QPalette::ColorRole(proxy()->styleHint(SH_Dial_BackgroundRole, dial))));
                p->setPen(Qt::NoPen);
                p->drawEllipse(br);
                p->setBrush(Qt::NoBrush);
            }
            p->setPen(QPen(pal.dark().color()));
            p->drawArc(br, 60 * 16, 180 * 16);
            p->setPen(QPen(pal.light().color()));
            p->drawArc(br, 240 * 16, 180 * 16);

            qreal a;
            const StaticPolygonF<3> arrow = calcArrow(dial, a);

            p->setPen(Qt::NoPen);
            p->setBrush(pal.button());
            p->setRenderHint(QPainter::Qt4CompatiblePainting);
            p->drawPolygon(arrow.cbegin(), arrow.size());

            a = QStyleHelper::angle(QPointF(width / 2, height / 2), arrow[0]);
            p->setBrush(Qt::NoBrush);

            if (a <= 0 || a > 200) {
                p->setPen(pal.light().color());
                p->drawLine(arrow[2], arrow[0]);
                p->drawLine(arrow[1], arrow[2]);
                p->setPen(pal.dark().color());
                p->drawLine(arrow[0], arrow[1]);
            } else if (a > 0 && a < 45) {
                p->setPen(pal.light().color());
                p->drawLine(arrow[2], arrow[0]);
                p->setPen(pal.dark().color());
                p->drawLine(arrow[1], arrow[2]);
                p->drawLine(arrow[0], arrow[1]);
            } else if (a >= 45 && a < 135) {
                p->setPen(pal.dark().color());
                p->drawLine(arrow[2], arrow[0]);
                p->drawLine(arrow[1], arrow[2]);
                p->setPen(pal.light().color());
                p->drawLine(arrow[0], arrow[1]);
            } else if (a >= 135 && a < 200) {
                p->setPen(pal.dark().color());
                p->drawLine(arrow[2], arrow[0]);
                p->setPen(pal.light().color());
                p->drawLine(arrow[0], arrow[1]);
                p->drawLine(arrow[1], arrow[2]);
            }

            // draw focus rect around the dial
            QStyleOptionFocusRect fropt;
            fropt.rect = dial->rect;
            fropt.state = dial->state;
            fropt.palette = dial->palette;
            if (fropt.state & QStyle::State_HasFocus) {
                br.adjust(0, 0, 2, 2);
                if (dial->subControls & SC_DialTickmarks) {
                    int r = qMin(width, height) / 2;
                    br.translate(-r / 6, - r / 6);
                    br.setWidth(br.width() + r / 3);
                    br.setHeight(br.height() + r / 3);
                }
                fropt.rect = br.adjusted(-2, -2, 2, 2);
                proxy()->drawPrimitive(QStyle::PE_FrameFocusRect, &fropt, p);
            }
            p->restore();
        }
        break;
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            // Draw frame
            QRect textRect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxLabel);
            QRect checkBoxRect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxCheckBox);
            if (groupBox->subControls & QStyle::SC_GroupBoxFrame) {
                QStyleOptionFrame frame;
                frame.QStyleOption::operator=(*groupBox);
                frame.features = groupBox->features;
                frame.lineWidth = groupBox->lineWidth;
                frame.midLineWidth = groupBox->midLineWidth;
                frame.rect = proxy()->subControlRect(CC_GroupBox, opt, SC_GroupBoxFrame);
                p->save();
                QRegion region(groupBox->rect);
                if (!groupBox->text.isEmpty()) {
                    bool ltr = groupBox->direction == Qt::LeftToRight;
                    QRect finalRect;
                    if (groupBox->subControls & QStyle::SC_GroupBoxCheckBox) {
                        finalRect = checkBoxRect.united(textRect);
                        finalRect.adjust(ltr ? -4 : 0, 0, ltr ? 0 : 4, 0);
                    } else {
                        finalRect = textRect;
                    }
                    region -= finalRect;
                }
                p->setClipRegion(region);
                proxy()->drawPrimitive(PE_FrameGroupBox, &frame, p);
                p->restore();
            }

            // Draw title
            if ((groupBox->subControls & QStyle::SC_GroupBoxLabel) && !groupBox->text.isEmpty()) {
                QColor textColor = groupBox->textColor;
                if (textColor.isValid())
                    p->setPen(textColor);
                int alignment = int(groupBox->textAlignment);
                if (!proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt))
                    alignment |= Qt::TextHideMnemonic;

                proxy()->drawItemText(p, textRect,  Qt::TextShowMnemonic | Qt::AlignHCenter | alignment,
                             groupBox->palette, groupBox->state & State_Enabled, groupBox->text,
                             textColor.isValid() ? QPalette::NoRole : QPalette::WindowText);

                if (groupBox->state & State_HasFocus) {
                    QStyleOptionFocusRect fropt;
                    fropt.QStyleOption::operator=(*groupBox);
                    fropt.rect = textRect;
                    proxy()->drawPrimitive(PE_FrameFocusRect, &fropt, p);
                }
            }

            // Draw checkbox
            if (groupBox->subControls & SC_GroupBoxCheckBox) {
                QStyleOptionButton box;
                box.QStyleOption::operator=(*groupBox);
                box.rect = checkBoxRect;
                proxy()->drawPrimitive(PE_IndicatorCheckBox, &box, p);
            }
        }
        break;
    case CC_MdiControls:
        {
            QStyleOptionButton btnOpt;
            btnOpt.QStyleOption::operator=(*opt);
            btnOpt.state &= ~State_MouseOver;
            int bsx = 0;
            int bsy = 0;
            const int buttonIconMetric = proxy()->pixelMetric(PM_TitleBarButtonIconSize, &btnOpt);
            const QSize buttonIconSize(buttonIconMetric, buttonIconMetric);
            if (opt->subControls & QStyle::SC_MdiCloseButton) {
                if (opt->activeSubControls & QStyle::SC_MdiCloseButton && (opt->state & State_Sunken)) {
                    btnOpt.state |= State_Sunken;
                    btnOpt.state &= ~State_Raised;
                    bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal);
                    bsy = proxy()->pixelMetric(PM_ButtonShiftVertical);
                } else {
                    btnOpt.state |= State_Raised;
                    btnOpt.state &= ~State_Sunken;
                    bsx = 0;
                    bsy = 0;
                }
                btnOpt.rect = proxy()->subControlRect(CC_MdiControls, opt, SC_MdiCloseButton);
                proxy()->drawPrimitive(PE_PanelButtonCommand, &btnOpt, p);
                QPixmap pm = proxy()->standardIcon(SP_TitleBarCloseButton).pixmap(opt->window, buttonIconSize);
                proxy()->drawItemPixmap(p, btnOpt.rect.translated(bsx, bsy), Qt::AlignCenter, pm);
            }
            if (opt->subControls & QStyle::SC_MdiNormalButton) {
                if (opt->activeSubControls & QStyle::SC_MdiNormalButton && (opt->state & State_Sunken)) {
                    btnOpt.state |= State_Sunken;
                    btnOpt.state &= ~State_Raised;
                    bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal);
                    bsy = proxy()->pixelMetric(PM_ButtonShiftVertical);
                } else {
                    btnOpt.state |= State_Raised;
                    btnOpt.state &= ~State_Sunken;
                    bsx = 0;
                    bsy = 0;
                }
                btnOpt.rect = proxy()->subControlRect(CC_MdiControls, opt, SC_MdiNormalButton);
                proxy()->drawPrimitive(PE_PanelButtonCommand, &btnOpt, p);
                QPixmap pm = proxy()->standardIcon(SP_TitleBarNormalButton).pixmap(opt->window, buttonIconSize);
                proxy()->drawItemPixmap(p, btnOpt.rect.translated(bsx, bsy), Qt::AlignCenter, pm);
            }
            if (opt->subControls & QStyle::SC_MdiMinButton) {
                if (opt->activeSubControls & QStyle::SC_MdiMinButton && (opt->state & State_Sunken)) {
                    btnOpt.state |= State_Sunken;
                    btnOpt.state &= ~State_Raised;
                    bsx = proxy()->pixelMetric(PM_ButtonShiftHorizontal);
                    bsy = proxy()->pixelMetric(PM_ButtonShiftVertical);
                } else {
                    btnOpt.state |= State_Raised;
                    btnOpt.state &= ~State_Sunken;
                    bsx = 0;
                    bsy = 0;
                }
                btnOpt.rect = proxy()->subControlRect(CC_MdiControls, opt, SC_MdiMinButton);
                proxy()->drawPrimitive(PE_PanelButtonCommand, &btnOpt, p);
                QPixmap pm = proxy()->standardIcon(SP_TitleBarMinButton).pixmap(opt->window, buttonIconSize);
                proxy()->drawItemPixmap(p, btnOpt.rect.translated(bsx, bsy), Qt::AlignCenter, pm);
            }
        }
        break;
    default:
        qWarning("QCommonStyle::drawComplexControl: Control %d not handled", cc);
    }
}

/*!
    \reimp
*/
QStyle::SubControl QCommonStyle::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, const QPoint &pt) const
{
    SubControl sc = SC_None;
    switch (cc) {
    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            QRect r = proxy()->subControlRect(cc, slider, SC_SliderHandle);
            if (r.isValid() && r.contains(pt)) {
                sc = SC_SliderHandle;
            } else {
                r = proxy()->subControlRect(cc, slider, SC_SliderGroove);
                if (r.isValid() && r.contains(pt))
                    sc = SC_SliderGroove;
            }
        }
        break;
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            QRect r;
            uint ctrl = SC_ScrollBarAddLine;
            while (ctrl <= SC_ScrollBarGroove) {
                r = proxy()->subControlRect(cc, scrollbar, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt)) {
                    sc = QStyle::SubControl(ctrl);
                    break;
                }
                ctrl <<= 1;
            }
        }
        break;
    case CC_ToolButton:
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect r;
            uint ctrl = SC_ToolButton;
            while (ctrl <= SC_ToolButtonMenu) {
                r = proxy()->subControlRect(cc, toolbutton, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt)) {
                    sc = QStyle::SubControl(ctrl);
                    break;
                }
                ctrl <<= 1;
            }
        }
        break;
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
            QRect r;
            uint ctrl = SC_SpinBoxUp;
            while (ctrl <= SC_SpinBoxEditField) {
                r = proxy()->subControlRect(cc, spinbox, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt)) {
                    sc = QStyle::SubControl(ctrl);
                    break;
                }
                ctrl <<= 1;
            }
        }
        break;
    case CC_TitleBar:
        if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) {
            QRect r;
            uint ctrl = SC_TitleBarSysMenu;

            while (ctrl <= SC_TitleBarLabel) {
                r = proxy()->subControlRect(cc, tb, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt)) {
                    sc = QStyle::SubControl(ctrl);
                    break;
                }
                ctrl <<= 1;
            }
        }
        break;
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            QRect r;
            uint ctrl = SC_ComboBoxArrow;  // Start here and go down.
            while (ctrl > 0) {
                r = proxy()->subControlRect(cc, cb, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt)) {
                    sc = QStyle::SubControl(ctrl);
                    break;
                }
                ctrl >>= 1;
            }
        }
        break;
    case CC_GroupBox:
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            QRect r;
            uint ctrl = SC_GroupBoxCheckBox;
            while (ctrl <= SC_GroupBoxFrame) {
                r = proxy()->subControlRect(cc, groupBox, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt)) {
                    sc = QStyle::SubControl(ctrl);
                    break;
                }
                ctrl <<= 1;
            }
        }
        break;
    case CC_MdiControls:
        {
            QRect r;
            uint ctrl = SC_MdiMinButton;
            while (ctrl <= SC_MdiCloseButton) {
                r = proxy()->subControlRect(CC_MdiControls, opt, QStyle::SubControl(ctrl));
                if (r.isValid() && r.contains(pt) && (opt->subControls & ctrl)) {
                    sc = QStyle::SubControl(ctrl);
                    return sc;
                }
                ctrl <<= 1;
            }
        }
        break;
    default:
        qWarning("QCommonStyle::hitTestComplexControl: Case %d not handled", cc);
    }
    return sc;
}

/*!
    \reimp
*/
QRect QCommonStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc) const
{
    QRect ret;
    switch (cc) {
    case CC_Slider:
        if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            int tickOffset = proxy()->pixelMetric(PM_SliderTickmarkOffset, slider);
            int thickness = proxy()->pixelMetric(PM_SliderControlThickness, slider);

            switch (sc) {
            case SC_SliderHandle: {
                int sliderPos = 0;
                int len = proxy()->pixelMetric(PM_SliderLength, slider);
                bool horizontal = slider->orientation == Qt::Horizontal;
                sliderPos = sliderPositionFromValue(slider->minimum, slider->maximum,
                                                    slider->sliderPosition,
                                                    (horizontal ? slider->rect.width()
                                                                : slider->rect.height()) - len,
                                                    slider->upsideDown);
                if (horizontal)
                    ret.setRect(slider->rect.x() + sliderPos, slider->rect.y() + tickOffset, len, thickness);
                else
                    ret.setRect(slider->rect.x() + tickOffset, slider->rect.y() + sliderPos, thickness, len);
                break; }
            case SC_SliderGroove:
                if (slider->orientation == Qt::Horizontal)
                    ret.setRect(slider->rect.x(), slider->rect.y() + tickOffset,
                                slider->rect.width(), thickness);
                else
                    ret.setRect(slider->rect.x() + tickOffset, slider->rect.y(),
                                thickness, slider->rect.height());
                break;
            default:
                break;
            }
            ret = visualRect(slider->direction, slider->rect, ret);
        }
        break;
    case CC_ScrollBar:
        if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            const QRect scrollBarRect = scrollbar->rect;
            int sbextent = 0;
            if (!proxy()->styleHint(SH_ScrollBar_Transient, scrollbar))
                sbextent = proxy()->pixelMetric(PM_ScrollBarExtent, scrollbar);
            int maxlen = ((scrollbar->orientation == Qt::Horizontal) ?
                          scrollBarRect.width() : scrollBarRect.height()) - (sbextent * 2);
            int sliderlen;

            // calculate slider length
            if (scrollbar->maximum != scrollbar->minimum) {
                uint range = scrollbar->maximum - scrollbar->minimum;
                sliderlen = (qint64(scrollbar->pageStep) * maxlen) / (range + scrollbar->pageStep);

                int slidermin = proxy()->pixelMetric(PM_ScrollBarSliderMin, scrollbar);
                if (sliderlen < slidermin || range > INT_MAX / 2)
                    sliderlen = slidermin;
                if (sliderlen > maxlen)
                    sliderlen = maxlen;
            } else {
                sliderlen = maxlen;
            }

            int sliderstart = sbextent + sliderPositionFromValue(scrollbar->minimum,
                                                                 scrollbar->maximum,
                                                                 scrollbar->sliderPosition,
                                                                 maxlen - sliderlen,
                                                                 scrollbar->upsideDown);

            switch (sc) {
            case SC_ScrollBarSubLine:            // top/left button
                if (scrollbar->orientation == Qt::Horizontal) {
                    int buttonWidth = qMin(scrollBarRect.width() / 2, sbextent);
                    ret.setRect(0, 0, buttonWidth, scrollBarRect.height());
                } else {
                    int buttonHeight = qMin(scrollBarRect.height() / 2, sbextent);
                    ret.setRect(0, 0, scrollBarRect.width(), buttonHeight);
                }
                break;
            case SC_ScrollBarAddLine:            // bottom/right button
                if (scrollbar->orientation == Qt::Horizontal) {
                    int buttonWidth = qMin(scrollBarRect.width()/2, sbextent);
                    ret.setRect(scrollBarRect.width() - buttonWidth, 0, buttonWidth, scrollBarRect.height());
                } else {
                    int buttonHeight = qMin(scrollBarRect.height()/2, sbextent);
                    ret.setRect(0, scrollBarRect.height() - buttonHeight, scrollBarRect.width(), buttonHeight);
                }
                break;
            case SC_ScrollBarSubPage:            // between top/left button and slider
                if (scrollbar->orientation == Qt::Horizontal)
                    ret.setRect(sbextent, 0, sliderstart - sbextent, scrollBarRect.height());
                else
                    ret.setRect(0, sbextent, scrollBarRect.width(), sliderstart - sbextent);
                break;
            case SC_ScrollBarAddPage:            // between bottom/right button and slider
                if (scrollbar->orientation == Qt::Horizontal)
                    ret.setRect(sliderstart + sliderlen, 0,
                                maxlen - sliderstart - sliderlen + sbextent, scrollBarRect.height());
                else
                    ret.setRect(0, sliderstart + sliderlen, scrollBarRect.width(),
                                maxlen - sliderstart - sliderlen + sbextent);
                break;
            case SC_ScrollBarGroove:
                if (scrollbar->orientation == Qt::Horizontal)
                    ret.setRect(sbextent, 0, scrollBarRect.width() - sbextent * 2,
                                scrollBarRect.height());
                else
                    ret.setRect(0, sbextent, scrollBarRect.width(),
                                scrollBarRect.height() - sbextent * 2);
                break;
            case SC_ScrollBarSlider:
                if (scrollbar->orientation == Qt::Horizontal)
                    ret.setRect(sliderstart, 0, sliderlen, scrollBarRect.height());
                else
                    ret.setRect(0, sliderstart, scrollBarRect.width(), sliderlen);
                break;
            default:
                break;
            }
            ret = visualRect(scrollbar->direction, scrollBarRect, ret);
        }
        break;
    case CC_SpinBox:
        if (const QStyleOptionSpinBox *spinbox = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
            QSize bs;
            int fw = spinbox->frame ? proxy()->pixelMetric(PM_SpinBoxFrameWidth, spinbox) : 0;
            bs.setHeight(qMax(8, spinbox->rect.height()/2 - fw));
            // 1.6 -approximate golden mean
            bs.setWidth(qMax(16, qMin(bs.height() * 8 / 5, spinbox->rect.width() / 4)));
            int y = fw + spinbox->rect.y();
            int x, lx, rx;
            x = spinbox->rect.x() + spinbox->rect.width() - fw - bs.width();
            lx = fw;
            rx = x - fw;
            switch (sc) {
            case SC_SpinBoxUp:
                if (spinbox->buttonSymbols == QStyleOptionSpinBox::NoButtons)
                    return QRect();
                ret = QRect(x, y, bs.width(), bs.height());
                break;
            case SC_SpinBoxDown:
                if (spinbox->buttonSymbols == QStyleOptionSpinBox::NoButtons)
                    return QRect();

                ret = QRect(x, y + bs.height(), bs.width(), bs.height());
                break;
            case SC_SpinBoxEditField:
                if (spinbox->buttonSymbols == QStyleOptionSpinBox::NoButtons) {
                    ret = QRect(lx, fw, spinbox->rect.width() - 2*fw, spinbox->rect.height() - 2*fw);
                } else {
                    ret = QRect(lx, fw, rx, spinbox->rect.height() - 2*fw);
                }
                break;
            case SC_SpinBoxFrame:
                ret = spinbox->rect;
            default:
                break;
            }
            ret = visualRect(spinbox->direction, spinbox->rect, ret);
        }
        break;
    case CC_ToolButton:
        if (const QStyleOptionToolButton *tb = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, tb);
            ret = tb->rect;
            switch (sc) {
            case SC_ToolButton:
                if ((tb->features
                     & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay))
                    == QStyleOptionToolButton::MenuButtonPopup)
                    ret.adjust(0, 0, -mbi, 0);
                break;
            case SC_ToolButtonMenu:
                if ((tb->features
                     & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay))
                    == QStyleOptionToolButton::MenuButtonPopup)
                    ret.adjust(ret.width() - mbi, 0, 0, 0);
                break;
            default:
                break;
            }
            ret = visualRect(tb->direction, tb->rect, ret);
        }
        break;
    case CC_ComboBox:
        if (const QStyleOptionComboBox *cb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            const qreal dpi = QStyleHelper::dpi(opt);
            const int x = cb->rect.x(), y = cb->rect.y(), wi = cb->rect.width(), he = cb->rect.height();
            const int margin = cb->frame ? qRound(QStyleHelper::dpiScaled(3, dpi)) : 0;
            const int bmarg = cb->frame ? qRound(QStyleHelper::dpiScaled(2, dpi)) : 0;
            const int xpos = x + wi - bmarg - qRound(QStyleHelper::dpiScaled(16, dpi));


            switch (sc) {
            case SC_ComboBoxFrame:
                ret = cb->rect;
                break;
            case SC_ComboBoxArrow:
                ret.setRect(xpos, y + bmarg, qRound(QStyleHelper::dpiScaled(16, opt)), he - 2*bmarg);
                break;
            case SC_ComboBoxEditField:
                ret.setRect(x + margin, y + margin, wi - 2 * margin - qRound(QStyleHelper::dpiScaled(16, dpi)), he - 2 * margin);
                break;
            case SC_ComboBoxListBoxPopup:
                ret = cb->rect;
                break;
            default:
                break;
            }
            ret = visualRect(cb->direction, cb->rect, ret);
        }
        break;
    case CC_TitleBar:
        if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) {
            const int controlMargin = 2;
            const int controlHeight = tb->rect.height() - controlMargin *2;
            const int delta = controlHeight + controlMargin;
            int offset = 0;

            bool isMinimized = tb->titleBarState & Qt::WindowMinimized;
            bool isMaximized = tb->titleBarState & Qt::WindowMaximized;

            switch (sc) {
            case SC_TitleBarLabel:
                if (tb->titleBarFlags & (Qt::WindowTitleHint | Qt::WindowSystemMenuHint)) {
                    ret = tb->rect;
                    if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                        ret.adjust(delta, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowShadeButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                    if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                        ret.adjust(0, 0, -delta, 0);
                }
                break;
            case SC_TitleBarContextHelpButton:
                if (tb->titleBarFlags & Qt::WindowContextHelpButtonHint)
                    offset += delta;
                Q_FALLTHROUGH();
            case SC_TitleBarMinButton:
                if (!isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarMinButton)
                    break;
                Q_FALLTHROUGH();
            case SC_TitleBarNormalButton:
                if (isMinimized && (tb->titleBarFlags & Qt::WindowMinimizeButtonHint))
                    offset += delta;
                else if (isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarNormalButton)
                    break;
                Q_FALLTHROUGH();
            case SC_TitleBarMaxButton:
                if (!isMaximized && (tb->titleBarFlags & Qt::WindowMaximizeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarMaxButton)
                    break;
                Q_FALLTHROUGH();
            case SC_TitleBarShadeButton:
                if (!isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarShadeButton)
                    break;
                Q_FALLTHROUGH();
            case SC_TitleBarUnshadeButton:
                if (isMinimized && (tb->titleBarFlags & Qt::WindowShadeButtonHint))
                    offset += delta;
                else if (sc == SC_TitleBarUnshadeButton)
                    break;
                Q_FALLTHROUGH();
            case SC_TitleBarCloseButton:
                if (tb->titleBarFlags & Qt::WindowSystemMenuHint)
                    offset += delta;
                else if (sc == SC_TitleBarCloseButton)
                    break;
                ret.setRect(tb->rect.right() - offset, tb->rect.top() + controlMargin,
                            controlHeight, controlHeight);
                break;
            case SC_TitleBarSysMenu:
                if (tb->titleBarFlags & Qt::WindowSystemMenuHint) {
                    ret.setRect(tb->rect.left() + controlMargin, tb->rect.top() + controlMargin,
                                controlHeight, controlHeight);
                }
                break;
            default:
                break;
            }
            ret = visualRect(tb->direction, tb->rect, ret);
        }
        break;
    case CC_GroupBox: {
        if (const QStyleOptionGroupBox *groupBox = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            switch (sc) {
            case SC_GroupBoxFrame:
            case SC_GroupBoxContents: {
                int topMargin = 0;
                int topHeight = 0;
                int verticalAlignment = proxy()->styleHint(SH_GroupBox_TextLabelVerticalAlignment, groupBox);
                bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
                if (groupBox->text.size() || hasCheckBox) {
                    int checkBoxHeight = hasCheckBox ? proxy()->pixelMetric(PM_IndicatorHeight, groupBox) : 0;
                    topHeight = qMax(groupBox->fontMetrics.height(), checkBoxHeight);
                    if (verticalAlignment & Qt::AlignVCenter)
                        topMargin = topHeight / 2;
                    else if (verticalAlignment & Qt::AlignTop)
                        topMargin = topHeight;
                }

                QRect frameRect = groupBox->rect;
                frameRect.setTop(topMargin);

                if (sc == SC_GroupBoxFrame) {
                    ret = frameRect;
                    break;
                }

                int frameWidth = 0;
                if ((groupBox->features & QStyleOptionFrame::Flat) == 0)
                    frameWidth = proxy()->pixelMetric(PM_DefaultFrameWidth, groupBox);
                ret = frameRect.adjusted(frameWidth, frameWidth + topHeight - topMargin,
                                         -frameWidth, -frameWidth);
                break;
            }
            case SC_GroupBoxCheckBox:
            case SC_GroupBoxLabel: {
                QFontMetrics fontMetrics = groupBox->fontMetrics;
                int th = fontMetrics.height();
                int tw = fontMetrics.size(Qt::TextShowMnemonic, groupBox->text + QLatin1Char(' ')).width();
                int marg = (groupBox->features & QStyleOptionFrame::Flat) ? 0 : 8;
                ret = groupBox->rect.adjusted(marg, 0, -marg, 0);

                int indicatorWidth = proxy()->pixelMetric(PM_IndicatorWidth, opt);
                int indicatorHeight = proxy()->pixelMetric(PM_IndicatorHeight, opt);
                int indicatorSpace = proxy()->pixelMetric(PM_CheckBoxLabelSpacing, opt) - 1;
                bool hasCheckBox = groupBox->subControls & QStyle::SC_GroupBoxCheckBox;
                int checkBoxWidth = hasCheckBox ? (indicatorWidth + indicatorSpace) : 0;
                int checkBoxHeight = hasCheckBox ? indicatorHeight : 0;

                int h = qMax(th, checkBoxHeight);
                ret.setHeight(h);

                // Adjusted rect for label + indicatorWidth + indicatorSpace
                QRect totalRect = alignedRect(groupBox->direction, groupBox->textAlignment,
                                              QSize(tw + checkBoxWidth, h), ret);

                // Adjust totalRect if checkbox is set
                if (hasCheckBox) {
                    bool ltr = groupBox->direction == Qt::LeftToRight;
                    int left = 0;
                    // Adjust for check box
                    if (sc == SC_GroupBoxCheckBox) {
                        left = ltr ? totalRect.left() : (totalRect.right() - indicatorWidth);
                        int top = totalRect.top() + (h - checkBoxHeight) / 2;
                        totalRect.setRect(left, top, indicatorWidth, indicatorHeight);
                    // Adjust for label
                    } else {
                        left = ltr ? (totalRect.left() + checkBoxWidth - 2) : totalRect.left();
                        int top = totalRect.top() + (h - th) / 2;
                        totalRect.setRect(left, top, totalRect.width() - checkBoxWidth, th);
                    }
                }
                ret = totalRect;
                break;
            }
            default:
                break;
            }
        }
        break;
    }
    case CC_MdiControls:
    {
        int numSubControls = 0;
        if (opt->subControls & SC_MdiCloseButton)
            ++numSubControls;
        if (opt->subControls & SC_MdiMinButton)
            ++numSubControls;
        if (opt->subControls & SC_MdiNormalButton)
            ++numSubControls;
        if (numSubControls == 0)
            break;

        int buttonWidth = opt->rect.width() / numSubControls - 1;
        int offset = 0;
        switch (sc) {
        case SC_MdiCloseButton:
            // Only one sub control, no offset needed.
            if (numSubControls == 1)
                break;
            offset += buttonWidth + 2;
            Q_FALLTHROUGH();
        case SC_MdiNormalButton:
            // No offset needed if
            // 1) There's only one sub control
            // 2) We have a close button and a normal button (offset already added in SC_MdiClose)
            if (numSubControls == 1 || (numSubControls == 2 && !(opt->subControls & SC_MdiMinButton)))
                break;
            if (opt->subControls & SC_MdiNormalButton)
                offset += buttonWidth;
            break;
        default:
            break;
        }

        // Subtract one pixel if we only have one sub control. At this point
        // buttonWidth is the actual width + 1 pixel margin, but we don't want the
        // margin when there are no other controllers.
        if (numSubControls == 1)
            --buttonWidth;
        ret = QRect(offset, 0, buttonWidth, opt->rect.height());
        break; }
     default:
        qWarning("QCommonStyle::subControlRect: Case %d not handled", cc);
    }

    return ret;
}

int QCommonStyle::pixelMetric(PixelMetric m, const QStyleOption *opt) const
{
    int ret;

    switch (m) {
    case PM_FocusFrameVMargin:
    case PM_FocusFrameHMargin:
        ret = 2;
        break;
    case PM_MenuBarVMargin:
    case PM_MenuBarHMargin:
        ret = 0;
        break;
    case PM_DialogButtonsSeparator:
        ret = int(QStyleHelper::dpiScaled(5, opt));
        break;
    case PM_DialogButtonsButtonWidth:
        ret = int(QStyleHelper::dpiScaled(70, opt));
        break;
    case PM_DialogButtonsButtonHeight:
        ret = int(QStyleHelper::dpiScaled(30, opt));
        break;
    case PM_TitleBarHeight: {
        if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) {
            if ((tb->titleBarFlags & Qt::WindowType_Mask) == Qt::Tool) {
                ret = qMax(opt->fontMetrics.height(), 16);
            } else {
                ret = qMax(opt->fontMetrics.height(), 18);
            }
        } else {
            ret = int(QStyleHelper::dpiScaled(18., opt));
        }

        break; }
    case PM_TitleBarButtonSize:
        ret = int(QStyleHelper::dpiScaled(16., opt));
        break;
    case PM_TitleBarButtonIconSize:
        ret = int(QStyleHelper::dpiScaled(16., opt));
        break;

    case PM_ScrollBarSliderMin:
        ret = int(QStyleHelper::dpiScaled(9., opt));
        break;

    case PM_ButtonMargin:
        ret = int(QStyleHelper::dpiScaled(6., opt));
        break;

    case PM_DockWidgetTitleBarButtonMargin:
        ret = int(QStyleHelper::dpiScaled(2., opt));
        break;

    case PM_ButtonDefaultIndicator:
        ret = 0;
        break;

    case PM_MenuButtonIndicator:
        ret = int(QStyleHelper::dpiScaled(12, opt));
        break;

    case PM_ButtonShiftHorizontal:
    case PM_ButtonShiftVertical:

    case PM_DefaultFrameWidth:
        ret = 2;
        break;

    case PM_ComboBoxFrameWidth:
    case PM_SpinBoxFrameWidth:
    case PM_MenuPanelWidth:
    case PM_TabBarBaseOverlap:
    case PM_TabBarBaseHeight:
        ret = proxy()->pixelMetric(PM_DefaultFrameWidth, opt);
        break;
    case PM_MdiSubWindowFrameWidth:
        ret = int(QStyleHelper::dpiScaled(4, opt));
        break;
    case PM_MdiSubWindowMinimizedWidth:
        ret = int(QStyleHelper::dpiScaled(196, opt));
        break;
    case PM_ScrollBarExtent:
        ret = int(QStyleHelper::dpiScaled(16, opt));
        break;
    case PM_MaximumDragDistance:
        ret = QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::MaximumScrollBarDragDistance).toInt();
        break;
    case PM_SliderThickness:
        ret = int(QStyleHelper::dpiScaled(16, opt));
        break;
    case PM_SliderTickmarkOffset:
        if (const QStyleOptionSlider *sl = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            int space = (sl->orientation == Qt::Horizontal) ? sl->rect.height()
                                                            : sl->rect.width();
            int thickness = proxy()->pixelMetric(PM_SliderControlThickness, sl);
            int ticks = sl->tickPosition;

            if (ticks == QStyleOptionSlider::TicksBothSides)
                ret = (space - thickness) / 2;
            else if (ticks == QStyleOptionSlider::TicksAbove)
                ret = space - thickness;
            else
                ret = 0;
        } else {
            ret = 0;
        }
        break;
    case PM_SliderSpaceAvailable:
        if (const QStyleOptionSlider *sl = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
            if (sl->orientation == Qt::Horizontal)
                ret = sl->rect.width() - proxy()->pixelMetric(PM_SliderLength, sl);
            else
                ret = sl->rect.height() - proxy()->pixelMetric(PM_SliderLength, sl);
        } else {
            ret = 0;
        }
        break;
    case PM_DockWidgetSeparatorExtent:
        ret = int(QStyleHelper::dpiScaled(6, opt));
        break;

    case PM_DockWidgetHandleExtent:
        ret = int(QStyleHelper::dpiScaled(8, opt));
        break;
    case PM_DockWidgetTitleMargin:
        ret = 0;
        break;
    case PM_DockWidgetFrameWidth:
        ret = 1;
        break;
    case PM_SpinBoxSliderHeight:
    case PM_MenuBarPanelWidth:
        ret = 2;
        break;
    case PM_MenuBarItemSpacing:
        ret = 0;
        break;
    case PM_ToolBarFrameWidth:
        ret = 1;
        break;

    case PM_ToolBarItemMargin:
        ret = 0;
        break;

    case PM_ToolBarItemSpacing:
        ret = int(QStyleHelper::dpiScaled(4, opt));
        break;

    case PM_ToolBarHandleExtent:
        ret = int(QStyleHelper::dpiScaled(8, opt));
        break;

    case PM_ToolBarSeparatorExtent:
        ret = int(QStyleHelper::dpiScaled(6, opt));
        break;

    case PM_ToolBarExtensionExtent:
        ret = int(QStyleHelper::dpiScaled(12, opt));
        break;

    case PM_TabBarTabOverlap:
        ret = 3;
        break;

    case PM_TabBarTabHSpace:
        ret = int(QStyleHelper::dpiScaled(24, opt));
        break;

    case PM_TabBarTabShiftHorizontal:
        ret = 0;
        break;

    case PM_TabBarTabShiftVertical:
        ret = 2;
        break;

    case PM_TabBarTabVSpace: {
        const QStyleOptionTab *tb = qstyleoption_cast<const QStyleOptionTab *>(opt);
        if (tb && (tb->shape == QStyleOptionTab::RoundedNorth || tb->shape == QStyleOptionTab::RoundedSouth
                   || tb->shape == QStyleOptionTab::RoundedWest || tb->shape == QStyleOptionTab::RoundedEast))
            ret = 8;
        else
            if(tb && (tb->shape == QStyleOptionTab::TriangularWest || tb->shape == QStyleOptionTab::TriangularEast))
                ret = 3;
            else
                ret = 2;
        break; }

    case PM_ProgressBarChunkWidth:
        ret = 9;
        break;

    case PM_IndicatorWidth:
        ret = int(QStyleHelper::dpiScaled(13, opt));
        break;

    case PM_IndicatorHeight:
        ret = int(QStyleHelper::dpiScaled(13, opt));
        break;

    case PM_ExclusiveIndicatorWidth:
        ret = int(QStyleHelper::dpiScaled(12, opt));
        break;

    case PM_ExclusiveIndicatorHeight:
        ret = int(QStyleHelper::dpiScaled(12, opt));
        break;

    case PM_MenuTearoffHeight:
        ret = int(QStyleHelper::dpiScaled(10, opt));
        break;

    case PM_MenuScrollerHeight:
        ret = int(QStyleHelper::dpiScaled(10, opt));
        break;

    case PM_MenuDesktopFrameWidth:
    case PM_MenuHMargin:
    case PM_MenuVMargin:
        ret = 0;
        break;

    case PM_HeaderMargin:
        ret = int(QStyleHelper::dpiScaled(4, opt));
        break;
    case PM_HeaderMarkSize:
        ret = int(QStyleHelper::dpiScaled(16, opt));
        break;
    case PM_HeaderGripMargin:
        ret = int(QStyleHelper::dpiScaled(4, opt));
        break;
    case PM_HeaderDefaultSectionSizeHorizontal:
        ret = int(QStyleHelper::dpiScaled(100, opt));
        break;
    case PM_HeaderDefaultSectionSizeVertical:
        ret = int(QStyleHelper::dpiScaled(30, opt));
        break;
    case PM_TabBarScrollButtonWidth:
        ret = int(QStyleHelper::dpiScaled(16, opt));
        break;
    case PM_LayoutLeftMargin:
    case PM_LayoutTopMargin:
    case PM_LayoutRightMargin:
    case PM_LayoutBottomMargin:
        {
            bool isWindow = opt ? (opt->state & State_Window) : false;
            ret = proxy()->pixelMetric(isWindow ? PM_DefaultTopLevelMargin : PM_DefaultChildMargin);
        }
        break;
    case PM_LayoutHorizontalSpacing:
    case PM_LayoutVerticalSpacing:
        ret = proxy()->pixelMetric(PM_DefaultLayoutSpacing);
        break;

    case PM_DefaultTopLevelMargin:
        ret = int(QStyleHelper::dpiScaled(11, opt));
        break;
    case PM_DefaultChildMargin:
        ret = int(QStyleHelper::dpiScaled(9, opt));
        break;
    case PM_DefaultLayoutSpacing:
        ret = int(QStyleHelper::dpiScaled(6, opt));
        break;

    case PM_ToolBarIconSize:
        ret = 0;
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme())
            ret = theme->themeHint(QPlatformTheme::ToolBarIconSize).toInt();
        if (ret <= 0)
            ret =  int(QStyleHelper::dpiScaled(24, opt));
        break;

    case PM_TabBarIconSize:
    case PM_ListViewIconSize:
        ret = proxy()->pixelMetric(PM_SmallIconSize, opt);
        break;

    case PM_ButtonIconSize:
    case PM_SmallIconSize:
        ret = int(QStyleHelper::dpiScaled(16, opt));
        break;
    case PM_IconViewIconSize:
        ret = proxy()->pixelMetric(PM_LargeIconSize, opt);
        break;

    case PM_LargeIconSize:
        ret = int(QStyleHelper::dpiScaled(32, opt));
        break;

    case PM_ToolTipLabelFrameWidth:
        ret = 1;
        break;
    case PM_CheckBoxLabelSpacing:
    case PM_RadioButtonLabelSpacing:
        ret = int(QStyleHelper::dpiScaled(6, opt));
        break;
    case PM_SizeGripSize:
        ret = int(QStyleHelper::dpiScaled(13, opt));
        break;
    case PM_MessageBoxIconSize:
#ifdef Q_OS_MAC
        if (QGuiApplication::desktopSettingsAware()) {
            ret = 64; // No DPI scaling, it's handled elsewhere.
        } else
#endif
        {
            ret = int(QStyleHelper::dpiScaled(32, opt));
        }
        break;
    case PM_TextCursorWidth:
        ret = QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::TextCursorWidth).toInt();
        break;
    case PM_TabBar_ScrollButtonOverlap:
        ret = 1;
        break;
    case PM_TabCloseIndicatorWidth:
    case PM_TabCloseIndicatorHeight:
        ret = int(QStyleHelper::dpiScaled(16, opt));
        break;
    case PM_ScrollView_ScrollBarSpacing:
        ret = 2 * proxy()->pixelMetric(PM_DefaultFrameWidth, opt);
        break;
    case PM_ScrollView_ScrollBarOverlap:
        ret = 0;
        break;
    case PM_SubMenuOverlap:
        ret = -proxy()->pixelMetric(QStyle::PM_MenuPanelWidth, opt);
        break;
    case PM_TreeViewIndentation:
        ret = int(QStyleHelper::dpiScaled(20, opt));
        break;
    default:
        ret = 0;
        break;
    }

    return ret;
}

QSize QCommonStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz) const
{
    Q_D(const QCommonStyle);
    QSize sz(csz);
    switch (ct) {
    case CT_PushButton:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            int w = csz.width(),
                h = csz.height(),
                bm = proxy()->pixelMetric(PM_ButtonMargin, btn),
            fw = proxy()->pixelMetric(PM_DefaultFrameWidth, btn) * 2;
            w += bm + fw;
            h += bm + fw;
            if (btn->features & QStyleOptionButton::AutoDefaultButton){
                int dbw = proxy()->pixelMetric(PM_ButtonDefaultIndicator, btn) * 2;
                w += dbw;
                h += dbw;
            }
            sz = QSize(w, h);
        }
        break;
    case CT_RadioButton:
    case CT_CheckBox:
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            bool isRadio = (ct == CT_RadioButton);

            int w = proxy()->pixelMetric(isRadio ? PM_ExclusiveIndicatorWidth
                                        : PM_IndicatorWidth, btn);
            int h = proxy()->pixelMetric(isRadio ? PM_ExclusiveIndicatorHeight
                                        : PM_IndicatorHeight, btn);

            int margins = 0;
            // we add 4 pixels for label margins
            if (!btn->icon.isNull() || !btn->text.isEmpty())
                margins = 4 + proxy()->pixelMetric(isRadio ? PM_RadioButtonLabelSpacing
                                                  : PM_CheckBoxLabelSpacing, opt);
            sz += QSize(w + margins, 4);
            sz.setHeight(qMax(sz.height(), h));
        }
        break;
    case CT_MenuItem:
        if (const QStyleOptionMenuItem *mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            bool checkable = mi->menuHasCheckableItems;
            int maxpmw = mi->maxIconWidth;
            int w = sz.width(), h = sz.height();
            if (mi->menuItemType == QStyleOptionMenuItem::Separator) {
                w = 10;
                h = 2;
            } else {
                h =  mi->fontMetrics.height() + 8;
                if (!mi->icon.isNull()) {
                    int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                    h = qMax(h, mi->icon.actualSize(QSize(iconExtent, iconExtent)).height() + 4);
                }
            }
            if (mi->text.contains(QLatin1Char('\t')))
                w += 12;
            if (maxpmw > 0)
                w += maxpmw + 6;
            if (checkable && maxpmw < 20)
                w += 20 - maxpmw;
            if (checkable || maxpmw > 0)
                w += 2;
            w += 12;
            sz = QSize(w, h);
        }
        break;
    case CT_ToolButton:
        sz = QSize(sz.width() + 6, sz.height() + 5);
        break;
    case CT_ComboBox:
        if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
            int fw = cmb->frame ? proxy()->pixelMetric(PM_ComboBoxFrameWidth, opt) * 2 : 0;
            const int textMargins = 2*(proxy()->pixelMetric(PM_FocusFrameHMargin) + 1);
            // QItemDelegate::sizeHint expands the textMargins two times, thus the 2*textMargins...
            int other = qMax(23, 2*textMargins + proxy()->pixelMetric(QStyle::PM_ScrollBarExtent, opt));
            sz = QSize(sz.width() + fw + other, sz.height() + fw);
        }
        break;
    case CT_HeaderSection:
        if (const QStyleOptionHeader *hdr = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
            bool nullIcon = hdr->icon.isNull();
            int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, hdr);
            int iconSize = nullIcon ? 0 : proxy()->pixelMetric(QStyle::PM_SmallIconSize, hdr);
            QSize txt = hdr->fontMetrics.size(0, hdr->text);
            sz.setHeight(margin + qMax(iconSize, txt.height()) + margin);
            sz.setWidth((nullIcon ? 0 : margin) + iconSize
                        + (hdr->text.isNull() ? 0 : margin) + txt.width() + margin);
            if (hdr->sortIndicator != QStyleOptionHeader::None) {
                int margin = proxy()->pixelMetric(QStyle::PM_HeaderMargin, hdr);
                if (hdr->orientation == Qt::Horizontal)
                    sz.rwidth() += sz.height() + margin;
                else
                    sz.rheight() += sz.width() + margin;
            }
        }
        break;
    case CT_TabWidget:
        sz += QSize(4, 4);
        break;
    case CT_LineEdit:
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            const int borderSize = 2 * f->lineWidth;
            sz += QSize(borderSize, borderSize);
            const int minSize = 10;
            if (sz.width() < minSize)
                sz.rwidth() = minSize;
            if (sz.height() < minSize)
                sz.rheight() = minSize;
        }
        break;
    case CT_GroupBox:
        if (const QStyleOptionGroupBox *styleOpt = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            if (sz.isEmpty())
                sz = QSize(20, 20);
            sz += QSize(styleOpt->features.testFlag(QStyleOptionFrame::Flat) ? 0 : 16, 0);
        }
        break;
    case CT_MdiControls:
        if (const QStyleOptionComplex *styleOpt = qstyleoption_cast<const QStyleOptionComplex *>(opt)) {
            const int buttonSize = proxy()->pixelMetric(PM_TitleBarButtonSize, styleOpt);
            int width = 1;
            if (styleOpt->subControls & SC_MdiMinButton)
                width += buttonSize + 1;
            if (styleOpt->subControls & SC_MdiNormalButton)
                width += buttonSize + 1;
            if (styleOpt->subControls & SC_MdiCloseButton)
                width += buttonSize + 1;
            sz = QSize(width, buttonSize);
        } else {
            const int buttonSize = proxy()->pixelMetric(PM_TitleBarButtonSize, opt);
            sz = QSize(1 + 3 * (buttonSize + 1), buttonSize);
        }
        break;
    case CT_ItemViewItem:
        if (const QStyleOptionViewItem *vopt = qstyleoption_cast<const QStyleOptionViewItem *>(opt)) {
            QRect decorationRect, displayRect, checkRect;
            d->viewItemLayout(vopt, &checkRect, &decorationRect, &displayRect, true);
            sz = (decorationRect|displayRect|checkRect).size();
            if (decorationRect.isValid() && sz.height() == decorationRect.height())
                sz.rheight() += 2; // Prevent icons from overlapping.
                      }
        break;
    case CT_SpinBox:
        if (const QStyleOptionSpinBox *vopt = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
            // Add button + frame widths
            const qreal dpi = QStyleHelper::dpi(opt);
            const bool hasButtons = (vopt->buttonSymbols != QStyleOptionSpinBox::NoButtons);
            const int buttonWidth = hasButtons ? qRound(QStyleHelper::dpiScaled(16, dpi)) : 0;
            const int fw = vopt->frame ? proxy()->pixelMetric(PM_SpinBoxFrameWidth, vopt) : 0;
            sz += QSize(buttonWidth + 2*fw, 2*fw);
        }
        break;
    case CT_Slider:
        if (const QStyleOptionSlider *option = qstyleoption_cast<const QStyleOptionSlider *>(opt))
            sz = subControlRect(QStyle::CC_Slider, option, QStyle::SC_SliderHandle).size();
        break;
    case CT_Frame:
        if (const QStyleOptionFrame *option = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
            const int ninePatchSplit = 1;
            int w = qMax(10, (option->lineWidth * 2) + ninePatchSplit);
            sz = QSize(w, w);
        }
        break;
    case CT_ProgressBar:
        if (sz.isNull()) {
            // Special case: return minimum nine patch image size
            sz = QSize(10, 10);
        }
        break;
    case CT_ScrollBar:
    case CT_MenuBar:
    case CT_Menu:
    case CT_MenuBarItem:
    case CT_TabBarTab:
        // just return the contentsSize for now
        Q_FALLTHROUGH();
    default:
        break;
    }
    return sz;
}

QFont QCommonStyle::font(QStyle::ControlElement element, const QStyle::State state) const
{
    Q_UNUSED(element);
    Q_UNUSED(state);
    return QGuiApplication::font();
}

QMargins QCommonStyle::ninePatchMargins(QStyle::ControlElement ce, const QStyleOption *opt, const QSize &imageSize) const
{
    // By default, we just divide the image at the center
    int w = imageSize.width() / 2;
    int h = imageSize.height() / 2;
    return QMargins(w, h, w, h);
}

QMargins QCommonStyle::ninePatchMargins(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, const QSize &imageSize) const
{
    // By default, we just divide the image at the center
    int w = imageSize.width() / 2;
    int h = imageSize.height() / 2;
    return QMargins(w, h, w, h);
}

int QCommonStyle::styleHint(StyleHint sh, const QStyleOption *opt, QStyleHintReturn *hret) const
{
    int ret = 0;

    switch (sh) {
    case SH_Menu_KeyboardSearch:
        ret = false;
        break;
    case SH_Slider_AbsoluteSetButtons:
        ret = Qt::MidButton;
        break;
    case SH_Slider_PageSetButtons:
        ret = Qt::LeftButton;
        break;
    case SH_ScrollBar_ContextMenu:
        ret = true;
        break;
    case SH_GroupBox_TextLabelVerticalAlignment:
        ret = Qt::AlignVCenter;
        break;
    case SH_GroupBox_TextLabelColor:
        ret = opt ? int(opt->palette.color(QPalette::Text).rgba()) : 0;
        break;

    case SH_ListViewExpand_SelectMouseType:
    case SH_TabBar_SelectMouseType:
        ret = QEvent::MouseButtonPress;
        break;

    case SH_TabBar_Alignment:
        ret = Qt::AlignLeft;
        break;

    case SH_Header_ArrowAlignment:
        ret = Qt::AlignRight | Qt::AlignVCenter;
        break;

    case SH_TitleBar_AutoRaise:
        ret = false;
        break;

    case SH_Menu_SubMenuPopupDelay:
        ret = 256;
        break;

    case SH_Menu_SloppySubMenus:
        ret = true;
        break;

    case SH_Menu_SubMenuUniDirection:
        ret = false;
        break;
    case SH_Menu_SubMenuUniDirectionFailCount:
        ret = 1;
        break;
    case SH_Menu_SubMenuSloppySelectOtherActions:
        ret = true;
        break;
    case SH_Menu_SubMenuSloppyCloseTimeout:
        ret = 1000;
        break;
    case SH_Menu_SubMenuResetWhenReenteringParent:
        ret = false;
        break;
    case SH_Menu_SubMenuDontStartSloppyOnLeave:
        ret = false;
        break;

    case SH_ProgressDialog_TextLabelAlignment:
        ret = Qt::AlignCenter;
        break;

    case SH_BlinkCursorWhenTextSelected:
#if defined(Q_OS_DARWIN)
        ret = 0;
#else
        ret = 1;
#endif
        break;

    case SH_Table_GridLineColor:
        if (opt)
            ret = opt->palette.color(QPalette::Mid).rgba();
        else
            ret = -1;
        break;
    case SH_LineEdit_PasswordCharacter: {
        const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme();
        const QPlatformTheme::ThemeHint hintType = QPlatformTheme::PasswordMaskCharacter;
        const QVariant hint = theme ? theme->themeHint(hintType) : QPlatformTheme::defaultThemeHint(hintType);
        ret = hint.toChar().unicode();
        break;
    }
    case SH_LineEdit_PasswordMaskDelay:
        ret = QGuiApplicationPrivate::platformTheme()->themeHint(QPlatformTheme::PasswordMaskDelay).toInt();
        break;
    case SH_ToolBox_SelectedPageTitleBold:
        ret = 1;
        break;

    case SH_UnderlineShortcut:
        ret = 1;
        break;

    case SH_SpinBox_ClickAutoRepeatRate:
        ret = 150;
        break;

    case SH_SpinBox_ClickAutoRepeatThreshold:
        ret = 500;
        break;

    case SH_SpinBox_KeyPressAutoRepeatRate:
        ret = 75;
        break;

    case SH_Menu_SelectionWrap:
        ret = true;
        break;

    case SH_Menu_FillScreenWithScroll:
        ret = true;
        break;

    case SH_ToolTipLabel_Opacity:
        ret = 255;
        break;

    case SH_Button_FocusPolicy:
        ret = Qt::StrongFocus;
        break;

    case SH_MessageBox_UseBorderForButtonSpacing:
        ret = 0;
        break;

    case SH_ToolButton_PopupDelay:
        ret = 600;
        break;

    case SH_FocusFrame_Mask:
        ret = 1;
        break;
    case SH_RubberBand_Mask:
        if (const QStyleOptionRubberBand *rbOpt = qstyleoption_cast<const QStyleOptionRubberBand *>(opt)) {
            ret = 0;
            if (rbOpt->shape == QStyleOptionRubberBand::Rectangle) {
                ret = true;
                if(QStyleHintReturnMask *mask = qstyleoption_cast<QStyleHintReturnMask*>(hret)) {
                    mask->region = opt->rect;
                    int margin = proxy()->pixelMetric(PM_DefaultFrameWidth) * 2;
                    mask->region -= opt->rect.adjusted(margin, margin, -margin, -margin);
                }
            }
        }
        break;
    case SH_SpinControls_DisableOnBounds:
        ret = 1;
        break;

    case SH_Dial_BackgroundRole:
        ret = QPalette::Window;
        break;

    case SH_ComboBox_LayoutDirection:
        ret = opt ? opt->direction : Qt::LeftToRight;
        break;

    case SH_ItemView_EllipsisLocation:
        ret = Qt::AlignTrailing;
        break;

    case SH_ItemView_ShowDecorationSelected:
        ret = false;
        break;

    case SH_ItemView_ActivateItemOnSingleClick:
        ret = 0;
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme())
            ret = theme->themeHint(QPlatformTheme::ItemViewActivateItemOnSingleClick).toBool() ? 1 : 0;
        break;
    case SH_TitleBar_ModifyNotification:
        ret = true;
        break;
    case SH_ScrollBar_RollBetweenButtons:
        ret = false;
        break;
    case SH_TabBar_ElideMode:
        ret = Qt::ElideNone;
        break;
    case SH_DialogButtonLayout:
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme())
            ret = theme->themeHint(QPlatformTheme::DialogButtonBoxLayout).toInt();
        break;
    case SH_ComboBox_PopupFrameStyle:
        ret = QStyleOptionFrame::StyledPanel | QStyleOptionFrame::Plain;
        break;
    case SH_MessageBox_TextInteractionFlags:
        ret = Qt::LinksAccessibleByMouse;
        break;
    case SH_DialogButtonBox_ButtonsHaveIcons:
        ret = 0;
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme())
            ret = theme->themeHint(QPlatformTheme::DialogButtonBoxButtonsHaveIcons).toBool() ? 1 : 0;
        break;
    case SH_SpellCheckUnderlineStyle:
        ret = QTextCharFormat::WaveUnderline;
        break;
    case SH_MessageBox_CenterButtons:
        ret = true;
        break;
    case SH_ItemView_MovementWithoutUpdatingSelection:
        ret = true;
        break;
    case SH_FocusFrame_AboveWidget:
        ret = false;
        break;
    case SH_TabWidget_DefaultTabPosition:
        ret = QStyleOptionTabBarBase::North;
        break;
    case SH_ToolBar_Movable:
        ret = true;
        break;
    case SH_TextControl_FocusIndicatorTextCharFormat:
        ret = true;
        if (QStyleHintReturnVariant *vret = qstyleoption_cast<QStyleHintReturnVariant*>(hret)) {
            QPen outline(opt->palette.color(QPalette::Text), 1, Qt::DotLine);
            QTextCharFormat fmt;
            fmt.setProperty(QTextFormat::OutlinePen, outline);
            vret->variant = fmt;
        }
        break;
    case SH_WizardStyle:
        break;
    case SH_FormLayoutWrapPolicy:
        break;
    case SH_FormLayoutFieldGrowthPolicy:
        break;
    case SH_FormLayoutFormAlignment:
        ret = Qt::AlignLeft | Qt::AlignTop;
        break;
    case SH_FormLayoutLabelAlignment:
        ret = Qt::AlignLeft;
        break;
    case SH_ItemView_ArrowKeysNavigateIntoChildren:
        ret = false;
        break;
    case SH_ItemView_DrawDelegateFrame:
        ret = 0;
        break;
    case SH_TabBar_CloseButtonPosition:
        ret = QStyleOptionTabBarBase::RightSide;
        break;
    case SH_TabBar_ChangeCurrentDelay:
        ret = 500;
        break;
    case SH_DockWidget_ButtonsHaveFrame:
        ret = true;
        break;
    case SH_ToolButtonStyle:
        ret = 0;
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme())
            ret = theme->themeHint(QPlatformTheme::ToolButtonStyle).toInt();
        break;
    case SH_RequestSoftwareInputPanel:
        ret = RSIP_OnMouseClick;
        break;
    case SH_ScrollBar_Transient:
        ret = false;
        break;
    case SH_Menu_SupportsSections:
        ret = false;
        break;
    case SH_ToolTip_WakeUpDelay:
        ret = 700;
        break;
    case SH_ToolTip_FallAsleepDelay:
        ret = 2000;
        break;
    case SH_Widget_Animate:
        ret = true;
        break;
    case SH_Splitter_OpaqueResize:
        ret = true;
        break;
    case SH_ItemView_ScrollMode:
        ret = QStyleOptionViewItem::ScrollPerItem;
        break;
    case SH_TitleBar_ShowToolTipsOnButtons:
        ret = true;
        break;
    case SH_Widget_Animation_Duration:
        ret = styleHint(SH_Widget_Animate, opt, hret) ? 200 : 0;
        break;
    case SH_ComboBox_AllowWheelScrolling:
        ret = true;
        break;
    case SH_SpinBox_ButtonsInsideFrame:
        ret = true;
        break;
    case SH_SpinBox_StepModifier:
        ret = Qt::ControlModifier;
        break;
    default:
        ret = 0;
        break;
    }

    return ret;
}

static QPixmap cachedPixmapFromXPM(const char * const *xpm)
{
    QPixmap result;
    const QString tag = QString::asprintf("xpm:0x%p", static_cast<const void*>(xpm));
    if (!QPixmapCache::find(tag, &result)) {
        result = QPixmap(xpm);
        QPixmapCache::insert(tag, result);
    }
    return result;
}

static inline QPixmap titleBarMenuCachedPixmapFromXPM() { return cachedPixmapFromXPM(qt_menu_xpm); }

static inline QString clearText16IconPath()
{
    return QStringLiteral(":/qt-project.org/styles/commonstyle/images/cleartext-16.png");
}

#if defined(Q_OS_WIN) || QT_CONFIG(imageformat_png)
static QIcon clearTextIcon(bool rtl)
{
    const QString directionalThemeName = rtl
        ? QStringLiteral("edit-clear-locationbar-ltr") : QStringLiteral("edit-clear-locationbar-rtl");
    if (QIcon::hasThemeIcon(directionalThemeName))
        return QIcon::fromTheme(directionalThemeName);
    const QString themeName = QStringLiteral("edit-clear");
    if (QIcon::hasThemeIcon(themeName))
        return QIcon::fromTheme(themeName);

    QIcon icon;
#ifndef QT_NO_IMAGEFORMAT_PNG
    QPixmap clearText16(clearText16IconPath());
    Q_ASSERT(!clearText16.size().isEmpty());
    icon.addPixmap(clearText16);
    QPixmap clearText32(QStringLiteral(":/qt-project.org/styles/commonstyle/images/cleartext-32.png"));
    Q_ASSERT(!clearText32.size().isEmpty());
    icon.addPixmap(clearText32);
    clearText32.setDevicePixelRatio(2); // The 32x32 pixmap can also be used for 16x16/devicePixelRatio=2
    icon.addPixmap(clearText32);
#endif // !QT_NO_IMAGEFORMAT_PNG
    return icon;
}
#endif

QPixmap QCommonStyle::standardPixmap(StandardPixmap sp, const QStyleOption *option) const
{
    const bool rtl = (option && option->direction == Qt::RightToLeft) || (!option && QGuiApplication::isRightToLeft());
#ifdef QT_NO_IMAGEFORMAT_PNG
    Q_UNUSED(widget);
    Q_UNUSED(sp);
#else
    QPixmap pixmap;

    if (QGuiApplication::desktopSettingsAware() && !QIcon::themeName().isEmpty()) {
        switch (sp) {
        case SP_DialogYesButton:
        case SP_DialogOkButton:
            pixmap = QIcon::fromTheme(QLatin1String("dialog-ok")).pixmap(16);
            break;
        case SP_DialogApplyButton:
            pixmap = QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(16);
            break;
        case SP_DialogDiscardButton:
            pixmap = QIcon::fromTheme(QLatin1String("edit-delete")).pixmap(16);
            break;
        case SP_DialogCloseButton:
            pixmap = QIcon::fromTheme(QLatin1String("dialog-close")).pixmap(16);
            break;
        case SP_DirHomeIcon:
            pixmap = QIcon::fromTheme(QLatin1String("user-home")).pixmap(16);
            break;
        case SP_MessageBoxInformation:
            pixmap = QIcon::fromTheme(QLatin1String("messagebox_info")).pixmap(16);
            break;
        case SP_MessageBoxWarning:
            pixmap = QIcon::fromTheme(QLatin1String("messagebox_warning")).pixmap(16);
            break;
        case SP_MessageBoxCritical:
            pixmap = QIcon::fromTheme(QLatin1String("messagebox_critical")).pixmap(16);
            break;
        case SP_MessageBoxQuestion:
            pixmap = QIcon::fromTheme(QLatin1String("help")).pixmap(16);
            break;
        case SP_DialogOpenButton:
        case SP_DirOpenIcon:
            pixmap = QIcon::fromTheme(QLatin1String("folder-open")).pixmap(16);
            break;
        case SP_FileIcon:
            pixmap = QIcon::fromTheme(QLatin1String("text-x-generic"),
                                      QIcon::fromTheme(QLatin1String("empty"))).pixmap(16);
            break;
        case SP_DirClosedIcon:
        case SP_DirIcon:
                pixmap = QIcon::fromTheme(QLatin1String("folder")).pixmap(16);
                break;
        case SP_DriveFDIcon:
                pixmap = QIcon::fromTheme(QLatin1String("media-floppy"),
                                          QIcon::fromTheme(QLatin1String("3floppy_unmount"))).pixmap(16);
                break;
        case SP_ComputerIcon:
                pixmap = QIcon::fromTheme(QLatin1String("computer"),
                                          QIcon::fromTheme(QLatin1String("system"))).pixmap(16);
                break;
        case SP_DesktopIcon:
                pixmap = QIcon::fromTheme(QLatin1String("user-desktop"),
                                          QIcon::fromTheme(QLatin1String("desktop"))).pixmap(16);
                break;
        case SP_TrashIcon:
                pixmap = QIcon::fromTheme(QLatin1String("user-trash"),
                                          QIcon::fromTheme(QLatin1String("trashcan_empty"))).pixmap(16);
                break;
        case SP_DriveCDIcon:
        case SP_DriveDVDIcon:
                pixmap = QIcon::fromTheme(QLatin1String("media-optical"),
                                          QIcon::fromTheme(QLatin1String("cdrom_unmount"))).pixmap(16);
                break;
        case SP_DriveHDIcon:
                pixmap = QIcon::fromTheme(QLatin1String("drive-harddisk"),
                                          QIcon::fromTheme(QLatin1String("hdd_unmount"))).pixmap(16);
                break;
        case SP_FileDialogToParent:
                pixmap = QIcon::fromTheme(QLatin1String("go-up"),
                                          QIcon::fromTheme(QLatin1String("up"))).pixmap(16);
                break;
        case SP_FileDialogNewFolder:
                pixmap = QIcon::fromTheme(QLatin1String("folder_new")).pixmap(16);
                break;
        case SP_ArrowUp:
                pixmap = QIcon::fromTheme(QLatin1String("go-up"),
                                          QIcon::fromTheme(QLatin1String("up"))).pixmap(16);
                break;
        case SP_ArrowDown:
                pixmap = QIcon::fromTheme(QLatin1String("go-down"),
                                          QIcon::fromTheme(QLatin1String("down"))).pixmap(16);
                break;
        case SP_ArrowRight:
                pixmap = QIcon::fromTheme(QLatin1String("go-next"),
                                          QIcon::fromTheme(QLatin1String("forward"))).pixmap(16);
                break;
        case SP_ArrowLeft:
                pixmap = QIcon::fromTheme(QLatin1String("go-previous"),
                                          QIcon::fromTheme(QLatin1String("back"))).pixmap(16);
                break;
        case SP_FileDialogDetailedView:
                pixmap = QIcon::fromTheme(QLatin1String("view_detailed")).pixmap(16);
                break;
        case SP_FileDialogListView:
                pixmap = QIcon::fromTheme(QLatin1String("view_icon")).pixmap(16);
                break;
        case SP_BrowserReload:
                pixmap = QIcon::fromTheme(QLatin1String("reload")).pixmap(16);
                break;
        case SP_BrowserStop:
                pixmap = QIcon::fromTheme(QLatin1String("process-stop")).pixmap(16);
                break;
        case SP_MediaPlay:
                pixmap = QIcon::fromTheme(QLatin1String("media-playback-start")).pixmap(16);
                break;
        case SP_MediaPause:
                pixmap = QIcon::fromTheme(QLatin1String("media-playback-pause")).pixmap(16);
                break;
        case SP_MediaStop:
                pixmap = QIcon::fromTheme(QLatin1String("media-playback-stop")).pixmap(16);
                break;
        case SP_MediaSeekForward:
                pixmap = QIcon::fromTheme(QLatin1String("media-seek-forward")).pixmap(16);
                break;
        case SP_MediaSeekBackward:
                pixmap = QIcon::fromTheme(QLatin1String("media-seek-backward")).pixmap(16);
                break;
        case SP_MediaSkipForward:
                pixmap = QIcon::fromTheme(QLatin1String("media-skip-forward")).pixmap(16);
                break;
        case SP_MediaSkipBackward:
                pixmap = QIcon::fromTheme(QLatin1String("media-skip-backward")).pixmap(16);
                break;
        case SP_DialogResetButton:
                pixmap = QIcon::fromTheme(QLatin1String("edit-clear")).pixmap(24);
                break;
        case SP_DialogHelpButton:
                pixmap = QIcon::fromTheme(QLatin1String("help-contents")).pixmap(24);
                break;
        case SP_DialogNoButton:
        case SP_DialogCancelButton:
                pixmap = QIcon::fromTheme(QLatin1String("dialog-cancel"),
                                         QIcon::fromTheme(QLatin1String("process-stop"))).pixmap(24);
                break;
        case SP_DialogSaveButton:
                pixmap = QIcon::fromTheme(QLatin1String("document-save")).pixmap(24);
                break;
        case SP_FileLinkIcon:
            pixmap = QIcon::fromTheme(QLatin1String("emblem-symbolic-link")).pixmap(16);
            if (!pixmap.isNull()) {
                QPixmap fileIcon = QIcon::fromTheme(QLatin1String("text-x-generic")).pixmap(16);
                if (fileIcon.isNull())
                    fileIcon = QIcon::fromTheme(QLatin1String("empty")).pixmap(16);
                if (!fileIcon.isNull()) {
                    QPainter painter(&fileIcon);
                    painter.drawPixmap(0, 0, 16, 16, pixmap);
                    return fileIcon;
                }
            }
            break;
        case SP_DirLinkIcon:
                pixmap = QIcon::fromTheme(QLatin1String("emblem-symbolic-link")).pixmap(16);
                if (!pixmap.isNull()) {
                    QPixmap dirIcon = QIcon::fromTheme(QLatin1String("folder")).pixmap(16);
                    if (!dirIcon.isNull()) {
                        QPainter painter(&dirIcon);
                        painter.drawPixmap(0, 0, 16, 16, pixmap);
                        return dirIcon;
                    }
                }
                break;
        case SP_LineEditClearButton:
            pixmap = clearTextIcon(rtl).pixmap(16);
            break;
        default:
            break;
        }
    }

    if (!pixmap.isNull())
        return pixmap;
#endif //QT_NO_IMAGEFORMAT_PNG
    switch (sp) {
#ifndef QT_NO_IMAGEFORMAT_XPM
    case SP_ToolBarHorizontalExtensionButton:
        if (rtl) {
            QImage im(tb_extension_arrow_h_xpm);
            im = im.convertToFormat(QImage::Format_ARGB32).mirrored(true, false);
            return QPixmap::fromImage(im);
        }
        return cachedPixmapFromXPM(tb_extension_arrow_h_xpm);
    case SP_ToolBarVerticalExtensionButton:
        return cachedPixmapFromXPM(tb_extension_arrow_v_xpm);
    case SP_FileDialogStart:
        return cachedPixmapFromXPM(filedialog_start_xpm);
    case SP_FileDialogEnd:
        return cachedPixmapFromXPM(filedialog_end_xpm);
#endif
#ifndef QT_NO_IMAGEFORMAT_PNG
    case SP_CommandLink:
    case SP_ArrowForward:
        if (rtl)
            return proxy()->standardPixmap(SP_ArrowLeft, option);
        return proxy()->standardPixmap(SP_ArrowRight, option);
    case SP_ArrowBack:
        if (rtl)
            return proxy()->standardPixmap(SP_ArrowRight, option);
        return proxy()->standardPixmap(SP_ArrowLeft, option);
    case SP_ArrowLeft:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/left-16.png"));
    case SP_ArrowRight:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/right-16.png"));
    case SP_ArrowUp:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/up-16.png"));
    case SP_ArrowDown:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/down-16.png"));
    case SP_FileDialogToParent:
        return proxy()->standardPixmap(SP_ArrowUp, option);
    case SP_FileDialogNewFolder:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/newdirectory-16.png"));
    case SP_FileDialogDetailedView:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewdetailed-16.png"));
    case SP_FileDialogInfoView:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/fileinfo-16.png"));
    case SP_FileDialogContentsView:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/filecontents-16.png"));
    case SP_FileDialogListView:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewlist-16.png"));
    case SP_FileDialogBack:
        return proxy()->standardPixmap(SP_ArrowBack, option);
    case SP_DriveHDIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/harddrive-16.png"));
    case SP_TrashIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/trash-16.png"));
    case SP_DriveFDIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/floppy-16.png"));
    case SP_DriveNetIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/networkdrive-16.png"));
    case SP_DesktopIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/desktop-16.png"));
    case SP_ComputerIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/computer-16.png"));
    case SP_DriveCDIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/cdr-16.png"));
    case SP_DriveDVDIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/dvd-16.png"));
    case SP_DirHomeIcon:
    case SP_DirOpenIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/diropen-16.png"));
    case SP_DirIcon:
    case SP_DirClosedIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/dirclosed-16.png"));
    case SP_DirLinkIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/dirlink-16.png"));
    case SP_FileIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/file-16.png"));
    case SP_FileLinkIcon:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/filelink-16.png"));
    case SP_DialogOkButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-ok-16.png"));
    case SP_DialogCancelButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-16.png"));
    case SP_DialogHelpButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-help-16.png"));
    case SP_DialogOpenButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-open-16.png"));
    case SP_DialogSaveButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-save-16.png"));
    case SP_DialogCloseButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-close-16.png"));
    case SP_DialogApplyButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-16.png"));
    case SP_DialogResetButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-clear-16.png"));
    case SP_DialogDiscardButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-delete-16.png"));
    case SP_DialogYesButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-yes-16.png"));
    case SP_DialogNoButton:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-no-16.png"));
    case SP_BrowserReload:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/refresh-24.png"));
    case SP_BrowserStop:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/stop-24.png"));
    case SP_MediaPlay:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-play-32.png"));
    case SP_MediaPause:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-pause-32.png"));
    case SP_MediaStop:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-stop-32.png"));
    case SP_MediaSeekForward:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-seek-forward-32.png"));
    case SP_MediaSeekBackward:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-seek-backward-32.png"));
    case SP_MediaSkipForward:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-skip-forward-32.png"));
    case SP_MediaSkipBackward:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-skip-backward-32.png"));
    case SP_MediaVolume:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-volume-16.png"));
    case SP_MediaVolumeMuted:
        return QPixmap(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-volume-muted-16.png"));
    case SP_LineEditClearButton:
        return QPixmap(clearText16IconPath());
#endif // QT_NO_IMAGEFORMAT_PNG
    default:
        break;
    }

#ifndef QT_NO_IMAGEFORMAT_XPM
    switch (sp) {
    case SP_TitleBarMenuButton:
        return titleBarMenuCachedPixmapFromXPM();
    case SP_TitleBarShadeButton:
        return cachedPixmapFromXPM(qt_shade_xpm);
    case SP_TitleBarUnshadeButton:
        return cachedPixmapFromXPM(qt_unshade_xpm);
    case SP_TitleBarNormalButton:
        return cachedPixmapFromXPM(qt_normalizeup_xpm);
    case SP_TitleBarMinButton:
        return cachedPixmapFromXPM(qt_minimize_xpm);
    case SP_TitleBarMaxButton:
        return cachedPixmapFromXPM(qt_maximize_xpm);
    case SP_TitleBarCloseButton:
        return cachedPixmapFromXPM(qt_close_xpm);
    case SP_TitleBarContextHelpButton:
        return cachedPixmapFromXPM(qt_help_xpm);
    case SP_DockWidgetCloseButton:
        return cachedPixmapFromXPM(dock_widget_close_xpm);
    case SP_MessageBoxInformation:
        return cachedPixmapFromXPM(information_xpm);
    case SP_MessageBoxWarning:
        return cachedPixmapFromXPM(warning_xpm);
    case SP_MessageBoxCritical:
        return cachedPixmapFromXPM(critical_xpm);
    case SP_MessageBoxQuestion:
        return cachedPixmapFromXPM(question_xpm);
    default:
        break;
    }
#endif //QT_NO_IMAGEFORMAT_XPM

#if !QT_CONFIG(imageformat_png) && !QT_CONFIG(imageformat_xpm) && !QT_CONFIG(imageformat_png)
    Q_UNUSED(rtl);
#endif

    return QPixmap();
}

#if QT_CONFIG(imageformat_png)
static inline QString iconResourcePrefix() { return QStringLiteral(":/qt-project.org/styles/commonstyle/images/"); }
static inline QString iconPngSuffix() { return QStringLiteral(".png"); }

static void addIconFiles(const QString &prefix, const int sizes[], size_t count, QIcon &icon)
{
    for (size_t i = 0; i < count; ++i)
        icon.addFile(prefix + QString::number(sizes[i]) + iconPngSuffix());
}

static const int dockTitleIconSizes[] = {10, 16, 20, 32, 48, 64};
static const int titleBarSizes[] = {16, 32, 48};
static const int toolBarExtHSizes[] = {8, 16, 32};
static const int toolBarExtVSizes[] = {5, 10, 20};
#endif // imageformat_png

/*!
    \internal
*/
QIcon QCommonStyle::standardIcon(StandardPixmap standardIcon, const QStyleOption *option) const
{
    QIcon icon;
    const bool rtl = (option && option->direction == Qt::RightToLeft) || (!option && QGuiApplication::isRightToLeft());

#ifdef Q_OS_WIN
    switch (standardIcon) {
    case SP_DriveCDIcon:
    case SP_DriveDVDIcon:
    case SP_DriveNetIcon:
    case SP_DriveHDIcon:
    case SP_DriveFDIcon:
    case SP_FileIcon:
    case SP_FileLinkIcon:
    case SP_DesktopIcon:
    case SP_ComputerIcon:
    case SP_VistaShield:
    case SP_MessageBoxInformation:
    case SP_MessageBoxWarning:
    case SP_MessageBoxCritical:
    case SP_MessageBoxQuestion:
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme()) {
            QPlatformTheme::StandardPixmap sp = static_cast<QPlatformTheme::StandardPixmap>(standardIcon);
            for (int size = 16 ; size <= 32 ; size += 16) {
                QPixmap pixmap = theme->standardPixmap(sp, QSizeF(size, size));
                icon.addPixmap(pixmap, QIcon::Normal);
            }
        }
        break;
    case SP_DirIcon:
    case SP_DirLinkIcon:
        if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme()) {
            QPlatformTheme::StandardPixmap spOff = static_cast<QPlatformTheme::StandardPixmap>(standardIcon);
            QPlatformTheme::StandardPixmap spOn = standardIcon == SP_DirIcon ? QPlatformTheme::DirOpenIcon :
                                                                                 QPlatformTheme::DirLinkOpenIcon;
            for (int size = 16 ; size <= 32 ; size += 16) {
                QSizeF pixSize(size, size);
                QPixmap pixmap = theme->standardPixmap(spOff, pixSize);
                icon.addPixmap(pixmap, QIcon::Normal, QIcon::Off);
                pixmap = theme->standardPixmap(spOn, pixSize);
                icon.addPixmap(pixmap, QIcon::Normal, QIcon::On);
            }
        }
        break;
    case SP_LineEditClearButton:
        icon = clearTextIcon(rtl);
        break;
    default:
        break;
    }
    if (!icon.isNull())
        return icon;

#endif

    if (QGuiApplication::desktopSettingsAware() && !QIcon::themeName().isEmpty()) {
        switch (standardIcon) {
        case SP_DirHomeIcon:
                icon = QIcon::fromTheme(QLatin1String("user-home"));
                break;
        case SP_MessageBoxInformation:
                icon = QIcon::fromTheme(QLatin1String("dialog-information"));
                break;
        case SP_MessageBoxWarning:
                icon = QIcon::fromTheme(QLatin1String("dialog-warning"));
                break;
        case SP_MessageBoxCritical:
                icon = QIcon::fromTheme(QLatin1String("dialog-error"));
                break;
        case SP_MessageBoxQuestion:
                icon = QIcon::fromTheme(QLatin1String("dialog-question"));
                break;
        case SP_DialogOpenButton:
        case SP_DirOpenIcon:
                icon = QIcon::fromTheme(QLatin1String("folder-open"));
                break;
        case SP_DialogSaveButton:
                icon = QIcon::fromTheme(QLatin1String("document-save"));
                break;
        case SP_DialogApplyButton:
                icon = QIcon::fromTheme(QLatin1String("dialog-ok-apply"));
                break;
        case SP_DialogYesButton:
        case SP_DialogOkButton:
                icon = QIcon::fromTheme(QLatin1String("dialog-ok"));
                break;
        case SP_DialogDiscardButton:
                icon = QIcon::fromTheme(QLatin1String("edit-delete"));
                break;
        case SP_DialogResetButton:
                icon = QIcon::fromTheme(QLatin1String("edit-clear"));
                break;
        case SP_DialogHelpButton:
                icon = QIcon::fromTheme(QLatin1String("help-contents"));
                break;
        case SP_FileIcon:
                icon = QIcon::fromTheme(QLatin1String("text-x-generic"));
                break;
        case SP_DirClosedIcon:
        case SP_DirIcon:
                icon = QIcon::fromTheme(QLatin1String("folder"));
                break;
        case SP_DriveFDIcon:
                icon = QIcon::fromTheme(QLatin1String("floppy_unmount"));
                break;
        case SP_ComputerIcon:
                icon = QIcon::fromTheme(QLatin1String("computer"),
                                        QIcon::fromTheme(QLatin1String("system")));
                break;
        case SP_DesktopIcon:
                icon = QIcon::fromTheme(QLatin1String("user-desktop"));
                break;
        case SP_TrashIcon:
                icon = QIcon::fromTheme(QLatin1String("user-trash"));
                break;
        case SP_DriveCDIcon:
        case SP_DriveDVDIcon:
                icon = QIcon::fromTheme(QLatin1String("media-optical"));
                break;
        case SP_DriveHDIcon:
                icon = QIcon::fromTheme(QLatin1String("drive-harddisk"));
                break;
        case SP_FileDialogToParent:
                icon = QIcon::fromTheme(QLatin1String("go-up"));
                break;
        case SP_FileDialogNewFolder:
                icon = QIcon::fromTheme(QLatin1String("folder-new"));
                break;
        case SP_ArrowUp:
                icon = QIcon::fromTheme(QLatin1String("go-up"));
                break;
        case SP_ArrowDown:
                icon = QIcon::fromTheme(QLatin1String("go-down"));
                break;
        case SP_ArrowRight:
                icon = QIcon::fromTheme(QLatin1String("go-next"));
                break;
        case SP_ArrowLeft:
                icon = QIcon::fromTheme(QLatin1String("go-previous"));
                break;
        case SP_DialogNoButton:
        case SP_DialogCancelButton:
                icon = QIcon::fromTheme(QLatin1String("dialog-cancel"),
                                        QIcon::fromTheme(QLatin1String("process-stop")));
                break;
        case SP_DialogCloseButton:
                icon = QIcon::fromTheme(QLatin1String("window-close"));
                break;
        case SP_FileDialogDetailedView:
                icon = QIcon::fromTheme(QLatin1String("view-list-details"));
                break;
        case SP_FileDialogListView:
                icon = QIcon::fromTheme(QLatin1String("view-list-icons"));
                break;
        case SP_BrowserReload:
                icon = QIcon::fromTheme(QLatin1String("view-refresh"));
                break;
        case SP_BrowserStop:
                icon = QIcon::fromTheme(QLatin1String("process-stop"));
                break;
        case SP_MediaPlay:
                icon = QIcon::fromTheme(QLatin1String("media-playback-start"));
                break;
        case SP_MediaPause:
                icon = QIcon::fromTheme(QLatin1String("media-playback-pause"));
                break;
        case SP_MediaStop:
                icon = QIcon::fromTheme(QLatin1String("media-playback-stop"));
                break;
        case SP_MediaSeekForward:
                icon = QIcon::fromTheme(QLatin1String("media-seek-forward"));
                break;
        case SP_MediaSeekBackward:
                icon = QIcon::fromTheme(QLatin1String("media-seek-backward"));
                break;
        case SP_MediaSkipForward:
                icon = QIcon::fromTheme(QLatin1String("media-skip-forward"));
                break;
        case SP_MediaSkipBackward:
                icon = QIcon::fromTheme(QLatin1String("media-skip-backward"));
                break;
        case SP_MediaVolume:
                icon = QIcon::fromTheme(QLatin1String("audio-volume-medium"));
                break;
        case SP_MediaVolumeMuted:
                icon = QIcon::fromTheme(QLatin1String("audio-volume-muted"));
                break;
        case SP_ArrowForward:
            if (rtl)
                return QCommonStyle::standardIcon(SP_ArrowLeft, option);
            return QCommonStyle::standardIcon(SP_ArrowRight, option);
        case SP_ArrowBack:
            if (rtl)
                return QCommonStyle::standardIcon(SP_ArrowRight, option);
            return QCommonStyle::standardIcon(SP_ArrowLeft, option);
        case SP_FileLinkIcon:
            {
                QIcon linkIcon = QIcon::fromTheme(QLatin1String("emblem-symbolic-link"));
                if (!linkIcon.isNull()) {
                    QIcon baseIcon = QCommonStyle::standardIcon(SP_FileIcon, option);
                    const QList<QSize> sizes = baseIcon.availableSizes(QIcon::Normal, QIcon::Off);
                    for (int i = 0 ; i < sizes.size() ; ++i) {
                        int size = sizes[i].width();
                        QPixmap basePixmap = baseIcon.pixmap(option->window, QSize(size, size));
                        QPixmap linkPixmap = linkIcon.pixmap(option->window, QSize(size / 2, size / 2));
                        QPainter painter(&basePixmap);
                        painter.drawPixmap(size/2, size/2, linkPixmap);
                        icon.addPixmap(basePixmap);
                    }
                }
            }
            break;
        case SP_DirLinkIcon:
            {
                QIcon linkIcon = QIcon::fromTheme(QLatin1String("emblem-symbolic-link"));
                if (!linkIcon.isNull()) {
                    QIcon baseIcon = QCommonStyle::standardIcon(SP_DirIcon, option);
                    const QList<QSize> sizes = baseIcon.availableSizes(QIcon::Normal, QIcon::Off);
                    for (int i = 0 ; i < sizes.size() ; ++i) {
                        int size = sizes[i].width();
                        QPixmap basePixmap = baseIcon.pixmap(option->window, QSize(size, size));
                        QPixmap linkPixmap = linkIcon.pixmap(option->window, QSize(size / 2, size / 2));
                        QPainter painter(&basePixmap);
                        painter.drawPixmap(size/2, size/2, linkPixmap);
                        icon.addPixmap(basePixmap);
                    }
                }
        }
        break;
        default:
            break;
        }
    } // if (QGuiApplication::desktopSettingsAware() && !QIcon::themeName().isEmpty())

    if (!icon.isNull())
        return icon;

#if defined(Q_OS_MAC)
    if (QGuiApplication::desktopSettingsAware()) {
        switch (standardIcon) {
        case SP_DirIcon: {
            // A rather special case
            QIcon closeIcon = QCommonStyle::standardIcon(SP_DirClosedIcon, option);
            QIcon openIcon = QCommonStyle::standardIcon(SP_DirOpenIcon, option);
            closeIcon.addPixmap(openIcon.pixmap(16, 16), QIcon::Normal, QIcon::On);
            closeIcon.addPixmap(openIcon.pixmap(32, 32), QIcon::Normal, QIcon::On);
            closeIcon.addPixmap(openIcon.pixmap(64, 64), QIcon::Normal, QIcon::On);
            closeIcon.addPixmap(openIcon.pixmap(128, 128), QIcon::Normal, QIcon::On);
            return closeIcon;
        }

        case SP_TitleBarNormalButton:
        case SP_TitleBarCloseButton: {
            QIcon titleBarIcon;
            if (standardIcon == SP_TitleBarCloseButton) {
                titleBarIcon.addFile(QLatin1String(":/qt-project.org/styles/macstyle/images/closedock-16.png"));
                titleBarIcon.addFile(QLatin1String(":/qt-project.org/styles/macstyle/images/closedock-down-16.png"), QSize(16, 16), QIcon::Normal, QIcon::On);
            } else {
                titleBarIcon.addFile(QLatin1String(":/qt-project.org/styles/macstyle/images/dockdock-16.png"));
                titleBarIcon.addFile(QLatin1String(":/qt-project.org/styles/macstyle/images/dockdock-down-16.png"), QSize(16, 16), QIcon::Normal, QIcon::On);
            }
            return titleBarIcon;
        }

        case SP_MessageBoxQuestion:
        case SP_MessageBoxInformation:
        case SP_MessageBoxWarning:
        case SP_MessageBoxCritical:
        case SP_DesktopIcon:
        case SP_TrashIcon:
        case SP_ComputerIcon:
        case SP_DriveFDIcon:
        case SP_DriveHDIcon:
        case SP_DriveCDIcon:
        case SP_DriveDVDIcon:
        case SP_DriveNetIcon:
        case SP_DirOpenIcon:
        case SP_DirClosedIcon:
        case SP_DirLinkIcon:
        case SP_FileLinkIcon:
        case SP_FileIcon:
            if (const QPlatformTheme *theme = QGuiApplicationPrivate::platformTheme()) {
                QPlatformTheme::StandardPixmap sp = static_cast<QPlatformTheme::StandardPixmap>(standardIcon);
                QIcon retIcon;
                const QList<QSize> sizes = theme->themeHint(QPlatformTheme::IconPixmapSizes).value<QList<QSize> >();
                for (const QSize &size : sizes) {
                    QPixmap mainIcon;
                    const QString cacheKey = QLatin1String("qt_mac_constructQIconFromIconRef") + QString::number(standardIcon) + QString::number(size.width());
                    if (standardIcon >= QStyle::SP_CustomBase) {
                        mainIcon = theme->standardPixmap(sp, QSizeF(size));
                    } else if (QPixmapCache::find(cacheKey, &mainIcon) == false) {
                        mainIcon = theme->standardPixmap(sp, QSizeF(size));
                        QPixmapCache::insert(cacheKey, mainIcon);
                    }

                    retIcon.addPixmap(mainIcon);
                }
                if (!retIcon.isNull())
                    return retIcon;
            }

        default:
            break;
        }
    } // if (QGuiApplication::desktopSettingsAware())
#endif // Q_OS_MAC

    switch (standardIcon) {
#ifndef QT_NO_IMAGEFORMAT_PNG
    case SP_TitleBarMinButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("titlebar-min-"),
                     titleBarSizes, sizeof(titleBarSizes)/sizeof(titleBarSizes[0]), icon);
        break;
    case SP_TitleBarMaxButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("titlebar-max-"),
                     titleBarSizes, sizeof(titleBarSizes)/sizeof(titleBarSizes[0]), icon);
        break;
    case SP_TitleBarShadeButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("titlebar-shade-"),
                     titleBarSizes, sizeof(titleBarSizes)/sizeof(titleBarSizes[0]), icon);

        break;
    case SP_TitleBarUnshadeButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("titlebar-unshade-"),
                     titleBarSizes, sizeof(titleBarSizes)/sizeof(titleBarSizes[0]), icon);
        break;
    case SP_TitleBarContextHelpButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("titlebar-contexthelp-"),
                     titleBarSizes, sizeof(titleBarSizes)/sizeof(titleBarSizes[0]), icon);
        break;
     case SP_FileDialogNewFolder:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/newdirectory-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/newdirectory-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/newdirectory-128.png"), QSize(128, 128));
        break;
    case SP_FileDialogBack:
        return QCommonStyle::standardIcon(SP_ArrowBack, option);
    case SP_FileDialogToParent:
        return QCommonStyle::standardIcon(SP_ArrowUp, option);
    case SP_FileDialogDetailedView:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewdetailed-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewdetailed-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewdetailed-128.png"), QSize(128, 128));
        break;
    case SP_FileDialogInfoView:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/fileinfo-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/fileinfo-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/fileinfo-128.png"), QSize(128, 128));
        break;
    case SP_FileDialogContentsView:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/filecontents-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/filecontents-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/filecontents-128.png"), QSize(128, 128));
        break;
    case SP_FileDialogListView:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewlist-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewlist-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/viewlist-128.png"), QSize(128, 128));
        break;
    case SP_DialogOkButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-ok-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-ok-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-ok-128.png"), QSize(128, 128));
        break;
    case SP_DialogCancelButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-128.png"), QSize(128, 128));
        break;
    case SP_DialogHelpButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-help-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-help-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-help-128.png"), QSize(128, 128));
        break;
    case SP_DialogOpenButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-open-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-open-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-open-128.png"), QSize(128, 128));
        break;
    case SP_DialogSaveButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-save-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-save-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-save-128.png"), QSize(128, 128));
        break;
    case SP_DialogCloseButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-close-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-close-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-close-128.png"), QSize(128, 128));
        break;
    case SP_DialogApplyButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-apply-128.png"), QSize(128, 128));
        break;
    case SP_DialogResetButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-clear-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-clear-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-clear-128.png"), QSize(128, 128));
        break;
    case SP_DialogDiscardButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-delete-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-delete-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-delete-128.png"), QSize(128, 128));
        break;
    case SP_DialogYesButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-yes-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-yes-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-yes-128.png"), QSize(128, 128));
        break;
    case SP_DialogNoButton:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-no-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-no-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/standardbutton-no-128.png"), QSize(128, 128));
        break;
    case SP_ArrowForward:
        if (rtl)
            return QCommonStyle::standardIcon(SP_ArrowLeft, option);
        return QCommonStyle::standardIcon(SP_ArrowRight, option);
    case SP_ArrowBack:
        if (rtl)
            return QCommonStyle::standardIcon(SP_ArrowRight, option);
        return QCommonStyle::standardIcon(SP_ArrowLeft, option);
    case SP_ArrowLeft:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/left-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/left-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/left-128.png"), QSize(128, 128));
        break;
    case SP_ArrowRight:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/right-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/right-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/right-128.png"), QSize(128, 128));
        break;
    case SP_ArrowUp:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/up-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/up-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/up-128.png"), QSize(128, 128));
        break;
    case SP_ArrowDown:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/down-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/down-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/down-128.png"), QSize(128, 128));
        break;
   case SP_DirHomeIcon:
   case SP_DirIcon:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/dirclosed-16.png"),
                     QSize(), QIcon::Normal, QIcon::Off);
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/diropen-16.png"),
                     QSize(), QIcon::Normal, QIcon::On);
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/dirclosed-32.png"),
                     QSize(32, 32), QIcon::Normal, QIcon::Off);
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/diropen-32.png"),
                     QSize(32, 32), QIcon::Normal, QIcon::On);
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/dirclosed-128.png"),
                     QSize(128, 128), QIcon::Normal, QIcon::Off);
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/diropen-128.png"),
                     QSize(128, 128), QIcon::Normal, QIcon::On);
        break;
    case SP_DriveCDIcon:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/cdr-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/cdr-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/cdr-128.png"), QSize(128, 128));
        break;
    case SP_DriveDVDIcon:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/dvd-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/dvd-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/dvd-128.png"), QSize(128, 128));
        break;
    case SP_FileIcon:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/file-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/file-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/file-128.png"), QSize(128, 128));
        break;
    case SP_FileLinkIcon:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/filelink-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/filelink-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/filelink-128.png"), QSize(128, 128));
        break;
    case SP_TrashIcon:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/trash-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/trash-32.png"), QSize(32, 32));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/trash-128.png"), QSize(128, 128));
        break;
    case SP_BrowserReload:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/refresh-24.png"), QSize(24, 24));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/refresh-32.png"), QSize(32, 32));
        break;
    case SP_BrowserStop:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/stop-24.png"), QSize(24, 24));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/stop-32.png"), QSize(32, 32));
        break;
    case SP_MediaPlay:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-play-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-play-32.png"), QSize(32, 32));
        break;
    case SP_MediaPause:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-pause-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-pause-32.png"), QSize(32, 32));
        break;
    case SP_MediaStop:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-stop-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-stop-32.png"), QSize(32, 32));
        break;
    case SP_MediaSeekForward:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-seek-forward-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-seek-forward-32.png"), QSize(32, 32));
        break;
    case SP_MediaSeekBackward:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-seek-backward-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-seek-backward-32.png"), QSize(32, 32));
        break;
    case SP_MediaSkipForward:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-skip-forward-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-skip-forward-32.png"), QSize(32, 32));
        break;
    case SP_MediaSkipBackward:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-skip-backward-16.png"), QSize(16, 16));
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-skip-backward-32.png"), QSize(32, 32));
        break;
    case SP_MediaVolume:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-volume-16.png"), QSize(16, 16));
        break;
    case SP_MediaVolumeMuted:
        icon.addFile(QLatin1String(":/qt-project.org/styles/commonstyle/images/media-volume-muted-16.png"), QSize(16, 16));
        break;
    case SP_TitleBarCloseButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("closedock-"),
                     dockTitleIconSizes, sizeof(dockTitleIconSizes)/sizeof(dockTitleIconSizes[0]), icon);
        break;
    case SP_TitleBarMenuButton:
#  ifndef QT_NO_IMAGEFORMAT_XPM
        icon.addPixmap(titleBarMenuCachedPixmapFromXPM());
#  endif
        icon.addFile(QLatin1String(":/qt-project.org/qmessagebox/images/qtlogo-64.png"));
        break;
    case SP_TitleBarNormalButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("normalizedockup-"),
                     dockTitleIconSizes, sizeof(dockTitleIconSizes)/sizeof(dockTitleIconSizes[0]), icon);
        break;
    case SP_ToolBarHorizontalExtensionButton: {
        QString prefix = iconResourcePrefix() + QStringLiteral("toolbar-ext-h-");
        if (rtl)
            prefix += QStringLiteral("rtl-");
        addIconFiles(prefix, toolBarExtHSizes, sizeof(toolBarExtHSizes)/sizeof(toolBarExtHSizes[0]), icon);
    }
        break;
    case SP_ToolBarVerticalExtensionButton:
        addIconFiles(iconResourcePrefix() + QStringLiteral("toolbar-ext-v-"),
                     toolBarExtVSizes, sizeof(toolBarExtVSizes)/sizeof(toolBarExtVSizes[0]), icon);
        break;
#endif // QT_NO_IMAGEFORMAT_PNG
    default:
        icon.addPixmap(proxy()->standardPixmap(standardIcon, option));
        break;
    }
    return icon;
}

static inline uint qt_intensity(uint r, uint g, uint b)
{
    // 30% red, 59% green, 11% blue
    return (77 * r + 150 * g + 28 * b) / 255;
}

/*! \reimp */
QPixmap QCommonStyle::generatedIconPixmap(QIcon::Mode iconMode, const QPixmap &pixmap,
                                          const QStyleOption *opt) const
{
    switch (iconMode) {
    case QIcon::Disabled: {
        QImage im = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);

        // Create a colortable based on the background (black -> bg -> white)
        QColor bg = opt->palette.color(QPalette::Disabled, QPalette::Window);
        int red = bg.red();
        int green = bg.green();
        int blue = bg.blue();
        uchar reds[256], greens[256], blues[256];
        for (int i=0; i<128; ++i) {
            reds[i]   = uchar((red   * (i<<1)) >> 8);
            greens[i] = uchar((green * (i<<1)) >> 8);
            blues[i]  = uchar((blue  * (i<<1)) >> 8);
        }
        for (int i=0; i<128; ++i) {
            reds[i+128]   = uchar(qMin(red   + (i << 1), 255));
            greens[i+128] = uchar(qMin(green + (i << 1), 255));
            blues[i+128]  = uchar(qMin(blue  + (i << 1), 255));
        }

        int intensity = qt_intensity(red, green, blue);
        const int factor = 191;

        // High intensity colors needs dark shifting in the color table, while
        // low intensity colors needs light shifting. This is to increase the
        // perceived contrast.
        if ((red - factor > green && red - factor > blue)
            || (green - factor > red && green - factor > blue)
            || (blue - factor > red && blue - factor > green))
            intensity = qMin(255, intensity + 91);
        else if (intensity <= 128)
            intensity -= 51;

        for (int y=0; y<im.height(); ++y) {
            QRgb *scanLine = (QRgb*)im.scanLine(y);
            for (int x=0; x<im.width(); ++x) {
                QRgb pixel = *scanLine;
                // Calculate color table index, taking intensity adjustment
                // and a magic offset into account.
                uint ci = uint(qGray(pixel)/3 + (130 - intensity / 3));
                *scanLine = qRgba(reds[ci], greens[ci], blues[ci], qAlpha(pixel));
                ++scanLine;
            }
        }

        return QPixmap::fromImage(im);
    }
    case QIcon::Selected: {
        QImage img = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
        QColor color = opt->palette.color(QPalette::Normal, QPalette::Highlight);
        color.setAlphaF(qreal(0.3));
        QPainter painter(&img);
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(0, 0, img.width(), img.height(), color);
        painter.end();
        return QPixmap::fromImage(img); }
    case QIcon::Active:
        return pixmap;
    default:
        break;
    }
    return pixmap;
}

} // namespace QQC2

QT_END_NAMESPACE

#include "moc_qquickcommonstyle.cpp"

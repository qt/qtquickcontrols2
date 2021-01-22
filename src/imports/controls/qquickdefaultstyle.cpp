/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Controls 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#include "qquickdefaultstyle_p.h"

QT_BEGIN_NAMESPACE

QQuickDefaultStyle::QQuickDefaultStyle(QObject *parent) :
    QObject(parent)
{
}

QColor QQuickDefaultStyle::backgroundColor() const
{
    return QColor::fromRgba(0xFFFFFFFF);
}

QColor QQuickDefaultStyle::overlayModalColor() const
{
    return QColor::fromRgba(0x7F28282A);
}

QColor QQuickDefaultStyle::overlayDimColor() const
{
    return QColor::fromRgba(0x1F28282A);
}

QColor QQuickDefaultStyle::textColor() const
{
    return QColor::fromRgba(0xFF353637);
}

QColor QQuickDefaultStyle::textDarkColor() const
{
    return QColor::fromRgba(0xFF26282A);
}

QColor QQuickDefaultStyle::textLightColor() const
{
    return QColor::fromRgba(0xFFFFFFFF);
}

QColor QQuickDefaultStyle::textLinkColor() const
{
    return QColor::fromRgba(0xFF45A7D7);
}

QColor QQuickDefaultStyle::textSelectionColor() const
{
    return QColor::fromRgba(0xFFFDDD5C);
}

QColor QQuickDefaultStyle::textDisabledColor() const
{
    return QColor::fromRgba(0xFFBDBEBF);
}

QColor QQuickDefaultStyle::textDisabledLightColor() const
{
    return QColor::fromRgba(0xFFC2C2C2);
}

QColor QQuickDefaultStyle::textPlaceholderColor() const
{
    return QColor::fromRgba(0xFF777777);
}

QColor QQuickDefaultStyle::focusColor() const
{
    return QColor::fromRgba(0xFF0066FF);
}

QColor QQuickDefaultStyle::focusLightColor() const
{
    return QColor::fromRgba(0xFFF0F6FF);
}

QColor QQuickDefaultStyle::focusPressedColor() const
{
    return QColor::fromRgba(0xFFCCE0FF);
}

QColor QQuickDefaultStyle::buttonColor() const
{
    return QColor::fromRgba(0xFFE0E0E0);
}

QColor QQuickDefaultStyle::buttonPressedColor() const
{
    return QColor::fromRgba(0xFFD0D0D0);
}

QColor QQuickDefaultStyle::buttonCheckedColor() const
{
    return QColor::fromRgba(0xFF353637);
}

QColor QQuickDefaultStyle::buttonCheckedPressedColor() const
{
    return QColor::fromRgba(0xFF585A5C);
}

QColor QQuickDefaultStyle::buttonCheckedFocusColor() const
{
    return QColor::fromRgba(0xFF599BFF);
}

QColor QQuickDefaultStyle::toolButtonColor() const
{
    return QColor::fromRgba(0x33333333);
}

QColor QQuickDefaultStyle::tabButtonColor() const
{
    return QColor::fromRgba(0xFF353637);
}

QColor QQuickDefaultStyle::tabButtonPressedColor() const
{
    return QColor::fromRgba(0xFF585A5C);
}

QColor QQuickDefaultStyle::tabButtonCheckedPressedColor() const
{
    return QColor::fromRgba(0xFFE4E4E4);
}

QColor QQuickDefaultStyle::delegateColor() const
{
    return QColor::fromRgba(0xFFEEEEEE);
}

QColor QQuickDefaultStyle::delegatePressedColor() const
{
    return QColor::fromRgba(0xFFBDBEBF);
}

QColor QQuickDefaultStyle::delegateFocusColor() const
{
    return QColor::fromRgba(0xFFE5EFFF);
}

QColor QQuickDefaultStyle::indicatorPressedColor() const
{
    return QColor::fromRgba(0xFFF6F6F6);
}

QColor QQuickDefaultStyle::indicatorDisabledColor() const
{
    return QColor::fromRgba(0xFFFDFDFD);
}

QColor QQuickDefaultStyle::indicatorFrameColor() const
{
    return QColor::fromRgba(0xFF909090);
}

QColor QQuickDefaultStyle::indicatorFramePressedColor() const
{
    return QColor::fromRgba(0xFF808080);
}

QColor QQuickDefaultStyle::indicatorFrameDisabledColor() const
{
    return QColor::fromRgba(0xFFD6D6D6);
}

QColor QQuickDefaultStyle::frameDarkColor() const
{
    return QColor::fromRgba(0xFF353637);
}

QColor QQuickDefaultStyle::frameLightColor() const
{
    return QColor::fromRgba(0xFFBDBEBF);
}

QColor QQuickDefaultStyle::scrollBarColor() const
{
    return QColor::fromRgba(0xFFBDBEBF);
}

QColor QQuickDefaultStyle::scrollBarPressedColor() const
{
    return QColor::fromRgba(0xFF28282A);
}

QColor QQuickDefaultStyle::progressBarColor() const
{
    return QColor::fromRgba(0xFFE4E4E4);
}

QColor QQuickDefaultStyle::pageIndicatorColor() const
{
    return QColor::fromRgba(0xFF28282A);
}

QColor QQuickDefaultStyle::separatorColor() const
{
    return QColor::fromRgba(0xFFCCCCCC);
}

QColor QQuickDefaultStyle::disabledDarkColor() const
{
    return QColor::fromRgba(0xFF353637);
}

QColor QQuickDefaultStyle::disabledLightColor() const
{
    return QColor::fromRgba(0xFFBDBEBF);
}

QT_END_NAMESPACE

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

#ifndef QQUICKFUSIONSTYLE_P_H
#define QQUICKFUSIONSTYLE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>

QT_BEGIN_NAMESPACE

class QQuickFusionStyle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor lightShade READ lightShade CONSTANT)
    Q_PROPERTY(QColor darkShade READ darkShade CONSTANT)
    Q_PROPERTY(QColor topShadow READ topShadow CONSTANT)
    Q_PROPERTY(QColor innerContrastLine READ innerContrastLine CONSTANT)

public:
    explicit QQuickFusionStyle(QObject *parent = nullptr);

    static QColor lightShade();
    static QColor darkShade();
    static QColor topShadow();
    static QColor innerContrastLine();

    Q_INVOKABLE static QColor highlight(const QPalette &palette);
    Q_INVOKABLE static QColor highlightedText(const QPalette &palette);
    Q_INVOKABLE static QColor outline(const QPalette &palette);
    Q_INVOKABLE static QColor highlightedOutline(const QPalette &palette);
    Q_INVOKABLE static QColor tabFrameColor(const QPalette &palette);
    Q_INVOKABLE static QColor buttonColor(const QPalette &palette, bool highlighted = false, bool down = false, bool hovered = false);
    Q_INVOKABLE static QColor buttonOutline(const QPalette &palette, bool highlighted = false, bool enabled = true);
    Q_INVOKABLE static QColor gradientStart(const QColor &baseColor);
    Q_INVOKABLE static QColor gradientStop(const QColor &baseColor);
    Q_INVOKABLE static QColor mergedColors(const QColor &colorA, const QColor &colorB, int factor = 50);
    Q_INVOKABLE static QColor grooveColor(const QPalette &palette);
};

QT_END_NAMESPACE

#endif // QQUICKFUSIONSTYLE_P_H

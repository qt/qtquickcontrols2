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

import QtQuick 2.13
import QtQuick.Templates 2.13 as T
import QtQuick.Controls.Imagine 2.13
import QtQuick.Controls.Imagine.impl 2.13

T.SplitView {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    handle: NinePatchImage {
        source: Imagine.url + "splitview-handle"
        NinePatchImageSelector on source {
            states: [
                {"vertical": control.orientation === Qt.Vertical},
                {"horizontal":control.orientation === Qt.Horizontal},
                {"disabled": !control.enabled},
                {"pressed": T.SplitHandle.pressed},
                {"mirrored": control.mirrored},
                {"hovered": T.SplitHandle.hovered}
            ]
        }
    }
}

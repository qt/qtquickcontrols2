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

import QtQuick 2.12
import QtQuick.Templates 2.12 as T
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Controls.Fusion 2.12
import QtQuick.Controls.Fusion.impl 2.12

T.Dial {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding) || 100 // ### remove 100 in Qt 6
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding) || 100 // ### remove 100 in Qt 6

    background: DialImpl {
        implicitWidth: 100
        implicitHeight: 100
        palette: control.palette
        highlight: control.visualFocus
    }

    handle: KnobImpl {
        x: control.background.x + control.background.width / 2 - control.handle.width / 2
        y: control.background.y + control.background.height / 2 - control.handle.height / 2
        width: control.width / 7
        height: control.height / 7
        palette: control.palette
        transform: [
            Translate {
                y: -Math.min(control.background.width, control.background.height) * 0.42 + control.handle.height
            },
            Rotation {
                angle: control.angle
                origin.x: control.handle.width / 2
                origin.y: control.handle.height / 2
            }
        ]
    }
}

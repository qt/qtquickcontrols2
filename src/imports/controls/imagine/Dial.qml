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
import QtQuick.Controls.Imagine 2.12
import QtQuick.Controls.Imagine.impl 2.12

T.Dial {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                           (handle ? handle.implicitWidth : 0) + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                            (handle ? handle.implicitHeight : 0) + topPadding + bottomPadding)

    topPadding: background ? background.topPadding : 0
    leftPadding: background ? background.leftPadding : 0
    rightPadding: background ? background.rightPadding : 0
    bottomPadding: background ? background.bottomPadding : 0

    topInset: background ? -background.topInset || 0 : 0
    leftInset: background ? -background.leftInset || 0 : 0
    rightInset: background ? -background.rightInset || 0 : 0
    bottomInset: background ? -background.bottomInset || 0 : 0

    handle: Image {
        x: background.x + background.width / 2 - handle.width / 2
        y: background.y + background.height / 2 - handle.height / 2

        source: Imagine.url + "dial-handle"
        ImageSelector on source {
            states: [
                {"disabled": !control.enabled},
                {"pressed": control.pressed},
                {"focused": control.visualFocus},
                {"mirrored": control.mirrored},
                {"hovered": control.hovered}
            ]
        }

        transform: [
            Translate {
                y: -Math.min(control.background.width, control.background.height) * 0.4 + control.handle.height / 2
            },
            Rotation {
                angle: control.angle
                origin.x: handle.width / 2
                origin.y: handle.height / 2
            }
        ]
    }

    background: NinePatchImage {
        x: control.width / 2 - width / 2
        y: control.height / 2 - height / 2
        width: Math.max(64, Math.min(control.width, control.height))
        height: width
        fillMode: Image.PreserveAspectFit

        source: Imagine.url + "dial-background"
        NinePatchImageSelector on source {
            states: [
                {"disabled": !control.enabled},
                {"pressed": control.pressed},
                {"focused": control.visualFocus},
                {"mirrored": control.mirrored},
                {"hovered": control.hovered}
            ]
        }
    }
}

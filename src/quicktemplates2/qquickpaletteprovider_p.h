/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
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

#ifndef QQUICKPALETTEPROVIDER_P_H
#define QQUICKPALETTEPROVIDER_P_H

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

#include <QtQuickTemplates2/private/qtquicktemplates2global_p.h>
#include <QtQml/private/qqmlglobal_p.h>

QT_BEGIN_NAMESPACE

class Q_QUICKTEMPLATES2_PRIVATE_EXPORT QQuickPaletteProvider : public QQmlValueTypeProvider
{
public:
    static void init();
    static void cleanup();

    const QMetaObject *getMetaObjectForMetaType(int type) override;
    bool init(int type, QVariant& dst) override;
    bool equal(int type, const void *lhs, const QVariant &rhs) override;
    bool store(int type, const void *src, void *dst, size_t dstSize) override;
    bool read(const QVariant &src, void *dst, int dstType) override;
    bool write(int type, const void *src, QVariant& dst) override;
};

QT_END_NAMESPACE

#endif // QQUICKTEMPLATES2VALUETYPEPROVIDER_P_H

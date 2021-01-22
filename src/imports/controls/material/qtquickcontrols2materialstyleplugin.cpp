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

#include <QtQuickControls2/private/qquickstyleplugin_p.h>

#include "qquickmaterialstyle_p.h"
#include "qquickmaterialtheme_p.h"
#include "qquickmaterialbusyindicator_p.h"
#include "qquickmaterialprogressbar_p.h"
#include "qquickmaterialripple_p.h"

#include <QtQuickControls2/private/qquickstyleselector_p.h>
#include <QtQuickControls2/private/qquickpaddedrectangle_p.h>

QT_BEGIN_NAMESPACE

class QtQuickControls2MaterialStylePlugin : public QQuickStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QtQuickControls2MaterialStylePlugin(QObject *parent = nullptr);

    void registerTypes(const char *uri) override;

    QString name() const override;
    void initializeTheme(QQuickTheme *theme) override;
};

QtQuickControls2MaterialStylePlugin::QtQuickControls2MaterialStylePlugin(QObject *parent) : QQuickStylePlugin(parent)
{
    QQuickMaterialStyle::initGlobals();
}

void QtQuickControls2MaterialStylePlugin::registerTypes(const char *uri)
{
    qmlRegisterModule(uri, 2, QT_VERSION_MINOR); // Qt 5.12->2.12, 5.13->2.13...
    qmlRegisterUncreatableType<QQuickMaterialStyle>(uri, 2, 0, "Material", tr("Material is an attached property"));

    QByteArray import = QByteArray(uri) + ".impl";
    qmlRegisterModule(import, 2, QT_VERSION_MINOR); // Qt 5.12->2.12, 5.13->2.13...

    qmlRegisterType<QQuickMaterialBusyIndicator>(import, 2, 0, "BusyIndicatorImpl");
    qmlRegisterType<QQuickMaterialProgressBar>(import, 2, 0, "ProgressBarImpl");
    qmlRegisterType<QQuickMaterialRipple>(import, 2, 0, "Ripple");
    qmlRegisterType(resolvedUrl(QStringLiteral("BoxShadow.qml")), import, 2, 0, "BoxShadow");
    qmlRegisterType(resolvedUrl(QStringLiteral("CheckIndicator.qml")), import, 2, 0, "CheckIndicator");
    qmlRegisterType(resolvedUrl(QStringLiteral("CursorDelegate.qml")), import, 2, 0, "CursorDelegate");
    qmlRegisterType(resolvedUrl(QStringLiteral("ElevationEffect.qml")), import, 2, 0, "ElevationEffect");
    qmlRegisterType(resolvedUrl(QStringLiteral("RadioIndicator.qml")), import, 2, 0, "RadioIndicator");
    qmlRegisterType(resolvedUrl(QStringLiteral("RectangularGlow.qml")), import, 2, 0, "RectangularGlow");
    qmlRegisterType(resolvedUrl(QStringLiteral("SliderHandle.qml")), import, 2, 0, "SliderHandle");
    qmlRegisterType(resolvedUrl(QStringLiteral("SwitchIndicator.qml")), import, 2, 0, "SwitchIndicator");
}

QString QtQuickControls2MaterialStylePlugin::name() const
{
    return QStringLiteral("Material");
}

void QtQuickControls2MaterialStylePlugin::initializeTheme(QQuickTheme *theme)
{
    QQuickMaterialTheme::initialize(theme);
}

QT_END_NAMESPACE

#include "qtquickcontrols2materialstyleplugin.moc"

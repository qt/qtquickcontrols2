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

#include "qquickuniversalbusyindicator_p.h"
#include "qquickuniversalfocusrectangle_p.h"
#include "qquickuniversalprogressbar_p.h"
#include "qquickuniversalstyle_p.h"
#include "qquickuniversaltheme_p.h"

QT_BEGIN_NAMESPACE

class QtQuickControls2UniversalStylePlugin: public QQuickStylePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QtQuickControls2UniversalStylePlugin(QObject *parent = nullptr);

    void registerTypes(const char *uri) override;

    QString name() const override;
    void initializeTheme(QQuickTheme *theme) override;
};

QtQuickControls2UniversalStylePlugin::QtQuickControls2UniversalStylePlugin(QObject *parent) : QQuickStylePlugin(parent)
{
    QQuickUniversalStyle::initGlobals();
}

void QtQuickControls2UniversalStylePlugin::registerTypes(const char *uri)
{
    qmlRegisterModule(uri, 2, QT_VERSION_MINOR); // Qt 5.12->2.12, 5.13->2.13...
    qmlRegisterUncreatableType<QQuickUniversalStyle>(uri, 2, 0, "Universal", tr("Universal is an attached property"));

    QByteArray import = QByteArray(uri) + ".impl";
    qmlRegisterModule(import, 2, QT_VERSION_MINOR); // Qt 5.12->2.12, 5.13->2.13...

    qmlRegisterType<QQuickUniversalFocusRectangle>(import, 2, 0, "FocusRectangle");
    qmlRegisterType<QQuickUniversalBusyIndicator>(import, 2, 0, "BusyIndicatorImpl");
    qmlRegisterType<QQuickUniversalProgressBar>(import, 2, 0, "ProgressBarImpl");

    qmlRegisterType(resolvedUrl(QStringLiteral("CheckIndicator.qml")), import, 2, 0, "CheckIndicator");
    qmlRegisterType(resolvedUrl(QStringLiteral("RadioIndicator.qml")), import, 2, 0, "RadioIndicator");
    qmlRegisterType(resolvedUrl(QStringLiteral("SwitchIndicator.qml")), import, 2, 0, "SwitchIndicator");
}

QString QtQuickControls2UniversalStylePlugin::name() const
{
    return QStringLiteral("Universal");
}

void QtQuickControls2UniversalStylePlugin::initializeTheme(QQuickTheme *theme)
{
    QQuickUniversalTheme::initialize(theme);
}

QT_END_NAMESPACE

#include "qtquickcontrols2universalstyleplugin.moc"

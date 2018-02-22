/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QtTest/qtest.h>
#include "../shared/visualtestutil.h"

#include <QtGui/qpalette.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuickTemplates2/private/qquickapplicationwindow_p.h>
#include <QtQuickTemplates2/private/qquickcontrol_p.h>
#include <QtQuickTemplates2/private/qquickcontrol_p_p.h>
#include <QtQuickTemplates2/private/qquickpopup_p.h>
#include <QtQuickTemplates2/private/qquicktheme_p.h>

using namespace QQuickVisualTestUtil;

class tst_palette : public QQmlDataTest
{
    Q_OBJECT

private slots:
    void initTestCase();

    void palette_data();
    void palette();

    void inheritance_data();
    void inheritance();

    void defaultPalette_data();
    void defaultPalette();

    void listView_data();
    void listView();
};

void tst_palette::initTestCase()
{
    QQmlDataTest::initTestCase();

    // Import QtQuick.Controls to initialize styles and themes so that
    // QQuickControlPrivate::themePalette() returns a palette from the
    // style's theme instead of the platform's theme.
    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData("import QtQuick.Controls 2.3; Control { }", QUrl());
    delete component.create();
}

void tst_palette::palette_data()
{
    QTest::addColumn<QString>("testFile");
    QTest::addColumn<QPalette>("expectedPalette");

    QPalette defaultPalette = QQuickTheme::themePalette(QQuickTheme::SystemPalette);
    defaultPalette.setColor(QPalette::Base, QColor("#efefef"));
    defaultPalette.setColor(QPalette::Text, QColor("#101010"));

    QTest::newRow("Control") << "palette-control-default.qml" << defaultPalette;
    QTest::newRow("AppWindow") << "palette-appwindow-default.qml" << defaultPalette;
    QTest::newRow("Popup") << "palette-popup-default.qml" << defaultPalette;

    QPalette customPalette;
    customPalette.setColor(QPalette::AlternateBase, QColor("aqua"));
    customPalette.setColor(QPalette::Base, QColor("azure"));
    customPalette.setColor(QPalette::BrightText, QColor("beige"));
    customPalette.setColor(QPalette::Button, QColor("bisque"));
    customPalette.setColor(QPalette::ButtonText, QColor("chocolate"));
    customPalette.setColor(QPalette::Dark, QColor("coral"));
    customPalette.setColor(QPalette::Highlight, QColor("crimson"));
    customPalette.setColor(QPalette::HighlightedText, QColor("fuchsia"));
    customPalette.setColor(QPalette::Light, QColor("gold"));
    customPalette.setColor(QPalette::Link, QColor("indigo"));
    customPalette.setColor(QPalette::LinkVisited, QColor("ivory"));
    customPalette.setColor(QPalette::Mid, QColor("khaki"));
    customPalette.setColor(QPalette::Midlight, QColor("lavender"));
    customPalette.setColor(QPalette::Shadow, QColor("linen"));
    customPalette.setColor(QPalette::Text, QColor("moccasin"));
    customPalette.setColor(QPalette::ToolTipBase, QColor("navy"));
    customPalette.setColor(QPalette::ToolTipText, QColor("orchid"));
    customPalette.setColor(QPalette::Window, QColor("plum"));
    customPalette.setColor(QPalette::WindowText, QColor("salmon"));

    QTest::newRow("Control:custom") << "palette-control-custom.qml" << customPalette;
    QTest::newRow("AppWindow:custom") << "palette-appwindow-custom.qml" << customPalette;
    QTest::newRow("Popup:custom") << "palette-popup-custom.qml" << customPalette;
}

void tst_palette::palette()
{
    QFETCH(QString, testFile);
    QFETCH(QPalette, expectedPalette);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.loadUrl(testFileUrl(testFile));

    QScopedPointer<QObject> object(component.create());
    QVERIFY2(!object.isNull(), qPrintable(component.errorString()));

    QVariant var = object->property("palette");
    QVERIFY(var.isValid());

    QPalette actualPalette = var.value<QPalette>();
    QCOMPARE(actualPalette, expectedPalette);
}

void tst_palette::inheritance_data()
{
    QTest::addColumn<QString>("testFile");

    QTest::newRow("Control") << "inheritance-control.qml";
    QTest::newRow("Child Control") << "inheritance-childcontrol.qml";
    QTest::newRow("Dynamic Control") << "inheritance-dynamiccontrol.qml";
    QTest::newRow("Dynamic Child Control") << "inheritance-dynamicchildcontrol.qml";

    QTest::newRow("Popup") << "inheritance-popup.qml";
    QTest::newRow("Child Popup") << "inheritance-childpopup.qml";
    QTest::newRow("Dynamic Popup") << "inheritance-dynamicpopup.qml";
    QTest::newRow("Dynamic Child Popup") << "inheritance-dynamicchildpopup.qml";
}

void tst_palette::inheritance()
{
    QFETCH(QString, testFile);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.loadUrl(testFileUrl(testFile));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(component.create()));
    QVERIFY2(!window.isNull(), qPrintable(component.errorString()));

    QObject *control = window->property("control").value<QObject *>();
    QObject *child = window->property("child").value<QObject *>();
    QObject *grandChild = window->property("grandChild").value<QObject *>();
    QVERIFY(control && child && grandChild);

    QPalette defaultPalette = QQuickTheme::themePalette(QQuickTheme::SystemPalette);
    defaultPalette.setColor(QPalette::Base, QColor("#efefef"));
    defaultPalette.setColor(QPalette::Text, QColor("#101010"));

    QCOMPARE(window->palette(), defaultPalette);

    QCOMPARE(control->property("palette").value<QPalette>(), defaultPalette);
    QCOMPARE(child->property("palette").value<QPalette>(), defaultPalette);
    QCOMPARE(grandChild->property("palette").value<QPalette>(), defaultPalette);

    QPalette childPalette(defaultPalette);
    childPalette.setColor(QPalette::Base, Qt::red);
    childPalette.setColor(QPalette::Text, Qt::green);
    childPalette.setColor(QPalette::Button, Qt::blue);
    child->setProperty("palette", childPalette);
    QCOMPARE(child->property("palette").value<QPalette>(), childPalette);
    QCOMPARE(grandChild->property("palette").value<QPalette>(), childPalette);

    QPalette grandChildPalette(childPalette);
    grandChildPalette.setColor(QPalette::Base, Qt::cyan);
    grandChildPalette.setColor(QPalette::Mid, Qt::magenta);
    grandChild->setProperty("palette", grandChildPalette);
    QCOMPARE(child->property("palette").value<QPalette>(), childPalette);
    QCOMPARE(grandChild->property("palette").value<QPalette>(), grandChildPalette);

    QPalette windowPalette(defaultPalette);
    windowPalette.setColor(QPalette::Window, Qt::gray);
    window->setPalette(windowPalette);
    QCOMPARE(window->palette(), windowPalette);
    QCOMPARE(control->property("palette").value<QPalette>(), windowPalette);

    childPalette.setColor(QPalette::Window, Qt::gray);
    QCOMPARE(child->property("palette").value<QPalette>(), childPalette);

    grandChildPalette.setColor(QPalette::Window, Qt::gray);
    QCOMPARE(grandChild->property("palette").value<QPalette>(), grandChildPalette);

    child->setProperty("palette", QVariant());
    QCOMPARE(child->property("palette").value<QPalette>(), windowPalette);
    QCOMPARE(grandChild->property("palette").value<QPalette>(), grandChildPalette);

    grandChild->setProperty("palette", QVariant());
    QCOMPARE(grandChild->property("palette").value<QPalette>(), windowPalette);
}

class TestTheme : public QQuickTheme
{
public:
    TestTheme()
    {
        std::fill(palettes, palettes + QQuickTheme::NPalettes, static_cast<QPalette *>(0));

        QPalette palette = QPalette();
        palette.setColor(QPalette::Window, Qt::gray);
        palettes[QQuickTheme::SystemPalette] = new QPalette(palette);

        palette.setColor(QPalette::ToolTipBase, Qt::yellow);
        palettes[QQuickTheme::ToolTipPalette] = new QPalette(palette);

        palette.setColor(QPalette::ButtonText, Qt::blue);
        palettes[QQuickTheme::ToolButtonPalette] = new QPalette(palette);

        palette.setColor(QPalette::Button, Qt::red);
        palettes[QQuickTheme::ButtonPalette] = new QPalette(palette);

        palette.setColor(QPalette::Text, Qt::green);
        palettes[QQuickTheme::CheckBoxPalette] = new QPalette(palette);

        palette.setColor(QPalette::Text, Qt::blue);
        palettes[QQuickTheme::RadioButtonPalette] = new QPalette(palette);

        // HeaderPalette unused

        palette.setColor(QPalette::Base, Qt::darkGray);
        palettes[QQuickTheme::ComboBoxPalette] = new QPalette(palette);

        palette.setColor(QPalette::Base, Qt::lightGray);
        palettes[QQuickTheme::ItemViewPalette] = new QPalette(palette);

        // MessageBoxLabelPalette unused

        palette.setColor(QPalette::ButtonText, Qt::white);
        palettes[QQuickTheme::TabBarPalette] = new QPalette(palette);

        palette.setColor(QPalette::WindowText, Qt::darkGray);
        palettes[QQuickTheme::LabelPalette] = new QPalette(palette);

        palette.setColor(QPalette::Mid, Qt::gray);
        palettes[QQuickTheme::GroupBoxPalette] = new QPalette(palette);

        palette.setColor(QPalette::Shadow, Qt::darkYellow);
        palettes[QQuickTheme::MenuPalette] = new QPalette(palette);

        // MenuBarPalette unused

        palette.setColor(QPalette::Base, Qt::cyan);
        palettes[QQuickTheme::TextEditPalette] = new QPalette(palette);

        palette.setColor(QPalette::Base, Qt::magenta);
        palettes[QQuickTheme::TextLineEditPalette] = new QPalette(palette);
    }

    const QPalette *palette(Palette type = SystemPalette) const override
    {
        return palettes[type];
    }

private:
    QPalette *palettes[QQuickTheme::NPalettes];
};

Q_DECLARE_METATYPE(QQuickTheme::Palette)

void tst_palette::defaultPalette_data()
{
    QTest::addColumn<QString>("control");
    QTest::addColumn<QQuickTheme::Palette>("paletteType");

    QTest::newRow("AbstractButton") << "AbstractButton" << QQuickTheme::SystemPalette;
    QTest::newRow("ApplicationWindow") << "ApplicationWindow" << QQuickTheme::SystemPalette;
    QTest::newRow("Button") << "Button" << QQuickTheme::ButtonPalette;
    QTest::newRow("CheckBox") << "CheckBox" << QQuickTheme::CheckBoxPalette;
    QTest::newRow("CheckDelegate") << "CheckDelegate" << QQuickTheme::ItemViewPalette;
    QTest::newRow("ComboBox") << "ComboBox" << QQuickTheme::ComboBoxPalette;
    QTest::newRow("Container") << "Container" << QQuickTheme::SystemPalette;
    QTest::newRow("Control") << "Control" << QQuickTheme::SystemPalette;
    QTest::newRow("Dial") << "Dial" << QQuickTheme::SystemPalette;
    QTest::newRow("Dialog") << "Dialog" << QQuickTheme::SystemPalette;
    QTest::newRow("DialogButtonBox") << "DialogButtonBox" << QQuickTheme::SystemPalette;
    QTest::newRow("Drawer") << "Drawer" << QQuickTheme::SystemPalette;
    QTest::newRow("Frame") << "Frame" << QQuickTheme::SystemPalette;
    QTest::newRow("GroupBox") << "GroupBox" << QQuickTheme::GroupBoxPalette;
    QTest::newRow("ItemDelegate") << "ItemDelegate" << QQuickTheme::ItemViewPalette;
    QTest::newRow("Label") << "Label" << QQuickTheme::LabelPalette;
    QTest::newRow("Menu") << "Menu" << QQuickTheme::MenuPalette;
    QTest::newRow("MenuItem") << "MenuItem" << QQuickTheme::MenuPalette;
    QTest::newRow("MenuSeparator") << "MenuSeparator" << QQuickTheme::MenuPalette;
    QTest::newRow("Page") << "Page" << QQuickTheme::SystemPalette;
    QTest::newRow("Pane") << "Pane" << QQuickTheme::SystemPalette;
    QTest::newRow("Popup") << "Popup" << QQuickTheme::SystemPalette;
    QTest::newRow("ProgressBar") << "ProgressBar" << QQuickTheme::SystemPalette;
    QTest::newRow("RadioButton") << "RadioButton" << QQuickTheme::RadioButtonPalette;
    QTest::newRow("RadioDelegate") << "RadioDelegate" << QQuickTheme::ItemViewPalette;
    QTest::newRow("RangeSlider") << "RangeSlider" << QQuickTheme::SystemPalette;
    QTest::newRow("RoundButton") << "RoundButton" << QQuickTheme::ButtonPalette;
    QTest::newRow("ScrollBar") << "ScrollBar" << QQuickTheme::SystemPalette;
    QTest::newRow("ScrollIndicator") << "ScrollIndicator" << QQuickTheme::SystemPalette;
    QTest::newRow("Slider") << "Slider" << QQuickTheme::SystemPalette;
    QTest::newRow("SpinBox") << "SpinBox" << QQuickTheme::TextLineEditPalette;
    QTest::newRow("SwipeDelegate") << "SwipeDelegate" << QQuickTheme::ItemViewPalette;
    QTest::newRow("Switch") << "Switch" << QQuickTheme::CheckBoxPalette; // ### TODO: add QQuickTheme::SwitchPalette
    QTest::newRow("SwitchDelegate") << "SwitchDelegate" << QQuickTheme::ItemViewPalette;
    QTest::newRow("TabBar") << "TabBar" << QQuickTheme::TabBarPalette;
    QTest::newRow("TabButton") << "TabButton" << QQuickTheme::TabBarPalette;
    QTest::newRow("TextArea") << "TextArea" << QQuickTheme::TextEditPalette;
    QTest::newRow("TextField") << "TextField" << QQuickTheme::TextLineEditPalette;
    QTest::newRow("ToolBar") << "ToolBar" << QQuickTheme::ToolButtonPalette;
    QTest::newRow("ToolButton") << "ToolButton" << QQuickTheme::ToolButtonPalette;
    QTest::newRow("ToolSeparator") << "ToolSeparator" << QQuickTheme::ToolButtonPalette;
    QTest::newRow("ToolTip") << "ToolTip" << QQuickTheme::ToolTipPalette;
    QTest::newRow("Tumbler") << "Tumbler" << QQuickTheme::ItemViewPalette;
}

void tst_palette::defaultPalette()
{
    QFETCH(QString, control);
    QFETCH(QQuickTheme::Palette, paletteType);

    QQuickTheme::setCurrent(new TestTheme);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.setData(QString("import QtQuick.Controls 2.3; %1 { }").arg(control).toUtf8(), QUrl());

    QScopedPointer<QObject> object(component.create());
    QVERIFY2(!object.isNull(), qPrintable(component.errorString()));

    QVariant var = object->property("palette");
    QVERIFY(var.isValid());

    QPalette expectedPalette = QQuickTheme::themePalette(paletteType);
    QPalette actualPalette = var.value<QPalette>();
    QCOMPARE(actualPalette, expectedPalette);
}

void tst_palette::listView_data()
{
    QTest::addColumn<QString>("objectName");

    QTest::newRow("Control") << "control";
    QTest::newRow("Label") << "label";
    QTest::newRow("TextArea") << "textarea";
    QTest::newRow("TextField") << "textfield";
}

void tst_palette::listView()
{
    QFETCH(QString, objectName);

    QQmlEngine engine;
    QQmlComponent component(&engine);
    component.loadUrl(testFileUrl("listview.qml"));

    QScopedPointer<QQuickApplicationWindow> window(qobject_cast<QQuickApplicationWindow *>(component.create()));
    QVERIFY2(!window.isNull(), qPrintable(component.errorString()));

    window->show();
    QVERIFY(QTest::qWaitForWindowActive(window.data()));

    QQuickItem *listView = window->property("listView").value<QQuickItem *>();
    QVERIFY(listView);

    QQuickItem *contentItem = listView->property("contentItem").value<QQuickItem *>();
    QVERIFY(contentItem);

    QVERIFY(QMetaObject::invokeMethod(listView, "forceLayout"));

    QQuickItem *column = contentItem->childItems().value(0);
    QVERIFY(column);

    QQuickItem *control = column->property(objectName.toUtf8()).value<QQuickItem *>();
    QVERIFY(control);

    QCOMPARE(control->property("palette").value<QPalette>().color(QPalette::Highlight), QColor(Qt::red));
}

QTEST_MAIN(tst_palette)

#include "tst_palette.moc"

/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15

import Muse.Ui 1.0
import Muse.UiComponents 1.0

Item { // krasko: ability to click on the entire save option
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true

    property alias title: titleLabel.text
    property alias isFreeLabelVisible: freeRect.visible
    property alias description: descriptionLabel.text
    property alias buttonText: button.text

    property alias imageSource: image.source

    property alias navigation: button.navigation

    signal buttonClicked

    readonly property int radius: 6

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RoundedRectangle {
            Layout.fillWidth: true
            implicitHeight: 208

            color: ui.theme.isDark ? "#44495A" : "#D7DEE5"

            topLeftRadius: root.radius
            topRightRadius: root.radius

            Image {
                id: image
                anchors.fill: parent

                fillMode: Image.PreserveAspectFit
            }
        }

        RoundedRectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true

            color: ui.theme.backgroundSecondaryColor

            bottomLeftRadius: root.radius
            bottomRightRadius: root.radius

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 12

                Row {
                    spacing: 8

                    StyledTextLabel {
                        id: titleLabel
                        Layout.fillWidth: true
                        font: ui.theme.headerBoldFont
                        horizontalAlignment: Text.AlignLeft
                    }

                    Rectangle {
                        id: freeRect
                        implicitHeight: freeLabel.implicitHeight + 4 * 2
                        implicitWidth: freeLabel.implicitWidth + 12 * 2

                        color: ui.theme.accentColor
                        radius: height / 2

                        visible: false

                        StyledTextLabel {
                            id: freeLabel
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter

                            text: qsTrc("project/save", "Free")
                            font: ui.theme.tabBoldFont
                            horizontalAlignment: Text.AlignLeft
                        }
                    }
                }

                StyledTextLabel {
                    id: descriptionLabel
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    wrapMode: Text.WordWrap
                    maximumLineCount: 0
                    font: ui.theme.largeBodyFont
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignTop
                }

                FlatButton {
                    id: button
                    Layout.fillWidth: true
                    accentButton: true

                    onClicked: {
                        root.buttonClicked()
                    }
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        hoverEnabled: true
        cursorShape: "PointingHandCursor"

        onClicked: {
            root.buttonClicked()
        }
    }

    states: [
        State {
            name: "HOVERED"
            when: mouseArea.containsMouse

            PropertyChanges {
                root {
                    opacity: 0.8
                }
            }
        }
    ]
}

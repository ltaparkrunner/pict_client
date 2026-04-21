import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Window {
    id: root
    title: "Выбор объекта (Локально / MinIO)"
    width: 500; height: 400
//    modal: false
//    standardButtons: Dialog.Cancel | Dialog.Open

    property string currentSelectedPath: ""
    property string currentPath: "/"
    signal pathSelected(string folderPath)

    onVisibleChanged: {
        if (visible) {
            listView.forceActiveFocus()
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Навигационная панель (Табы)
        TabBar {
            Layout.fillWidth: true
            TabButton {
                text: "Локально"
//                onClicked: storageModel.openLocalPath("/")
                onClicked: storageModel.enterLocal("/")
            }
            TabButton {
                text: "MinIO"
//                onClicked: storageModel.openMinioBucket("main-bucket")
                onClicked: storageModel.enterMinio("main-bucket")
            }
        }

        // Список файлов и папок
        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true // clip what is it?
            focus: true

            model: storageModel // Объект UnifiedStorageModel из C++
            property real lastClickTime: 0
            // Настройка ScrollBar (Полоса прокрутки)
            ScrollBar.vertical: ScrollBar {
                active: true // Всегда видна при прокрутке
                policy: ScrollBar.AlwaysOn // Или AsNeeded
            }

            highlight: Rectangle { color: "lightblue"; radius: 2 }
            highlightFollowsCurrentItem: true

            delegate: ItemDelegate {
                width: listView.width
                //highlighted: root.currentSelectedPath === model.path
                highlighted: ListView.isCurrentItem

                contentItem: RowLayout {
                    spacing: 10
                    Text {
                        text: model.isDir ? "📁" : "📄"
                        font.pixelSize: 18
                    }
                    Column {
                        Layout.fillWidth: true
                        Text { text: model.name; font.bold: true }
                        Text {
                            text: model.isMinio ? "Облако MinIO" : "Локальный путь"
                            font.pixelSize: 10; color: "gray"
                        }
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        // let currentTime = Date.now()
                        // if (currentTime - listView.lastClickTime < 500) {
                        //     console.log("Too quickly")
                        //     return
                        // }
                        // listView.lastClickTime = currentTime
                    // root.currentSelectedPath = model.path
                    // if (model.isDir) {
                    //     // if (model.isMinio) storageModel.openMinioBucket(model.name)
                    //     // else storageModel.openLocalPath(model.path)
                    //     if (model.isMinio) storageModel.enterMinio(model.name)
                    //     else storageModel.enterLocal(model.path)
                    // }
                        console.log("listView.currentIndex = index")
                        listView.currentIndex = index
                    }
                    onDoubleClicked: {
                        root.currentSelectedPath = model.path
                        if (model.isDir) {
                            if (model.isMinio) storageModel.enterMinio(model.name)
                            else storageModel.enterLocal(model.path)
                        } else {
                            root.accept() // Или закрыть диалог с выбором файла
                        }
                    }
                }
            }
        }

        // Поле выбранного пути
        TextField {
            Layout.fillWidth: true
            text: root.currentSelectedPath
            readOnly: true
            placeholderText: "Ничего не выбрано"
        }
    }
    RowLayout {
        Layout.alignment: Qt.AlignRight
        Button { text: "Cancel"; onClicked: root.close() }
        Button {
            text: "Open";
            onClicked: { console.log(currentSelectedPath);
                root.pathSelected(currentSelectedPath);
                root.close() }
        }
    }

    // onAccepted: {
    //     console.log("Окончательный выбор:", currentSelectedPath)
    //     // Передаем путь в основное приложение
    // }
}

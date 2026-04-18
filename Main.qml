import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    title: "Fixed Left, Flexible Right"

    property string mainImageSource: ""
    property list<string> myImages: ["", "", "", "", "", ""]
    property var gridImages: [
            "path/to/gray.png", "path/to/gray.png",
            "path/to/gray.png", "path/to/gray.png",
            "path/to/gray.png", "path/to/gray.png"
        ]
    // Основной горизонтальный контейнер
    RowLayout {
        anchors.fill: parent
        anchors.margins: 5//10
        spacing: 5//20

        // ЛЕВАЯ ЧАСТЬ (Поля ввода и сетка)
        ColumnLayout {
            // Рассчитываем ширину: 2 колонки по 150px + отступы (например, 10px spacing + 20px margins)
            //Layout.maximumWidth: 330
            Layout.fillHeight: true
            //Layout.preferredWidth: 330//300
            Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : 330
            spacing: 5//10

            // Поля ввода
            TextField {
                placeholderText: "Line Edit 1"
                Layout.fillWidth: true
            }

            TextField {
                placeholderText: "Line Edit 2"
                Layout.fillWidth: true
            }
            Item{
                Layout.fillHeight: true
                Layout.fillWidth: true
                GridView {
                    id: grid
                    anchors.fill: parent
                    anchors.margins: 10
                    clip: true
                    // Layout.fillHeight: true
                    // Layout.fillWidth: true
                    // 1. Настройка размера ячейки (обязательно!)
                    cellWidth: 100
                    cellHeight: 100

                    // 2. Ваша C++ модель (imageModel)
                    model: imageModel

                    // 3. Описание одного элемента
                    delegate: Item {
                        width: grid.cellWidth
                        height: grid.cellHeight

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 5 // Отступы между картинками
                            color: "white"
                            border.color: "silver"
                            radius: 4

                            Image {
                                anchors.fill: parent
                                anchors.margins: 2
                                source: {
//                                    console.log("the way is: ", model.imagePath)
                                    model.imagePath // Данные из C++
                                }
                                fillMode: Image.PreserveAspectCrop // Чтобы не искажать пропорции
                                clip: true
                            }
                        }
                    }

                    // Включаем прокрутку, если элементов много
                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }

        // ПРАВАЯ ЧАСТЬ (Большое окно изображения)
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : parent.width - 330
            //Layout.preferredWidth: parent.width / 2
            //color: "black"
            color: mainImageSource === "" ? "#808080" : "#222"
            border.color: "#222"    //"#333"

            Image {
                anchors.fill: parent
                source: mainImageSource //"https://placeholder.com" // Замените на свое фото
                fillMode: Image.PreserveAspectFit

                // Плавное появление картинки
                opacity: status === Image.Ready ? 1 : 0
                Behavior on opacity { NumberAnimation { duration: 500 } }
            }

            Text {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                text: "No Image Loaded"//"Large Image Preview"
                color: "white"
                font.pixelSize: 18
                visible: mainImageSource === ""
            }
        }
    }
}

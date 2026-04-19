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
    ColumnLayout{
        anchors.fill: parent
//        Layout.fillHeight: true
//        Layout.fillWidth: true
//        Layout.fillWidth : bool
        RowLayout {
            Layout.fillHeight: true
            width: parent.width
            // Поля ввода
            ColumnLayout{
                Layout.fillWidth: true
                Text{
                    text: { return "Read/Write to filesystem" + "  " + toString(parent.width) }
                }
                RowLayout{
                    TextField {
                        placeholderText: "Line Edit 1"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Browse"
                        Layout.fillWidth: true
                    }
                }
                RowLayout{
                    Button {
                        text: "Write to filesystem"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Write to database"
                        Layout.fillWidth: true
                    }
                }
                Component.onCompleted: {
                    console.log("Ширина 1: ", width)
                }
            }
            ColumnLayout{
                Layout.fillWidth: true
                Text{
                    text: "Read/Write to database"
                }
                RowLayout{
                    Layout.fillWidth: true
                    TextField {
                        placeholderText: "Line Edit 1"
                        Layout.fillWidth: true
                    }
                    Button {
                        text: "Browse"
                        Layout.fillWidth: true
                    }
                }
                RowLayout{
                    Layout.fillWidth: true
                    Button {
                        text: "Write to filesystem"
                        Layout.fillWidth: true
                    }
                    Button {
                        //text: "Write to database"
                        text: ""
                        Layout.fillWidth: true
                    }
                }
                Component.onCompleted: {
                    console.log("Ширина родителя 2: ", width)
                }
            }
        }

        RowLayout {
           Layout.fillHeight: true
           width: parent.width
//           Layout.fillWidth: parent.width
//            anchors.fill: parent
            // anchors.margins: 5//10
            // spacing: 5//20

            // ЛЕВАЯ ЧАСТЬ (Поля ввода и сетка)
            ColumnLayout {
                id: cl1
                // Рассчитываем ширину: 2 колонки по 150px + отступы (например, 10px spacing + 20px margins)
                //Layout.maximumWidth: 330
                Layout.fillHeight: true
//                Layout.preferredWidth: 330//300
                Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : 330
                spacing: 5//10

                // // Поля ввода
                // RowLayout{
                //     Layout.preferredWidth: cl1.width
                //     TextField {
                //         placeholderText: "Line Edit 1"
                //         Layout.fillWidth: true
                //     }
                //     Button {
                //         text: "Browse"
                //         Layout.fillWidth: true
                //     }
                // }
                // RowLayout{
                //     Layout.preferredWidth: cl1.width
                //     Button {
                //         text: "Write to filesystem"
                //         Layout.fillWidth: true
                //     }
                //     Button {
                //         text: "Write to "
                //         Layout.fillWidth: true
                //     }
                // }

                // TextField {
                //     placeholderText: "Line Edit 2"
                //     Layout.fillWidth: true
                // }
                Item{
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    GridView {
                        id: grid
                        anchors.fill: parent
                        anchors.margins: 10
                        clip: true

                        cellWidth: 100
                        cellHeight: 100

                        model: imageModel

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
                                    source: model.imagePath // Данные из C++
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
            ColumnLayout{
                Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : parent.width - 330
//                Layout.preferredWidth:270
                // RowLayout{
                //     TextField {
                //         placeholderText: "Line Edit 1"
                //         Layout.fillWidth: true
                //     }
                //     Button {
                //         text: "Browse"
                //         Layout.fillWidth: true
                //     }
                // }
                // RowLayout{
                //     Button {
                //         text: "Write to filesystem"
                //         Layout.fillWidth: true
                //     }
                //     Button {
                //         text: "Write to "
                //         Layout.fillWidth: true
                //     }
                // }
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
    }
}

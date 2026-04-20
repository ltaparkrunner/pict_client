import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import com.myapp.helpers 1.0

ApplicationWindow {
    visible: true
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    title: "Fixed Left, Flexible Right"

    property string mainImageSource: ""
    property list<string> myImages: ["", "", "", "", "", ""]
    // property var gridImages: [
    //         "path/to/gray.png", "path/to/gray.png",
    //         "path/to/gray.png", "path/to/gray.png",
    //         "path/to/gray.png", "path/to/gray.png"
    //     ]
    // Основной горизонтальный контейнер
    ColumnLayout{
        anchors.fill: parent
        ColumnLayout {
            Layout.fillHeight: true
            width: parent.width
                RowLayout{
                    TextField {
                        id: tf1
                        placeholderText: "Open/Write file/folder"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 4
                        text: folderDialog.folder
                    }
                    Button {
                        text: "Open"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        onClicked: {
                            if(tf1.text && tf1.text.trim().length > 0) {
                                let type = FileHelper.checkPathType(tf1.text);
                                if (type === FileHelperType.LocalFile) {
                                    console.log("Это локальный файл");
                                } else if (type === FileHelperType.LocalFolder) {
                                    console.log("Это локальная папка");
                                } else if (type === FileHelperType.MinioBucket) {
                                    console.log("Это бакет MinIO");
                                } else if (type === FileHelperType.MinioFile) {
                                    console.log("Это объект (файл) в MinIO");
                                } else {
                                    console.log("Путь не распознан или не существует");
                                }
                            }
                        }
                    }
                    Button {
                        text: "Write"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                    }
                }
                RowLayout{
                    Button {
                        text: "Open file"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        onClicked: fileDialog.open()
                    }
                    Button {
                        text: "Open folder"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        onClicked: folderDialog.open()
                    }
                // }
                // RowLayout{
                    Button {
                        text: "Write to file"
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                    }
                    Button {
                        text: "Write to folder "
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
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
                // id: cl1
                // Рассчитываем ширину: 2 колонки по 150px + отступы (например, 10px spacing + 20px margins)
                Layout.fillHeight: true
                Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : 330
                spacing: 5//10
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
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredWidth: (parent.width - 330 < 300) ? parent.width / 2 : parent.width - 330

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
    FolderDialog {
        id: folderDialog
        title: "Select the folder with images"

        onAccepted: {
            console.log("Selected folder:", folder)
            // Здесь можно вызвать C++ метод для загрузки картинок из этой папки
            // imageModel.loadFromFolder(folder)
        }
    }
    FileDialog {
        id: fileDialog
        title: "Выберите изображение"
        folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)

        // Настройка фильтров файлов
        nameFilters: ["Image files (*.png *.jpg)", "All files (*)"]

        // Режимы: OpenFile (один), OpenFiles (несколько), SaveFile (сохранение)
        fileMode: FileDialog.OpenFile

        onAccepted: {
            console.log("Выбран файл: " + fileDialog.file)
            // fileDialog.file возвращает URL (file:///...)
        }
    }
    FileDialog {
        id: saveDialog
        fileMode: FileDialog.SaveFile
        title: "Сохранить файл как..."
        onAccepted: {
            // Вызываем ваш метод C++ для записи данных
            FileHelper.writeToFile(saveDialog.file, "Привет, это контент файла!")
        }
    }
    FolderDialog {
        id: folderDialog2
        title: "Выберите папку для сохранения"

        onAccepted: {
            console.log("Сохраняем в:", folder)

            // Вызываем C++ метод
            let success = FileHelper.saveFilesToFolder(folder, filesToSave)

            if (success) {
                console.log("Все файлы успешно сохранены!")
            } else {
                console.log("Произошла ошибка при сохранении.")
            }
        }
    }
    /*
        Если вы планируете сохранять файлы, которые еще не существуют на диске (например, данные из интернета или текст из памяти), C++ метод должен использовать QFile::write вместо QFile::copy.
        FolderDialog: Позволяет пользователю выбрать только директорию. Он возвращает путь вида file:///....
        QStringList: QML автоматически преобразует массив JavaScript [] в QStringList для C++.
        QFile::copy: Самый быстрый способ перенести файлы. Мы используем QFileInfo(srcPath).fileName(), чтобы сохранить оригинальное имя файла в новой папке.

    */
}

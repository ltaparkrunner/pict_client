import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import com.myapp.helpers 1.0
import QtQuick.Controls.Basic
import QtQuick.Dialogs

ApplicationWindow {
    visible: true
    width: 1000
    height: 600
    minimumWidth: 400
    minimumHeight: 300
    title: "Fixed Left, Flexible Right"

    Connections {
        target: imageModel
        function onMinioImageToQML(url) {
            mainImageSource = url;
            //console.log("Получено из C++:", url)
        }
    }

    CustomFileDialog {
        id: customDialog
        onOpenPathsSelected:(paths) => {
            if(paths){
                tf.text = paths[0]
                for(var path of paths)
                    processPath(path)
            }
        }
    }

    SecondCustomFileDialog {
        id: secondCustomDialog
        onOpenPathsSelected:(paths) => {
            tf.text = currentSelectedPath
            processPath(currentSelectedPath)
        }
        // root.writePathSelected(ls, currentSelectedPath)
        onWritePathsSelected:(ls, paths) => {
            console.log("onWritePathsSelected paths: ", paths)
            if(paths){
                for(var path of paths) {
                    console.log("onWritePathsSelected path: ", path)
                    let type = FileHelper.checkPathType(path);
                    if(type === FileHelperType.LocalFolder){
                        ifFilesInFolder(ls, path)
                        if(ls){
                            tf.text = path;
                            FileHelper.processWritePathsLocal(ls, path)
                            return
                        }
                    }
                    else if(type === FileHelperType.MinioBucket){
                        ifFilesInFolder(ls, path)
                        if(ls){
                            tf.text = path;
                            FileHelper.processWritePathsMinio(ls, path)
                            return
                        }
                    }
                    else if(type === FileHelperType.LocalFile){
                        const dir1 = path.substring(0, filePath.lastIndexOf("/"));
                        ifFilesInFolder(ls, dir1);
                        if(ls){
                            tf.text = dir1;
                            FileHelper.processWritePathsLocal(ls, dir1)
                            return
                        }
                    }
                    else if(type === FileHelperType.MinioFile){
                        const dir1 = path.substring(0, filePath.lastIndexOf("/"));
                        ifFilesInFolder(ls, dir1)
                        if(ls){
                            tf.text = dir1;
                            FileHelper.processWritePathsMinio(ls, dir1)
                            return
                        }
                    }
                }
            }
            msgNothingToDo.text = "You must select a folder to save the images."
            msgNothingToDo.open()
        }

        onDeletePathsSelected:(paths) => {
            console.log("onDeletePathsSelected:(paths)", paths)
            if(paths){
                for(var path of paths) {
                    console.log("onDeletePathsSelected:(paths)", path)
                    let type = FileHelper.checkPathType(path);
                    if(type === FileHelperType.LocalFolder){
                        FileHelper.processDeleteFolderLocal(path)
                    }
                    else if(type === FileHelperType.MinioBucket){
                        FileHelper.processDeleteFolderMinio(path)
                    }
                    else if(type === FileHelperType.LocalFile){
                        console.log("FileHelper.processDeleteFileLocal(path)")
                        FileHelper.processDeleteFileLocal(path)
                    }
                    else if(type === FileHelperType.MinioFile){
                        console.log("FileHelper.processDeleteFileMinio(path)")
                        FileHelper.processDeleteFileMinio(path)
                    }
                }
                tf.text = path;
            }
            else {
                msgNothingToDo.text = "You must select a folder to save the images."
                msgNothingToDo.open()
            }
        }
        // Check if the files are in the target folder
        function ifFilesInFolder(ls, path){
            for(const [index, filePath] of ls.entries()){
                const dir = filePath.substring(0, filePath.lastIndexOf("/"));
                // console.log("dir = ", dir, "path = ", path)
                if(dir === path) ls.splice(index, 1);
            }
        }
    }

    MessageDialog {
        id: msgNothingToDo
        title: "Nothing To Do"  // "Подтверждение"
        text: "Path is empty, or smth like this"    // "Вы уверены, что хотите удалить этот файл?"
        informativeText: "Nothing To Do"     //"Это действие нельзя будет отменить."
        buttons: MessageDialog.Ok | MessageDialog.Cancel

        onAccepted: close()     //console.log("Нажата кнопка ОК")
        onRejected: close()     //console.log("Нажата кнопка Отмена")
    }

    MessageDialog {
        id: plug
        title: "Plug"  // "Подтверждение"
        text: "Code is not ready"    // "Вы уверены, что хотите удалить этот файл?"
        informativeText: "It'll be written"     //"Это действие нельзя будет отменить."
        buttons: MessageDialog.Ok | MessageDialog.Cancel

        onAccepted: close()     //console.log("Нажата кнопка ОК")
        onRejected: close()     //console.log("Нажата кнопка Отмена")
    }

    property string mainImageSource: ""
    property list<string> myImages: ["", "", "", "", "", ""]
    // Основной горизонтальный контейнер
    ColumnLayout{
        anchors.fill: parent
        ColumnLayout {
            Layout.fillHeight: true
            width: parent.width
            RowLayout{
                TextField {
                    id: tf
                    placeholderText: "Open/Write file/folder"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 4
                    text: ""//folderDialog.folder
                    background: Rectangle {
                        implicitWidth: 200
                        implicitHeight: 40
                        color: tf.enabled ? "transparent" : "#353535"
                        border.color: tf.activeFocus ? "#21be2b" : "#bdbebf"
                        border.width: tf.activeFocus ? 2 : 1
                        radius: 4
                    }
                }
                Button {
                    text: "Open"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        if(tf.text && tf.text.trim().length > 0) {
                            let type = FileHelper.checkPathType(tf.text);
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
                    text: "Open file/folder"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        wsClient.connectToServer()
                        storageModel.enterLocal("/");
                        customDialog.show();
                    }
                }
                Button {
                    text: "Write file/folder"
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    onClicked: {
                        wsClient.connectToServer()
                        storageModel.enterLocal("/");
                        secondCustomDialog.show();
                    }
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
                            MouseArea {
                                anchors.fill: parent
                                onDoubleClicked: {
//                                    console.log("delegate onDoubleClicked: ", model.imagePath)
                                    mainImageSource = model.imagePath
                                }
                            }
                        }
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

//        folder: StandardPaths.writableLocation(StandardPaths.PicturesLocation)

    function processPath(path){
        if(path) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                grid.model.addImagePath(path)
                mainImageSource = grid.model.resolvePath(path)
            } else if (type === FileHelperType.LocalFolder) {
//                console.log("Это локальная папка");
                mainImageSource = grid.model.addImagesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
//                console.log("Это бакет MinIO");
                mainImageSource = grid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
                console.log("Это объект (файл) в MinIO", path);
                mainImageSource = grid.model.addMinioImagePath(path)
            } else {
                console.log("Путь не распознан или не существует");
            }
        }
    }
    function processWritePaths(ls, paths) {
        if(paths) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                grid.model.addImagePath(path)
                mainImageSource = grid.model.resolvePath(path)
            } else if (type === FileHelperType.LocalFolder) {
                mainImageSource = grid.model.addImagesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
//                console.log("Это бакет MinIO");
                mainImageSource = grid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
                console.log("Это объект (файл) в MinIO", path);
                mainImageSource = grid.model.addMinioImagePath(path)
            } else {
                console.log("Путь не распознан или не существует");
            }
        }
    }
    function processDeletePath(currentSelectedPath) {
        if(path) {
            let type = FileHelper.checkPathType(path);
            if (type === FileHelperType.LocalFile) {
                grid.model.addImagePath(path)
                mainImageSource = grid.model.resolvePath(path)
            } else if (type === FileHelperType.LocalFolder) {
                mainImageSource = grid.model.addImagesFromFolder(path)
            } else if (type === FileHelperType.MinioBucket) {
                mainImageSource = grid.model.addImagesFromMinioBucket(path)
            } else if (type === FileHelperType.MinioFile) {
                console.log("Это объект (файл) в MinIO", path);
                mainImageSource = grid.model.addMinioImagePath(path)
            } else {
                console.log("Путь не распознан или не существует");
            }
        }
    }
}

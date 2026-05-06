import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: authDialog
    title: isLoginMode ? "Вход" : "Регистрация"
    standardButtons: Dialog.Ok | Dialog.Cancel
    anchors.centerIn: parent
    modal: true

    property bool isLoginMode: true

    // Сигналы для C++ части
    signal loginRequested(string email, string password)
    signal registerRequested(string email, string password)

    ColumnLayout {
        spacing: 15
        anchors.fill: parent
        anchors.margins: 10

        Label {
            text: isLoginMode ? "Войдите в аккаунт" : "Создать новый аккаунт"
            font.pixelSize: 16
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: emailField
            placeholderText: "Email (например, eve.holt@reqres.in)"
            Layout.fillWidth: true
            inputMethodHints: Qt.ImhEmailCharactersOnly
        }

        TextField {
            id: passwordField
            placeholderText: "Пароль"
            echoMode: TextInput.Password
            Layout.fillWidth: true
        }

        // Переключатель между режимами
        Button {
            flat: true
            text: isLoginMode ? "Нет аккаунта? Зарегистрироваться" : "Уже есть аккаунт? Войти"
            Layout.alignment: Qt.AlignRight
            onClicked: isLoginMode = !isLoginMode
        }

        Label {
            id: errorLabel
            color: "red"
            text: "Пожалуйста, заполните поля"
            visible: false
        }
    }

    // Обработка нажатия кнопки "ОК"
    onAccepted: {
        if (emailField.text === "" || passwordField.text === "") {
            errorLabel.visible = true
            // Не закрываем диалог при ошибке (в Qt Quick Dialog это требует переопределения кнопки)
            return
        }

        if (isLoginMode) {
            loginRequested(emailField.text, passwordField.text)
        } else {
            registerRequested(emailField.text, passwordField.text)
        }
    }
}

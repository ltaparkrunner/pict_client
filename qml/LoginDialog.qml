import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Effects // Для теней (в Qt 6)
import QtQuick.Controls.Basic

Dialog {
    id: authDialog
    modal: true
    anchors.centerIn: parent
    width: 350

    // Убираем стандартный фон, чтобы сделать свой красивый
    background: Rectangle {
        color: "#ffffff"
        radius: 12
        border.color: "#e0e0e0"

        // Эффект тени (для Qt 6)
        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowBlur: 0.5
            shadowColor: "#22000000"
            shadowVerticalOffset: 4
        }
    }

    property bool isLoginMode: true

    signal loginRequested(string email, string password)
    signal registerRequested(string email, string password)

    contentItem: ColumnLayout {
        spacing: 20

        // Заголовок с анимацией смены текста
        Label {
            text: isLoginMode ? "С возвращением!" : "Создать аккаунт"
            font.pixelSize: 24
            font.weight: Font.DemiBold
            color: "#212121"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10

            //  Behavior on text { StringAnimation { duration: 200 } }
        }

        // Поле Email
        ColumnLayout {
            spacing: 5
            Layout.fillWidth: true
            Label { text: "Email"; font.pixelSize: 12; color: "#757575" }
            TextField {
                id: emailField
                placeholderText: "name@example.com"
                Layout.fillWidth: true
                leftPadding: 10
                background: Rectangle {
                    implicitHeight: 45
                    radius: 8
                    border.color: emailField.activeFocus ? "#2196F3" : "#bdbdbd"
                    border.width: emailField.activeFocus ? 2 : 1
                }
            }
        }

        // Поле Пароль
        ColumnLayout {
            spacing: 5
            Layout.fillWidth: true
            Label { text: "Пароль"; font.pixelSize: 12; color: "#757575" }
            TextField {
                id: passwordField
                echoMode: TextInput.Password
                placeholderText: "••••••••"
                Layout.fillWidth: true
                leftPadding: 10
                background: Rectangle {
                    implicitHeight: 45
                    radius: 8
                    border.color: passwordField.activeFocus ? "#2196F3" : "#bdbdbd"
                    border.width: passwordField.activeFocus ? 2 : 1
                }
            }
        }

        // Кнопка переключения режима
        Text {
            text: isLoginMode ? "Нет аккаунта? Зарегистрироваться" : "Уже есть аккаунт? Войти"
            color: "#2196F3"
            font.pixelSize: 13
            Layout.alignment: Qt.AlignHCenter
            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: isLoginMode = !isLoginMode
            }
        }

        // Основная кнопка действия
        Button {
            id: mainButton
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            Layout.topMargin: 10

            contentItem: Text {
                text: isLoginMode ? "ВОЙТИ" : "РЕГИСТРАЦИЯ"
                color: "white"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.bold: true
            }

            background: Rectangle {
                color: mainButton.pressed ? "#1976D2" : "#2196F3"
                radius: 8
            }

            onClicked: {
                if (isLoginMode)
                    loginRequested(emailField.text, passwordField.text)
                else
                    registerRequested(emailField.text, passwordField.text)
                authDialog.accept()
            }
        }

        // Кнопка отмены
        Button {
            text: "Cancel"
            flat: true
            Layout.alignment: Qt.AlignHCenter
            onClicked: authDialog.reject()
        }
    }
}

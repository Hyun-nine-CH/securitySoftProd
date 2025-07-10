#include "dialog_log.h"
#include "mainwindow_admin.h"
#include "mainwindow.h"

#include <QApplication>
#include <QJsonObject> // QJsonObject를 사용하기 위해 헤더 추가
#include <QTcpSocket>  // QTcpSocket을 사용하기 위해 헤더 추가

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 1. 싱글톤 패턴으로 유일한 로그인 다이얼로그 인스턴스를 가져옵니다.
    Dialog_log* loginDialog = Dialog_log::getInstance();

    // 2. 로그인 창을 실행하고, 사용자가 로그인을 성공적으로 마쳤을 때 (Accepted)만 다음 로직을 실행합니다.
    if (loginDialog->exec() == QDialog::Accepted)
    {
        // 3. 싱글톤 객체에서 소켓과 로그인 성공 정보를 가져옵니다.
        QTcpSocket* socket = loginDialog->getSocket();
        QJsonObject userInfo = loginDialog->getUserInfo();

        // 4. (매우 중요!) 소켓의 부모-자식 관계를 끊어, 로그인 창이 사라져도 소켓이 유지되도록 합니다.
        socket->setParent(nullptr);
        //    로그인 창이 더 이상 소켓의 이벤트를 처리하지 않도록 시그널 연결을 해제합니다.
        Dialog_log::disconnect(socket, &QTcpSocket::readyRead, loginDialog, nullptr);

        // 5. 사용자 정보(JSON)에서 필요한 값들을 추출합니다.
        qint64 clientId = userInfo["ClientId"].toInteger();
        // MainWindow 생성자에 userInfo 객체 전체를 넘겨주므로, 여기서 모든 정보를
        // 다 파싱할 필요는 없습니다. MainWindow가 필요한 정보를 직접 파싱하게 됩니다.

        // 6. ClientId 값(1000 이상은 관리자로 가정)에 따라 올바른 메인 윈도우를 생성합니다.
        if (clientId >= 1000) {
            // 관리자용 메인 윈도우를 생성하고, 소켓과 사용자 정보를 전달합니다.
            MainWindow_Admin* adminWin = new MainWindow_Admin(socket, userInfo, nullptr);
            adminWin->setAttribute(Qt::WA_DeleteOnClose); // 창이 닫히면 자동으로 메모리에서 해제
            adminWin->show();
        } else {
            // 고객사(클라이언트)용 메인 윈도우를 생성하고, 소켓과 사용자 정보를 전달합니다.
            MainWindow* clientWin = new MainWindow(socket, userInfo, nullptr);
            clientWin->setAttribute(Qt::WA_DeleteOnClose);
            clientWin->show();
        }

        // 로그인 다이얼로그는 역할을 다했으므로 안전하게 삭제합니다.
        loginDialog->deleteLater();

        // 메인 윈도우의 이벤트 루프를 시작합니다.
        return app.exec();
    }

    // 로그인에 실패했거나 사용자가 창을 그냥 닫은 경우, 프로그램을 종료합니다.
    return 0;
}

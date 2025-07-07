#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

// 전방 선언
class ClientInfoForm_Chat;
namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // ⭐️ 생성자에서 회사 이름을 받도록 변경
    explicit MainWindow(QTcpSocket* socket, const QString& companyName, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // ⭐️ 서버에서 오는 모든 메시지를 처리하는 단일 슬롯
    void handleServerMessage();

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_socket;
    QString m_companyName; // ⭐️ 자신의 소속 회사 이름
    ClientInfoForm_Chat* m_chatTab; // ⭐️ 채팅 탭 포인터
};

#endif // MAINWINDOW_H

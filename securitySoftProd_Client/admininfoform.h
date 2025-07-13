#ifndef ADMININFOFORM_H
#define ADMININFOFORM_H

#include <QWidget>
#include <QJsonArray>

namespace Ui {
class AdminInfoForm;
}

class AdminInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit AdminInfoForm(QWidget *parent = nullptr);
    ~AdminInfoForm();

    // 클라이언트 목록 표시 함수
    void displayClientList(const QJsonArray& clientArray);

    // 서버에서 받은 데이터 처리
    void handleIncomingData(qint64 dataType, const QByteArray& payload, const QString& filename);

signals:
    // 클라이언트 목록 요청 시그널
    void clientListRequested();

    // 클라이언트 검색 요청 시그널
    void searchClientsRequested(const QString& company, const QString& department,
                                const QString& manager, const QString& phone);

private slots:
    void on_pushButtonSearch_clicked();
    void on_pushButtonReset_clicked(); // 초기화 버튼 슬롯 추가

private:
    Ui::AdminInfoForm *ui;
};

#endif // ADMININFOFORM_H

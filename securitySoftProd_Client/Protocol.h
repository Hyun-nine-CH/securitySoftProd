#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtGlobal>

namespace Protocol {
enum DataType : qint64 {
    // 파일 관련
    File_Transfer = 0x01,

    // 제품 정보 관련
    Request_Product_List = 0x03, // 서버의 RequestPdInfo에 해당
    Update_Product = 0x04, // 서버의 ModiProductInfo에 해당
    Add_Product = 0x05,
    Delete_Product = 0x06,

    // 로그인/회원가입 관련
    Login_Request = 0x07,
    Join_Request = 0x08,

    // 사용자/제품/주문 정보 관련
    Request_User_Info = 0x09, // 서버의 RequestUserInfo에 해당
    Response_Product_List = 0x0A,
    Request_Client_List = 0x0B,
    Response_Client_List = 0x0C,
    Request_Order_List = 0x0D,
    Response_Order_List = 0x0E,
    Submit_Order = 0x0F,
    Add_Order = 0x10, // 서버의 AddOrderInfo에 해당
    Request_Order_Info = 0x11, // 서버의 RequestOrderInfo에 해당
    Request_That_Order = 0x15, //고객의 주문정보 조회

    // 채팅 관련
    Chat_History_Request = 0x12, // 서버의 RequestChatLogInfo에 맞춤
    Chatting_Parse = 0x13, // 서버의 ChattingParse에 해당 (새로 추가)

    // 클라이언트 전용 코드 (충돌 방지를 위해 높은 값 사용)
    ID_Check_Request = 0x14, // 아이디중복 요청
    Chat_Message = 0x20, // 서버의 default에 맞춤
    Chat_History_Response = 0x21, // 임시 값

    // 로그아웃 관련
    Logout_Request = 0x23 // 원래 0x10이었지만 Add_Order와 충돌해서 변경
};
}

#endif // PROTOCOL_H

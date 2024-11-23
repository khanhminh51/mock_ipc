# **IPC Custome Library**

Hai thư viện `ProcessApp` và `Server` được xây dựng để cung cấp chức năng giao tiếp giữa các process với nhau với cốt lõi là các api của message queue. Trong đó thư viện `Server` đóng vai trò là trung gian để các process gửi và nhận message. Còn thư viện `ProcessApp` sẽ cung cấp các api cho các process để thực hiện giao tiếp với nhau.


## **Features**
### Function Requirement ###
1. **Create connect to server**  
   - Mỗi process có thể tạo hàng đợi thông điệp riêng bằng cách sử dụng hàm `createAppQueue`.  
   - Hàng đợi được đặt tên dựa trên tên của process, cho phép giao tiếp dễ dàng giữa các process thông qua server.

2. **Set data**  
   - Mỗi process có thể tạo dữ liệu cho riêng mình thông qua hàm `setData` và sẽ gửi thông báo lên server.

3. **Subscribe**  
   - Mỗi process có thể đăng ký vào các process khác thông qua hàm `subscribeTo`.

4. **Get data**  
   - Mỗi process có thể lấy dữ liệu từ process khác mà nó đã đăng ký thông qua hàm `getData`

5. **Set data from app**  
   - Mỗi process cũng có thể set data cho process mà nó đã đăng ký thông qua hàm `setDataFromApp`.

6. **Unsubscribe**  
   - Process có thể bỏ đăng ký tới process khác thông qua hàm `unsubscribeTo`.

### Non-Function Requirement ###
1. **Multi data**  
   - Thư viện hỗ trợ các loại dữ liệu khác nhau (`int`, `std::string`, `struct`) thông qua kiểu dữ liệu linh hoạt `std::variant`.
2. **Multi thread**  
   - Thư viện sử dụng cơ chế multi thread để tách các việc nhận và gửi message giúp tăng tốc độ phản hồi của hệ thống.



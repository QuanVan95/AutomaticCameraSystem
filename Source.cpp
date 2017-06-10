
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include "Tserial.h"

using namespace std;
using namespace cv;
/** Hàm detectAndDisplay là hàm dùng để nhận dạng tọa độ của khuôn mặt và hiển thị ra màn hình*/
/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables (Các biến toàn cục) */
/** Sao chép đường dẫn file haarcascade_frontalface_alt.xml từ opencv/data/haarscascades trên máy tính của bạn, */
//-- Note, either copy these  files from opencv/data/haarscascades to your current folder, or change these locations
String face_cascade_name = "E:\\Arduino IDE\\openvc3\\opencv\\sources\\data\\haarcascades\\haarcascade_frontalface_alt.xml";

/**Sử dụng lớp CascadeClassifier để phát hiện các đối tượng trong một luồng video. Chúng ta sẽ sử dụng hàm
detectMultiScale để tải hình ảnh lên và thực hiện việc phát hiện*/
CascadeClassifier face_cascade;

//Name of Window (tên của cửa sổ camera)
string window_name = "Capture - Face detection";

// Serial to Arduino global declarations (khai báo cổng nối tiếp toàn cục của Arduino)
int arduino_command;
Tserial *arduino_com;
unsigned char MSB = 0;
unsigned char LSB = 0;
short MSBLSB = 0;
// Serial to Arduino global declarations
int main(int argc, const char** argv)
{
	VideoCapture capture;//Khai báo lớp để lấy ảnh từ các tập tin video, chuỗi mô phỏng hình ảnh hoặc camera
	Mat frame;//Đọc vào đối tượng Mat của OpenCV
	arduino_com = new Tserial();// khởi tạo lớp Tserial
	if (arduino_com != 0) {
		//sử dụng hàm connect từ lớp Tserial đã khởi tạo
		arduino_com->connect("COM3", 57600, spNONE);
	}
	//// serial to Arduino setup (thiết lập kết nối tới Arduino) 
	//-- 1. Load the cascades (Nạp ảnh)
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };
	//-- 2. Read the video stream (Đọc các luồng video)
	capture.open(0);
	if (capture.isOpened())
	{
		for (;;)
		{
			capture >> frame;//đọc các mô phỏng hình ảnh đối tượng từ camera
			if (!frame.empty())
			{
				detectAndDisplay(frame);
			}
			if (waitKey(30) >= 0)    // Pause key                                                 
				break;
		}
	}
	// Serial to Arduino - shutdown (ngắt kết nối với Arduino)
	arduino_com->disconnect();
	delete arduino_com;
	arduino_com = 0;
	return 0;
}

void detectAndDisplay(Mat frame)
{
	std::vector<Rect> faces;//Khai báo 1 mảng để lưu các giá trị tọa độ x và y 
	Mat frame_gray; //Đọc vào đối tượng Mat của OpenCV

	/**Chuyển đổi hình ảnh từ BGR sang định dạng Grayscale,
	có nghĩa là chuyển từ ảnh có 3 kênh màu thông dụng Red,Green,Blue
	thành  ảnh 8 bits Grayscale, dùng hàm: cvtColor(InputArray, OutputArray, int code(code là mã màu)) */
	cvtColor(frame, frame_gray, CV_BGR2GRAY);

	/**cân bằng biểu đồ của một hình ảnh màu xám, đầu vào là 1 ảnh
	màu xám 8 bit. Chức năng của nó là sử dụng các thuật toán để
	bình thường hóa độ sáng và tăng độ tương phản của hình ảnh*/
	equalizeHist(frame_gray, frame_gray);

	//-- Detect faces (nhận dạng khuôn mặt)
	/** hàm detectMultiScale: Phát hiện các đối tượng kích thước
	khác nhau trong hình ảnh nhập vào. Phát hiện các đối tượng được
	trả về như là một danh sách các hình chữ nhật.*/
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	for (int i = 0; i < faces.size(); i++)
	{
		//hàm Point cung cấp tọa đô trung tâm x,y của hình cung
		/*Point center;
		center.x = faces[i].x + (int)faces[i].width / 2;
		center.y = faces[i].y + (int)faces[i].height / 2;*/
		Point center(faces[i].x + (int)faces[i].width*0.5, faces[i].y + (int)faces[i].height*0.5);

		//hàm vẽ 1 vòng cung đơn giản đặt ngay chỗ khuôn mặt của bạn
		ellipse(frame, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(155, 100, 65), 2, 8, 0);
		cout << "X:" << faces[i].x << "  y:" << faces[i].y << endl;

		// send X,Y of face center to serial com port (gửi x,y của trung tâm khuôn mặt tới cổng nối)
		// send X axis (gửi trục x)
		// read least significant byte (đọc những byte gần nhất)
		LSB = faces[i].x & 0xff;

		// read next significant byte (đọc những byte tiếp theo)
		MSB = (faces[i].x >> 8) & 0xff;
		arduino_com->sendChar(MSB);
		arduino_com->sendChar(LSB);

		// Send Y axis (gửi trục y) nó cũng hoạt động tương tự trục x
		LSB = faces[i].y & 0xff;// he 16 (hex)
		MSB = (faces[i].y >> 8) & 0xff;
		arduino_com->sendChar(MSB);
		arduino_com->sendChar(LSB);

		// serial com port send	 
		Mat faceROI = frame_gray(faces[i]);
	}
	//hiển thị ảnh
	imshow(window_name, frame);
}


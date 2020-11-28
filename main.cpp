#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include "pos.h"
#include <thread>
#include <chrono>
#include "serialcomm.h"


#define MAX 7  //이 미로는 세로를 x 가로를 y라고 설정
#define PATH 0
#define WALL 1
#define VISITED 2
#define BACKTRACKED 3

using namespace cv;
using namespace std;


int maze[MAX][MAX] = {
	{ 0,1,1,0,1,0,0 },
	{ 0,0,0,0,0,1,0 },
	{ 1,1,0,1,0,0,0 },
	{ 0,0,1,0,0,1,0 },
	{ 1,0,0,1,0,1,1 },
	{ 1,1,0,0,0,0,1 },
	{ 0,0,0,1,1,0,0 }, };

int n = 7; //미로의 크기


CSerialComm serialComm;
bool movable(pos pos, int dir);
stack<Position> find_route(Position p);
void show_route(stack<Position> s);

class match
{
public:
	stack<Position> input_stack;
	Position input;
	Point output;
	vector<Point> store_output;

	match(stack<Position> input_stack) { this->input_stack = input_stack; }
	void putVector(vector<Point> v) { store_output.assign(v.begin(), v.end()); }
	vector<Point> showVector() { return store_output; }
	Point popstack()  //스택의 탑값을 확인하고 그에 맞는 Point값으로 반환
	{
		Position temp = input_stack.top();
		int x = temp.x;
		int y = temp.y;
		return store_output.at(((x * 7) + y));
	}
};

ostream& operator << (ostream& stream, Position a) //<<연산자 재정의를 이용해서 position 출력
{
	stream << "(" << a.x << "," << a.y << ")";
	return stream;
}

Point operator - (Point &a, Point &b)
{
	Point sub;
	sub.x = a.x - b.x;
	sub.y = a.y - b.y;
	if (sub.x < 0) sub.x = -sub.x;
	if (sub.y < 0) sub.y = -sub.y;
	return sub;
}

Point divide(Point x, Point y) // 두 점 간의 차를 거리로 쓰려고 만든 함수
{
	Point a = x - y;
	if (a.x < 0)
		a.x = -a.x;
	if (a.y < 0)
		a.y = -a.y;
	return a;
}

bool movable(pos pos, int dir) {             //이동할수 있는지 없는지 판단하는 부울 함수
	Position new_pos;   //이동할 새로운 좌표를 변수로 설정한다.
	if (dir == 0) //북쪽일때 x좌표 -1
	{
		new_pos.x = pos.x - 1;
		new_pos.y = pos.y;
		if ((maze[new_pos.x][new_pos.y] == PATH) && new_pos.x >= 0 && new_pos.x < n &&new_pos.y >= 0 && new_pos.y < n)  //x,y좌표가 전부 0~n-1안에 있어야 한다.  //가는 곳이 PATH일때만 갈 수 있다.
			return true;
		else
			return false;
	}
	else if (dir == 1) //동쪽일 때 y좌표 +1
	{
		new_pos.x = pos.x;
		new_pos.y = pos.y + 1;
		if ((maze[new_pos.x][new_pos.y] == PATH) && new_pos.x >= 0 && new_pos.x < n  &&new_pos.y >= 0 && new_pos.y < n)  //x,y좌표가 전부 0~n-1안에 있어야 한다. //가는 곳이 PATH일때만 갈 수 있다.
			return true;
		else
			return false;
	}
	else if (dir == 2) // 남쪽일 때 x좌표 +1
	{
		new_pos.x = pos.x + 1;
		new_pos.y = pos.y;
		if ((maze[new_pos.x][new_pos.y] == PATH) && new_pos.x >= 0 && new_pos.x < n  &&new_pos.y >= 0 && new_pos.y < n)  //x,y좌표가 전부 0~n-1안에 있어야 한다. //가는 곳이 PATH일때만 갈 수 있다.
			return true;
		else
			return false;
	}
	else if (dir == 3) //서쪽일 때 y좌표 -1
	{
		new_pos.x = pos.x;
		new_pos.y = pos.y - 1;
		if ((maze[new_pos.x][new_pos.y] == PATH) && new_pos.x >= 0 && new_pos.x < n  &&new_pos.y >= 0 && new_pos.y < n)  //x,y좌표가 전부 0~n-1안에 있어야 한다. //가는 곳이 PATH일때만 갈 수 있다.
			return true;
		else
			return false;
	}
	else
		return false;
}

stack<Position> find_route(Position p)  //맵을 추적하여 최단경로를 찾는 함수
{
	stack<Position> s;
	Position temp = p;
	int cnt = maze[p.x][p.y];  //목적지까지의 거리
							   //s.push(p);
	while (cnt < 0) //cnt++ 시켜서 동서남북중에 maze[][]의 값이 cnt랑 같은거 스택에 저장
	{
		cnt++;
		if (maze[temp.x - 1][temp.y] == cnt) //북쪽으로 간거
		{
			s.push(temp);
			temp.x = temp.x - 1;
			temp.y = temp.y;
		}
		else if (maze[temp.x][temp.y + 1] == cnt)  //동쪽으로 검사
		{
			s.push(temp);
			temp.x = temp.x;
			temp.y = temp.y + 1;
		}
		else if (maze[temp.x + 1][temp.y] == cnt)  //남쪽으로 검사
		{
			s.push(temp);
			temp.x = temp.x + 1;
			temp.y = temp.y;
		}
		else if (maze[temp.x][temp.y - 1] == cnt)  //서쪽으로 검사
		{
			s.push(temp);
			temp.x = temp.x;
			temp.y = temp.y - 1;
		}
		else;

	}
	return s;
}

void show_route(stack<Position> s)  //스텍에 있는 것들 보여주는 함수
{
	cout << "최단경로 칸 수 :" << s.size() << endl;
	while (!(s.empty()))
	{
		cout << s.top() << " ";
		s.pop();
	}
}

stack<Position> MiroAlgorithm()
{
	queue<Position> que;
	stack<Position> result_stack;  //경로를 저장하는 스택

	Position first_cur;  //처음 출발점 지정
tryA:
	cout << "출발점을 지정해주세요 : ";
	cin >> first_cur.x >> first_cur.y;
	if (!(maze[first_cur.x][first_cur.y] == PATH))
	{
		cout << "출발점을 잘못 입력하였습니다" << endl;
		goto tryA;

	}

	int goal_x, goal_y;         //도착점 지정
tryB:
	cout << "도착점을 지정해주세요 : ";
	cin >> goal_x >> goal_y;
	if (!(maze[goal_x][goal_y] == PATH))
	{
		cout << "도착점을 잘못 입력하였습니다" << endl;
		goto tryB;

	}

	que.push(first_cur);
	maze[first_cur.x][first_cur.y] = -1;
	bool found = false;

	while (!(que.empty()))
	{
		Position cur = que.front();  //큐에 현재 상태를 집어 넣는다.
		que.pop();
		for (int dir = 0; dir < 4; dir++)
		{
			if (movable(cur, dir))
			{
				Position p = move_to(cur, dir);
				maze[p.x][p.y] = maze[cur.x][cur.y] - 1; //추가 배열을 쓰지 않기 위해서 방문 표시를 음수로 저장한다.
				if (p.x == goal_x && p.y == goal_y) //목적지에 도달했을 때
				{
					cout << "Found the path" << endl;
					found = true;
					result_stack = find_route(p);
					return result_stack;
					//show_route(result_stack);  //이거 과정하면 top하고 pop되니깐
					cout << endl;
					break;
				}
				que.push(p);
			}

		}
	}
}


void swap(int& a, int& b)
{
	int temp = a;
	a = b;
	b = temp;
}

void Send_data(BYTE data) {

	if (!serialComm.sendCommand(data))
	{
		printf("send command failed\n");
	}


}

void Connect_Uart_Port(const char* _portNum) {
	if (!serialComm.connect(_portNum))
	{
		cout << "connect faliled";
		//return -1;
		return;
	}
}

void DisConnect_Uart_Port() {
	serialComm.disconnect();
}

void Transfer_num_to_char(char *txDataBuf, int x_Temp,int y_Temp) {

	txDataBuf[0] = '#';
	txDataBuf[1] = (x_Temp / 1000) + 48;
	txDataBuf[2] = (x_Temp % 1000) / 100 + 48;
	txDataBuf[3] = (x_Temp % 100) / 10 + 48;
	txDataBuf[4] = (x_Temp % 10) + 48;
	txDataBuf[5] = ',';
	txDataBuf[6] = (y_Temp / 1000) + 48;
	txDataBuf[7] = (y_Temp % 1000) / 100 + 48;
	txDataBuf[8] = (y_Temp % 100) / 10 + 48;
	txDataBuf[9] = (y_Temp % 10) + 48;
}



int main()
{
	
	Connect_Uart_Port("COM3");

	//show_route(MiroAlgorithm());
	//미로찾기 알고리즘
	
	
	double center_x, center_y;
	int range_count = 0;


	/*CComm com3;
	com3.Open("COM3",115200);*/

	
	Scalar red(0, 0, 255);
	Scalar blue(255, 0, 0);
	Scalar yellow(0, 255, 255);

	Scalar magenta(255, 0, 255);

	Mat rgb_color = Mat(1, 1, CV_8UC3, red);
	Mat hsv_color;

	cvtColor(rgb_color, hsv_color, COLOR_BGR2HSV); //hsv 컬러로 변환

	int hue = (int)hsv_color.at<Vec3b>(0, 0)[0];
	int saturation = (int)hsv_color.at<Vec3b>(0, 0)[1];
	int value = (int)hsv_color.at<Vec3b>(0, 0)[2];

	//cout << "hue = " << hue << endl;
	//cout << "saturation = " << saturation << endl;
	//cout << "value = " << value << endl;

	int low_hue = hue - 10;
	int high_hue = hue + 10;

	int low_hue1 = 0, low_hue2 = 0;
	int high_hue1 = 0, high_hue2 = 0;

	if (low_hue < 10) {
		range_count = 2;

		high_hue1 = 180;
		low_hue1 = low_hue + 180;
		high_hue2 = high_hue;
		low_hue2 = 0;
	}
	else if (high_hue > 170) {
		range_count = 2;

		high_hue1 = low_hue;
		low_hue1 = 180;
		high_hue2 = high_hue - 180;
		low_hue2 = 0;
	}
	else {
		range_count = 1;

		low_hue1 = low_hue;
		high_hue1 = high_hue;
	}


	VideoCapture cap;
	Mat img_first,img_frame, img_hsv;

	int deviceID = 0;
	int apilD = CAP_ANY;
	cap.open(deviceID + apilD);

	if (!cap.isOpened()) {
		cerr << "ERROR! Unable to open camera\n";
		return -1;
	}

	/////한장만 받아오는 것 ,이게 기준이 될꺼에용, 이거 가지고 기준 좌표도 잡을꺼에용
	cap.read(img_first);
	if (img_first.empty()) {  //만약 안받아지면 꺼지고요.
		cerr << "ERROR! blank frame grabbed\n";
		return 0;
	}
	//HSV로 변환
	cvtColor(img_first, img_hsv, COLOR_BGR2HSV);

	//지정한 HSV 범위를 이용하여 영상을 이진화
	Mat img_mask1, img_mask2;
	inRange(img_hsv, Scalar(low_hue1, 50, 50), Scalar(high_hue1, 255, 255), img_mask1);
	if (range_count == 2) {
		inRange(img_hsv, Scalar(low_hue2, 50, 50), Scalar(high_hue2, 255, 255), img_mask2);
		img_mask1 |= img_mask2;
	}

	//morphological opening 작은 점들을 제거 
	erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
	dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));

	//morphological closing 영역의 구멍 메우기 
	dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	//라벨링 
	Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(img_mask1, img_labels,
		stats, centroids, 8, CV_32S);

	vector<Point> vTemp;

	//영역박스 그리기
	int max = -1, idx = 0;
	for (int j = 1; j < numOfLables; j++) {  //라벨링하는 과정
		int area = stats.at<int>(j, CC_STAT_AREA);
		center_x = centroids.at<double>(j, 0);
		center_y = centroids.at<double>(j, 1);
		if (max < area)
		{
			max = area;
			idx = j;
		}
		vTemp.push_back(Point(center_x, center_y));
		circle(img_first, Point(center_x, center_y), 3, Scalar(0, 255, 255), -1); //빨간색 부분을 화면에 띄울꺼에용
	}
	cout << "각 모서리의 좌표값 : " << endl;
	cout << vTemp << endl;
	cout << endl;


	//좌표 잘 안찍히면 여기 순서도 한번 바꿔보세요.
	Point distance_width = divide(vTemp.at(0), vTemp.at(1)); //두 점의 차를 이용해서 가로 길이를 구합니다.
	Point distance_height = divide(vTemp.at(2), vTemp.at(0)); //두 점의 차를 이용해서 세로 길이를 구합니다. 

	double width_equal_x = distance_width.x / 14; //가로 길이 간격
	double height_equal_y = distance_height.y / 14; //세로 길이 간격

	vector<Point> vPoint;

	//cout << distance_width << endl;
	//cout << distance_height << endl;

	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{   //만약 점이 잘 찍히지 않으면 이 부분의 부호를 고쳐주세용
			if (vTemp.at(0).x > vTemp.at(1).x)
			{
				swap(vTemp.at(0), vTemp.at(1));
				Point location(vTemp.at(0).x + ((width_equal_x)*(2 * j + 1)), vTemp.at(1).y + ((height_equal_y)*(2 * i + 1)));
				circle(img_first, location, 3, Scalar(0, 255, 255), -1);
				vPoint.push_back(location);
				
			}
			else if(vTemp.at(0).x < vTemp.at(1).x)
			{
				Point location(vTemp.at(0).x + ((width_equal_x)*(2 * j + 1)), vTemp.at(0).y + ((height_equal_y)*(2 * i + 1)));	
				circle(img_first, location, 3, Scalar(0, 255, 255), -1);
				vPoint.push_back(location);
			}
			
		}
	}
	//cout << "각 좌표의 중심 좌표 값 :" << endl;
	//cout << vPoint << endl;

	//여기서 클래스 만들겠습니다.
	match m(MiroAlgorithm());
	m.putVector(vPoint);
	//cout << m.showVector() << endl;
	//cout << "최단경로 칸 수 : " << m.input_stack.size() << endl;
	
	
	vPoint.clear(); //좌표 담는 벡터 클리어
	vTemp.clear();// 배열 담는 벡터 클리어


	Mat gray, edge;
	int cnt_dis = 0;

	int pwm_x = 0;
	int pwm_y = 0;;
	char ic[10];
	
	int err_x = 0;
	int err_y = 0;

	int pre_err_x = 0;
	int pre_err_y = 0;

	int sum_err_x = 0;
	int sum_err_y = 0;

	///////////////////////////kd값 설계
	double kp = 0.9;
	double kd = 12;

	while(true)
	{
		std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();
		
		// wait for a new frame from camera and store it into 'frame'
		cap.read(img_frame);
		// check if we succeeded
		if (img_frame.empty()) {
			cerr << "ERROR! blank frame grabbed\n";
			break;
		}
		
		cvtColor(img_frame, gray, COLOR_BGR2GRAY);  //그레이스케일 과정
		GaussianBlur(gray,edge, Size(3, 3), 2, 2);  //가우시안 불러 필터 거치는 과정인데 여기 사이즈값 조절하면 공 좀 더 잘 잡혀요
		
		vector<Vec3f> circles;
		HoughCircles(edge, circles, CV_HOUGH_GRADIENT, 1,edge.rows/8,400,30,15,40);
		
		
		//허프변환해서 찾은 공 검출 찾은거 화면에 띄우는 과정


		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(img_frame, center, 3, Scalar(0, 255, 0), -1, 8, 0); //원 중심 검출
			circle(img_frame, center, radius, Scalar(0, 0, 255), 3, 8, 0); //원 테두리 검출
			//cout << center << endl;
			//cout << m.popstack() << endl;
			//cout << "지금의 목적지 : " << m.input_stack.top() << endl;
			pre_err_x = err_x;
			pre_err_y = err_y;
			Point distance = m.popstack() - center;
			err_x = distance.x;
			err_y = distance.y;
			//cout << distance << endl;

			/////////////////////////////pd제어기 설계//////////////////////////////////


			sum_err_x = kp*err_x + kd*(err_x - pre_err_x);
			sum_err_y = kp*err_y + kd*(err_y - pre_err_y);

			pwm_x = 430 + sum_err_x;
			pwm_y = 370 + sum_err_y ;

			if (pwm_x > 1250)
			{
				pwm_x = 1249;
			}
			if (pwm_y > 1250)
			{
				pwm_y = 1249;
			}
			if (pwm_x < 0)
			{
				pwm_x = 1;
			}
			if (pwm_y < 0)
			{
			
				
				pwm_y = 0;
			}

			

			Transfer_num_to_char(ic, pwm_x, pwm_y);

			for (int i = 0; i < 10; i++)
			{
				Send_data(ic[i]);
			}

			if (distance.x <40 && distance.x > -40 && distance.y < 40 && distance.y > -40)  //영역안에서 얼마나 들어와 있니
			{
				cnt_dis++;;
			}
			if (cnt_dis == 15)
			{
				cnt_dis = 0;
				if (m.input_stack.size() == 1)
				{

				}
				else
				{
					m.input_stack.pop();
				}
				
				
			}
			cout << "얼마나 거기있었니 : " << cnt_dis << endl;
			/*if (m.input_stack.empty())
			{
				cout << "끝!!" << endl;
				pwm_x = 430;
				pwm_y = 370;

				Transfer_num_to_char(ic, pwm_x, pwm_y);
				for (int i = 0; i < 10; i++)
				{
					Send_data(ic[i]);
				}
				break;
			}*/
		}
		
		

		//com3.Write(cTmp, 7);

		imshow("원본 영상", img_frame);
		//imshow("엣지 영상", edge);

		if (waitKey(1) >= 0)
			break;
		std::chrono::system_clock::time_point EndTime = std::chrono::system_clock::now();

		std::chrono::milliseconds mill = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);
		//std::cout << "함수를 수행하는 걸린 시간(초) : " << mill.count() << " milliseconds" << std::endl;
		std::this_thread::sleep_for(0.05s-mill);  //제어주기 만드는 것 쓰레드를 sleep해서 지금은 0.05초 50ms
	
	}
	
	DisConnect_Uart_Port();

	return 0;
}

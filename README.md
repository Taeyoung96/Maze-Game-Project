# Upgrade Miro Game

광운대학교 로봇학부 학술소모임 **'BARAM'** 19년도 후반기 Toy Project에 대한 소스코드입니다.  

## 개발 환경
|OS|사용 언어|사용 IDE|
|:---:|:---:|:---:|
|Window 10|C++|Visual Studio 2017|

## 프로젝트 개발 동기

- 알고리즘을 공부하다 보면 빠짐 없이 등장하는 상황들이 있는데, 그 중 하나가 **미로 찾기**이다.  
  직접 미로를 만들고 내가 원하는 도착점으로 가는 프로젝트를 한다면,  
  간단한 PD 제어도 구현할 수 있고 OpenCV 및 알고리즘도 배울 수 있을 것 같아 이 프로젝트를 선정하였다.  
  
## 프로젝트 개요
1. OpenCV 3.4.3을 이용하여 미로판 인식 및 공의 위치 인식
2. 시작점과 끝점을 이용해 최단경로를 구해주는 BFS 알고리즘 구현  
3. 서보모터 이용한 PD제어를 통해 공이 정확한 위치로 갈 수 있도록 설계   

## System Architecture
<p align="center"><img src="https://user-images.githubusercontent.com/41863759/100521090-dde4dc80-31e4-11eb-8944-e06218821326.JPG" width="600px"></p>  


### Code Overview  
- `x64/` : Visual Studio를 이용하면 자동으로 생성  
- `pos.h` : 미로의 방향에 대한 헤더 파일  
- `serialport.h` , `serialport.cpp`, `serialcomm.h`, `serialcomm.cpp`, `Comm.h`, `Comm.cpp` : PC와 ATmega128 통신을 위한 코드
- `main.cpp` : 프로젝트 main 소스 코드

### Project scenario

1. 콘솔 창에 시작점과 도착점을 입력한다.  
2. BFS에 의해 최단경로 알고리즘을 구한다.  
3. 카메라를 통해 공을 인식 및 공의 좌표를 인식한다.  
4. Serial 통신을 통해 공이 있어야 하는 좌표에 갈 수 있도록 알맞은 서보 모터 각도 값을 전달한다.
5. 일정 시간동안 원하는 좌표에 공이 위치할 시, 다음 좌표로 Target이 바뀐다.  
6. 도착점에 공이 도착하면 프로젝트 종료된다.  

## 프로젝트 결과

<p align="center"><img src="https://user-images.githubusercontent.com/41863759/100521093-e3dabd80-31e4-11eb-8e9d-2fedb6222d89.JPG" width="500px"></p>  
<p align="center"> 전체적인 프로젝트 모습 </p>  

<p align="center"><img src="https://user-images.githubusercontent.com/41863759/100521096-e806db00-31e4-11eb-8c5b-175907574a32.gif" width="500px"></p>  
<p align="center"> 프로젝트 구현(소프트웨어) </p>  

<p align="center"><img src="https://user-images.githubusercontent.com/41863759/100521099-ec32f880-31e4-11eb-8e53-845ae7eca156.gif" width="500px"></p>  
<p align="center"> 프로젝트 구현(하드웨어) </p>  


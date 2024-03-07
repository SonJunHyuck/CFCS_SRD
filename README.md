# Position-Based Formation Control Scheme for Crowd Simulation using Short Range Destination (SRD)
### 위치 기반 동역학(Position based Dyanmics)을 이용한 군중 대형 제어 연구

## Video
https://www.youtube.com/watch?v=8RGSI7724wI
<p align="center"> <img src="https://github.com/SonJunHyuck/CFCS_SRD/blob/main/images/Old.png" width="600" height="336"> </p>
<p align="center"> :arrow_down::arrow_down::arrow_down::arrow_down::arrow_down::arrow_down::arrow_down::arrow_down: </p>
<p align="center"> <img src="https://github.com/SonJunHyuck/CFCS_SRD/blob/main/images/New.png" width="600" height="336"> </p>

### How to build
그림 설명 : https://sonnysmile.notion.site/Wind-Simulation-ece1ff331abe444eb23320a7842c3d56?pvs=4

(다른 프로젝트지만, 동일한 과정으로 빌드가 가능)

요약
1. Cmake -> Configure -> Visual Studio 17 2022 -> x64
2. Generate
3. Open Project (wind_simulatio.sln)
7. build

## cmake configuration
- Window : Clang 16.0.5 (GNU CLI) for MSCV 17.8.34xxx (Visual Studio Tools 2022 Release - amd64)
- 다운로드 링크 : https://visualstudio.microsoft.com/ko/visual-cpp-build-tools/
  
- Mac : Clang 13.1.6 arm64-apple-darwin21.5.0

## Refactoring (2024. 02)
- 재구성 전의 코드는 old branch에서 볼 수 있습니다.
- 퍼포먼스를 높이기보다, 코드 가독성을 위해 Refactoring을 진행하였습니다.

### Refactoring Details
- 불필요한 코드를 제거하였습니다.
- OpenGL Framework를 개선하였습니다.
- OOP의 취지에 맞게 클래스의 기능을 명확하게 구분하였습니다.
- 더블 포인터로 구성된 변수들을 가능하면, std::vector와 std::unique_ptr로 구성하였습니다.
- GPU 프로그래밍을 위해 설계된 부분을 CPU 중심으로 변경하였습니다. (메모리 절약을 위해)
- 자세한 사항 : https://sonnysmile.notion.site/CFCS_SRD-ed7969d7460044999de683465d66474c?pvs=4


### REF : Original Source of Position based multi-agent simulation
https://github.com/tomerwei/pbd-crowd-sim?tab=readme-ov-file

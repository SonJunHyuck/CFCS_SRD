# Position-Based Formation Control Scheme for Crowd Simulation using Short Range Destination (SRD)
- Position-Based Dynamics를 이용한 군중 대형 제어 연구

## Video
https://www.youtube.com/watch?v=8RGSI7724wI

## cmake configuration
- Window / Linux : GCC
- Mac : Clang 13.1.6 arm64-apple-darwin21.5.0

## Refactoring
- 재구성 전의 코드는 old branch에서 볼 수 있습니다.
- 퍼포먼스를 높이기보다, 코드 가독성을 위해 Refactoring을 진행하였습니다.
### 수정사항
- 불필요한 코드를 제거하였습니다.
- OpenGL Framework를 개선하였습니다.
- OOP의 취지에 맞게 클래스의 기능을 명확하게 구분하였습니다.
- 더블 포인터로 구성된 변수들을 가능하면, std::vector와 std::unique_ptr로 구성하였습니다.
- GPU 프로그래밍을 위해 설계된 부분을 CPU 중심으로 변경하였습니다. (메모리 절약을 위해)
- 자세한 사항 : https://sonnysmile.notion.site/CFCS_SRD-ed7969d7460044999de683465d66474c?pvs=4

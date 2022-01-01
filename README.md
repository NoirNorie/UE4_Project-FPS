# UE4_Project
- Native C++ 구현 중

21.12.15
- TPS 프로젝트 생성
- 기본 이동 바인딩

21.12.16
- 애니메이션과 행동 연결

21.12.17
- 인터페이스로 블루프린트와 플레이어 캐릭터 연결

21.12.18
- 조준 HUD, 조준 애니메이션 연결

21.12.19
- 조준 애니메이션 시 방향 수정
- 에임 오프셋 구현

21.12.20
- 총 발사 시 소리 추가, 애니메이션 추가

21.12.21
- 재장전과 애니메이션 연결
- 재장전 횟수 제한 구현

21.12.22
- 위젯 생성
- 위젯 생성 시 출력되는 에러 수정
- 플레이 맵 생성 중

21.12.23
- 1차 코드 리팩토링
- 코드 내 불필요한 부분 제거

21.12.24
- 캐릭터 외형 임포트 및 애니메이션 입력

21.12.25
- 블랙보드, 비헤이비어트리 생성
- 비헤이비어트리에 에러 존재
- 좀비 에셋, 애니메이션 임포트

21.12.26
- 비헤이비어 트리, 블랙보드 에러 수정
- 비헤이비어 트리, 블랙보드 활성화 확인
- 비헤이비어 트리, 블랙보드, 좀비와 연결 확인
- 좀비의 이동과 정지, 애니메이션 연결

21.12.27
- 비헤이비어 트리, 블랙보드 수정
- 좀비가 캐릭터 추적을 가능하도록 노드를 만들어 구현

21.12.28
- 좀비가 공격, 추적을 가능하도록 노드를 만들어 구현
- 지속적으로 캐릭터를 추적하고 공격하도록 만들었다

21.12.29
- 4종류의 언리얼 델리게이트 학습
- 비헤이비어 트리와 노드들의 수정작업
- 투사체를 만들었지만 직선으로 나아가지 않아 수정이 필요함

21.12.30
- 투사체를 직선으로 나아가도록 수정함
- 중력 값이 0.1, 투사체 속도는 20,000으로 보이는 구간에서는 직선으로 움직이는 것으로 보임
- 투사체가 크로스헤어와 같은 방향을 향하지 않는 문제가 있음
- 비슷하지만 정확히 일치하지 않음, 크로스헤어 중심으로 좌상단을 향하여 발사됨

21.12.31
- 투사체가 총에서 발사되는 것처럼 만들기 위해 총의 스켈레탈 메시에 소켓을 뚫어서 위치를 받아냄
- 총이 움직이는대로 총알이 발사되도록 만듦

22.01.01
- 무기 클래스들을 추가하여 습득하는 무기에 따라 들고있는 무기가 변경되도록 만듦
- 무기 에셋들을 구한다면 추가적으로 구현할 수 있을 것 같음

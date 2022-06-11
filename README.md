# C언어 슈팅 게임
<hr/>

## 목표
<hr/>
절차지향 방식을 사용하여 콘솔 화면에서 움직이는 간단한 슈팅게임을 만든다.<br/>
필요한 Scence을 구성하고 각 Scence에서 실행할 함수에서는 키보드 입력, 로직, 랜더의 순서대로 구성한다. <br/>
게임에 필요한 설정 파일을 구성하여 외부에서 읽어들인다.<br/>
<hr/>

## 구성
<hr/>

+ 커서
  + => : 메뉴 선택
  + <- : 설정 항목 값 감소
  + -> : 설정 항목 값 증가

+ 타이틀 메뉴
  + 게임 시작
  + 게임 설정
  + 게임 종료

+ 설정
  + 스테이지
    + 순서
    + 리스폰 딜레이
    + 오픈 여부      
    + 페이지
      + 총 5개의 페이지
      + 페이지 별 최대 20개 적 오브젝트
      + 적 리스폰 위치 설정

  + 플레이어 오브젝트
    + HP
    + 움직임 딜레이
    + 공격 딜레이
    + 총알 움직임 딜레이
    + 총알 데미지
    + 리스폰 위치

  + 적 오브젝트
    + HP
    + 움직임 딜레이
    + 공격 딜레이
    + 총알 움직임 딜레이
    + 총알 데미지
    + 점수

+ 설정 파일
  + 기본 설정 파일
    + 스테이지 개수
    + 스테이지 파일 이름
    + 플레이어 오브젝트 설정 내용
    + 적 오브젝트 설정 내용

  + 스테이지 파일
    + 페이지 별 적 리스폰 위치

+ 인 게임
  + 설정 로드
    + 스테이지, 플레이어 오브젝트, 적 오브젝트 세팅
    + 페이지 별 적 오브젝트 상대적 리스폰 시간 세팅
  + 스테이지 전환
    + 스테이지 안의 페이지들에 있는 적 오브젝트들의 HP 0
  + 게임 클리어
    + 모든 스테이지 완료
  + 게임 오버
    + 플레이어 오브젝트 HP 0
  + 충돌 처리
    + 충돌 효과 (맞은 부분 ' ', 그 이후 원상 복구)
    + 플레이어 일정 기간 무적 처리
  + 적 오브젝트
    + 움직임 패턴 구성 (오브젝트 별로)
    + 총알 오브젝트 구성 (오브젝트 별로)


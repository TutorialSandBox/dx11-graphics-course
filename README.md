# DX11로 배우는 컴퓨터 그래픽스

> 빈 화면에서 시작해 **디퍼드 렌더링된 텍스처 큐브 + 회전하는 3색 점광원 + ImGui 에디터 + 자유비행 카메라**까지,
> 한 줄씩 직접 쌓아 올리는 1:1 그래픽스 강의입니다.

이 저장소는 **대학교 1학년 수준**(자료구조까지 공부한 사람)을 대상으로 합니다.
DirectX, 선형대수, 그래픽스 이론을 처음 접해도 따라올 수 있도록 **이론 → 수학 → 코드** 순서로 차근차근 설명합니다.

---

## 이 강의의 약속

1. **하나씩.** 한 레슨은 하나의 개념만 다룹니다. 절대 한꺼번에 쏟아붓지 않습니다.
2. **왜부터.** "이 코드를 쳐라"가 아니라 "왜 이게 필요한가 → 수학적으로 무슨 일이 일어나는가 → 코드로는 이렇게 쓴다" 순서입니다.
3. **빌드되는 코드.** 각 레슨은 그 시점까지의 코드가 **실제로 빌드되고 실행되는** 상태입니다.
4. **직접 해보기.** 매 레슨 끝에는 손으로 풀거나 코드로 바꿔볼 연습 문제가 있습니다.

---

## 왜 DX12가 아니라 DX11인가?

이 강의의 최종 결과물은 동일한 디퍼드 렌더러지만, **입문용으로는 DX11이 훨씬 적합**합니다.

| | DX11 (이 강의) | DX12 |
|---|---|---|
| 명령 제출 | `ImmediateContext` 하나 — 그냥 그리면 됨 | 커맨드 리스트/얼로케이터 직접 관리 |
| 동기화 | `Present`가 알아서 | 펜스(Fence)로 손수 |
| 자원 바인딩 | 뷰를 직접 바인딩 | 디스크립터 힙 + 루트 시그니처 |
| GPU↔CPU 상태 | 드라이버가 처리 | 리소스 배리어 직접 |

즉 DX11은 **"그래픽스 개념"에 집중**하게 해주고, DX12는 거기에 **"GPU 자원 관리의 복잡함"**을 얹습니다.
개념을 먼저 DX11로 익히면 나중에 DX12로 넘어가기 훨씬 쉽습니다. (이 저장소의 모태가 된 DX12 엔진이 그 다음 단계입니다.)

---

## 수학 규약 (강의 전체 공통)

처음엔 의미를 몰라도 됩니다. Part 1에서 전부 설명합니다. 여기 적어두는 이유는 **"우리는 이 규약으로 일관되게 간다"**는 약속이기 때문입니다.

- **좌표계: 왼손 좌표계(Left-Handed, LH)** — +X 오른쪽, +Y 위, **+Z 화면 안쪽**.
- **행렬 저장: 행우선(row-major)** — 메모리에 한 행씩 차례로 저장.
- **벡터 규약: 행벡터(row-vector)** — 점에 행렬을 곱할 때 `v * M` 순서. HLSL에서는 `mul(v, M)`.
- 이 세 가지를 합치면 HLSL에서 `row_major`로 선언한 행렬을 **전치(transpose) 없이 그대로 업로드**할 수 있습니다.

---

## 커리큘럼

각 레슨에는 git 태그가 붙어 있어 `git checkout lesson-3.2` 처럼 그 시점의 코드 상태로 이동할 수 있습니다.

### Part 0 — 준비: 큰 그림 그리기
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [0.1](docs/00-foundations/0.1-graphics-pipeline.md) | 그래픽스 파이프라인 한눈에 | "정점이 화면 픽셀이 되기까지"의 전체 흐름 |
| [0.2](docs/00-foundations/0.2-coordinate-systems.md) | 좌표계와 공간들 | 모델→월드→뷰→클립 공간, 왜 행렬이 필요한가 |
| [0.3](docs/00-foundations/0.3-project-setup.md) | 개발 환경과 프로젝트 구조 | VS2022, 빌드, 이 저장소를 쓰는 법 |

### Part 1 — 수학: 직접 만드는 선형대수
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [1.1](docs/01-math/1.1-vectors.md) | 벡터 | 내적·외적의 기하학적 의미, `Vector2/3/4` 구현 |
| [1.2](docs/01-math/1.2-matrices.md) | 행렬 | 행렬 곱, 항등/전치, `Matrix` 구현 |
| [1.3](docs/01-math/1.3-transforms.md) | 변환 행렬 | 이동·회전·스케일, 합성 순서 |
| [1.4](docs/01-math/1.4-view-projection.md) | 뷰·투영·MVP | 카메라 행렬, 원근 투영, 최종 MVP 조립 |

### Part 2 — 창과 입력 (Win32)
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [2.1](docs/02-window/2.1-win32-window.md) | Win32 창 띄우기 | 메시지 루프, WndProc thunk 패턴 |
| [2.2](docs/02-window/2.2-input.md) | 입력 처리 | 키/마우스 facade, 델타, 캡처 |

### Part 3 — 첫 삼각형 (DX11 입문)
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [3.1](docs/03-triangle/3.1-device-swapchain.md) | 디바이스와 화면 클리어 | Device/Context/SwapChain/RTV |
| [3.2](docs/03-triangle/3.2-first-triangle.md) | 첫 삼각형 | 정점버퍼+셰이더+InputLayout |
| [3.3](docs/03-triangle/3.3-constant-buffer.md) | 상수 버퍼로 회전 | 행렬을 GPU로 (`mul(v,M)`) |

### Part 4 — 3D 큐브 (Forward 렌더링)
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [4.1](docs/04-cube/4.1-cube-depth.md) | 큐브와 깊이 버퍼 | 인덱스버퍼, 깊이 테스트, MVP |
| [4.2](docs/04-cube/4.2-texture.md) | 텍스처 입히기 | DDS 로딩, SRV, 샘플러, UV |
| [4.3](docs/04-cube/4.3-lighting.md) | 조명 (램버트) | `N·L`, 법선 변환, 앰비언트 |

### Part 5 — 자유비행 카메라
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [5.1](docs/05-camera/5.1-camera.md) | 자유비행 카메라 | yaw/pitch, 외적축, WASD, 짐벌락 |

### Part 6 — ImGui 에디터
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [6.1](docs/06-imgui/6.1-imgui.md) | ImGui 에디터 | 백엔드 통합, 메시지 후크, 입력 게이팅 |

### Part 7 — 디퍼드 렌더링 (최종 목표)
| 레슨 | 제목 | 무엇을 얻나 |
|---|---|---|
| [7.1](docs/07-deferred/7.1-gbuffer.md) | G-buffer와 지오메트리 패스 | MRT, 풀스크린 삼각형, 자원 hazard |
| [7.2](docs/07-deferred/7.2-point-lights.md) | 점광원과 최종 완성 | 거리 감쇠, 다중 점광원, 회전 3색 광원 |

![최종 결과](docs/07-deferred/images/7.2-point-lights.png)

---

## 📚 함께 보면 좋은 자료

- **[📖 책으로 읽기](book/)** — 모든 레슨(코드+설명)을 한 권으로 엮은 책. `book/MiniEngine-DX11-Book.html`을 더블클릭하면 사이드바 목차·코드 하이라이트가 있는 오프라인 책이 열립니다. (`Ctrl+P`로 PDF 저장 가능)
- **[🧑‍🏫 AI 튜터 프롬프트](TUTOR_PROMPT.md)** — 새 AI 대화에서 이 저장소를 1:1 선생님과 공부하는 법. (학습자용)
- **[게임 수학 빠른 안내](docs/appendix/game-math.md)** — 삼각함수/벡터/행렬이 가물가물하면 여기부터.

---

## 저장소 구조

```
dx11-graphics-course/
├─ README.md            ← 지금 이 파일 (전체 목차)
├─ TUTOR_PROMPT.md      ← AI 1:1 튜터로 공부하는 법
├─ MiniEngine.sln       ← Part 2~7 빌드 (VS2022)
├─ docs/                ← 레슨별 이론 설명 (마크다운 + 스크린샷)
│  ├─ 00-foundations/  01-math/  02-window/  03-triangle/
│  ├─ 04-cube/  05-camera/  06-imgui/  07-deferred/
│  └─ appendix/         ← 게임 수학 부록
├─ src/                 ← 강의가 진행되며 자라는 본체 코드
│  ├─ app/              ← 창, 입력, Application(메인 루프)
│  ├─ core/             ← DX11 디바이스, G-buffer
│  ├─ render/           ← 삼각형/큐브/디퍼드 렌더러, 카메라, 텍스처
│  ├─ editor/           ← ImGui 레이어, 에디터 패널
│  ├─ math/             ← 직접 만든 벡터/행렬 (Part 1)
│  └─ shaders/          ← HLSL (triangle/cube/gbuffer/lighting)
├─ examples/            ← Part 1 수학 콘솔 데모
├─ assets/              ← 텍스처(test.dds)
└─ third_party/imgui/   ← Dear ImGui (vendored)
```

## 시작하기

1. 이 저장소를 받습니다.
2. [Part 0.1](docs/00-foundations/0.1-graphics-pipeline.md)부터 **순서대로** 읽습니다. (수학 Part 1이 토대라 건너뛰지 마세요)
3. 빌드/실행:
   - **Part 1 (수학):** `examples/build_and_run.bat`
   - **Part 2~7 (그래픽스):** `MiniEngine.sln`을 VS2022로 열고 **F5**
4. 혼자 막히면 [AI 튜터 프롬프트](TUTOR_PROMPT.md)로 1:1 도움을 받으세요.

> 막히는 곳이 있으면 그 레슨의 "직접 해보기"와 "자주 막히는 곳"을 먼저 보세요.
> 수학이 약하면 [게임 수학 부록](docs/appendix/game-math.md)부터.

---

## 빌드/실행 검증됨

모든 그래픽스 레슨(Part 3~7)은 실제로 빌드·실행해 화면 출력을 캡처하며 검증했습니다.
각 단계는 `MiniEngine.exe --smoke N`(N프레임 후 종료) / `--capture out.bmp`(렌더 결과 저장)로 확인할 수 있습니다.

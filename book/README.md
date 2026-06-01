# 📖 책으로 읽기

이 강의의 모든 레슨(코드 + 설명)을 **한 권의 책**으로 엮어 둡니다.

| 파일 | 용도 | 여는 법 |
|---|---|---|
| **[MiniEngine-DX11-Book.html](MiniEngine-DX11-Book.html)** | 오프라인 책 (이미지 내장 · 사이드바 목차 · 코드 하이라이트) | 파일을 **더블클릭** → 브라우저에서 열림. 서버 불필요(자체 완결). |
| **[MiniEngine-DX11-Book.md](MiniEngine-DX11-Book.md)** | GitHub에서 한 페이지로 읽기 | GitHub에서 클릭 |

## HTML 책 특징

- 왼쪽 **사이드바 목차**로 Part → 레슨 바로 이동
- 스크린샷·다이어그램이 본문에 함께 (파일 하나에 모두 내장)
- 코드 블록 **문법 하이라이트**
- **PDF로 저장:** 브라우저에서 책을 열고 `Ctrl+P` → "PDF로 저장" (한글 폰트 그대로 출력됨)

## 다시 만들기 (문서를 고친 뒤)

```bash
pip install markdown pygments      # 최초 1회
python book/build_book.py
```

`docs/`의 레슨 문서를 수정하면 이 명령으로 책 두 권(HTML/MD)이 새로 생성됩니다.
읽기 순서·구성은 [`build_book.py`](build_book.py) 상단의 `PARTS`에서 정의합니다.

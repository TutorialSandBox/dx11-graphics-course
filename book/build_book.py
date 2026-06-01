#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
build_book.py — 레슨 문서들을 한 권의 "책"으로 엮는다. (모바일 반응형)
  출력 1: MiniEngine-DX11-Book.html  (자체완결: 이미지 내장 + 사이드바 목차 + 코드 하이라이트 + 모바일 대응)
  출력 2: MiniEngine-DX11-Book.md    (GitHub에서 한 페이지로 읽는 단일 마크다운)

사용:  python book/build_book.py
필요:  pip install markdown pygments
"""
import os, re, base64, mimetypes

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT_DIR = os.path.join(ROOT, "book")

# (파트 제목, [소스 문서 상대경로 ...]) — 읽기 순서
PARTS = [
    ("머리말 · 커리큘럼", ["README.md"]),
    ("Part 0 — 준비: 큰 그림", [
        "docs/00-foundations/0.1-graphics-pipeline.md",
        "docs/00-foundations/0.2-coordinate-systems.md",
        "docs/00-foundations/0.3-project-setup.md"]),
    ("Part 1 — 수학", [
        "docs/01-math/1.1-vectors.md",
        "docs/01-math/1.2-matrices.md",
        "docs/01-math/1.3-transforms.md",
        "docs/01-math/1.4-view-projection.md"]),
    ("Part 2 — 창과 입력", [
        "docs/02-window/2.1-win32-window.md",
        "docs/02-window/2.2-input.md"]),
    ("Part 3 — 첫 삼각형", [
        "docs/03-triangle/3.1-device-swapchain.md",
        "docs/03-triangle/3.2-first-triangle.md",
        "docs/03-triangle/3.3-constant-buffer.md"]),
    ("Part 4 — 3D 큐브", [
        "docs/04-cube/4.1-cube-depth.md",
        "docs/04-cube/4.2-texture.md",
        "docs/04-cube/4.3-lighting.md"]),
    ("Part 5 — 자유비행 카메라", ["docs/05-camera/5.1-camera.md"]),
    ("Part 6 — ImGui 에디터", ["docs/06-imgui/6.1-imgui.md"]),
    ("Part 7 — 디퍼드 렌더링", [
        "docs/07-deferred/7.1-gbuffer.md",
        "docs/07-deferred/7.2-point-lights.md"]),
    ("부록 · 게임 수학", ["docs/appendix/game-math.md"]),
    ("AI 튜터 프롬프트", ["TUTOR_PROMPT.md"]),
]

FENCE = re.compile(r"^\s*```")
HEADING = re.compile(r"^(#{1,6})\s")
DEAD_LINK = re.compile(r"(?<!\!)\[([^\]]+)\]\((?!https?://)(?!#)[^)]+\)")
IMG = re.compile(r"!\[([^\]]*)\]\(([^)]+)\)")


def shift_headings(text):
    out, in_fence = [], False
    for line in text.split("\n"):
        if FENCE.match(line):
            in_fence = not in_fence
            out.append(line); continue
        if not in_fence and HEADING.match(line):
            out.append("#" + line)
        else:
            out.append(line)
    return "\n".join(out)


def embed_images_base64(text, base_dir):
    def repl(m):
        alt, path = m.group(1), m.group(2).strip()
        if path.startswith("http") or path.startswith("data:"):
            return m.group(0)
        fp = os.path.normpath(os.path.join(base_dir, path))
        if not os.path.exists(fp):
            return f"*(이미지 없음: {path})*"
        mime = mimetypes.guess_type(fp)[0] or "image/png"
        with open(fp, "rb") as f:
            b64 = base64.b64encode(f.read()).decode("ascii")
        return f"![{alt}](data:{mime};base64,{b64})"
    return IMG.sub(repl, text)


def rewrite_images_relpath(text, base_dir):
    def repl(m):
        alt, path = m.group(1), m.group(2).strip()
        if path.startswith("http") or path.startswith("data:"):
            return m.group(0)
        fp = os.path.normpath(os.path.join(base_dir, path))
        rel = os.path.relpath(fp, OUT_DIR).replace("\\", "/")
        return f"![{alt}]({rel})"
    return IMG.sub(repl, text)


def neutralize_links(text):
    return DEAD_LINK.sub(r"**\1**", text)


def load_doc(relpath):
    with open(os.path.join(ROOT, relpath), "r", encoding="utf-8") as f:
        return f.read()


def build_markdown(for_html):
    chunks = []
    for part_title, docs in PARTS:
        chunks.append(f"\n\n# {part_title}\n")
        for rel in docs:
            base_dir = os.path.dirname(os.path.join(ROOT, rel))
            text = load_doc(rel)
            text = shift_headings(text)
            text = embed_images_base64(text, base_dir) if for_html else rewrite_images_relpath(text, base_dir)
            text = neutralize_links(text)
            chunks.append(text)
            chunks.append("\n\n---\n")
    return "\n".join(chunks)


CSS = """
:root { --fg:#222; --muted:#666; --accent:#2b6cb0; --bg:#fff; --code-bg:#f6f8fa; --border:#e2e8f0; }
* { box-sizing: border-box; }
html { -webkit-text-size-adjust: 100%; }
body { margin:0; color:var(--fg); background:var(--bg); font-size:16px; line-height:1.75;
  font-family:"Malgun Gothic","맑은 고딕","Apple SD Gothic Neo",system-ui,sans-serif;
  word-break: keep-all; overflow-wrap:anywhere; }

/* 상단바 (모바일 전용) */
#topbar { display:none; position:sticky; top:0; z-index:30; background:var(--accent); color:#fff;
  align-items:center; gap:12px; padding:11px 14px; }
#topbar button { background:none; border:none; color:#fff; font-size:22px; line-height:1; cursor:pointer; padding:0 4px; }
#topbar .t { font-weight:700; font-size:15px; }

#backdrop { display:none; position:fixed; inset:0; background:rgba(0,0,0,.45); z-index:40; }

#sidebar { position:fixed; top:0; left:0; width:300px; height:100vh; overflow-y:auto; z-index:50;
  border-right:1px solid var(--border); padding:20px 16px; background:#fafbfc; font-size:14px;
  -webkit-overflow-scrolling:touch; }
#sidebar h2 { font-size:15px; margin:0 0 10px; color:var(--accent); }
#sidebar .toc ul { list-style:none; padding-left:14px; margin:4px 0; }
#sidebar .toc > ul { padding-left:0; }
#sidebar .toc a { color:#334; text-decoration:none; display:block; padding:4px 0; }
#sidebar .toc a:hover { color:var(--accent); }
#sidebar .toc > ul > li > a { font-weight:700; margin-top:8px; color:#111; }

#content { margin-left:300px; padding:48px 56px; max-width:880px; }
h1 { border-bottom:2px solid var(--accent); padding-bottom:8px; margin-top:64px; line-height:1.35; }
h2 { border-bottom:1px solid var(--border); padding-bottom:5px; margin-top:44px; line-height:1.4; }
h3 { margin-top:30px; }
code { background:var(--code-bg); padding:2px 5px; border-radius:4px; font-size:0.9em;
  font-family:Consolas,"D2Coding",monospace; }
pre { background:var(--code-bg); border:1px solid var(--border); border-radius:8px;
  padding:14px 16px; overflow-x:auto; line-height:1.5; -webkit-overflow-scrolling:touch; }
pre code { background:none; padding:0; white-space:pre; word-break:normal; overflow-wrap:normal; }
img { max-width:100%; height:auto; border-radius:8px; box-shadow:0 1px 6px rgba(0,0,0,.12); display:block; margin:16px auto; }
table { border-collapse:collapse; margin:16px 0; width:100%; font-size:0.95em; }
th,td { border:1px solid var(--border); padding:7px 11px; text-align:left; }
th { background:#f1f5f9; }
blockquote { border-left:4px solid var(--accent); margin:16px 0; padding:6px 16px;
  background:#f7fafc; color:#374; border-radius:0 6px 6px 0; }
details { background:#f7fafc; border:1px solid var(--border); border-radius:8px; padding:10px 14px; margin:14px 0; }
summary { cursor:pointer; font-weight:700; color:var(--accent); }
hr { border:none; border-top:1px dashed var(--border); margin:36px 0; }
a { color:var(--accent); }
.cover { text-align:center; padding:60px 0 30px; }
.cover h1 { border:none; font-size:34px; }
.cover p { color:var(--muted); }

/* ===== 모바일 (폰/좁은 화면) ===== */
@media (max-width: 900px) {
  #topbar { display:flex; }
  #sidebar { transform:translateX(-100%); transition:transform .25s ease; box-shadow:2px 0 14px rgba(0,0,0,.18); }
  body.nav-open #sidebar { transform:translateX(0); }
  body.nav-open #backdrop { display:block; }
  #content { margin-left:0; padding:18px 16px 60px; max-width:100%; }
  h1 { margin-top:34px; font-size:23px; }
  h2 { margin-top:34px; font-size:19px; }
  h3 { font-size:17px; }
  .cover { padding:26px 0 8px; }
  .cover h1 { font-size:25px; }
  pre { font-size:13px; padding:12px; }
  table { font-size:0.88em; }
  /* 넓은 표는 가로 스크롤 */
  #content table { display:block; width:100%; overflow-x:auto; -webkit-overflow-scrolling:touch; }
}

@media print {
  #topbar, #backdrop { display:none !important; }
  #sidebar { display:none; }
  #content { margin:0; max-width:100%; }
}
"""

JS = """
<script>
(function(){
  var b = document.body;
  var btn = document.getElementById('navToggle');
  var bd  = document.getElementById('backdrop');
  var sb  = document.getElementById('sidebar');
  function close(){ b.classList.remove('nav-open'); }
  if (btn) btn.addEventListener('click', function(){ b.classList.toggle('nav-open'); });
  if (bd)  bd.addEventListener('click', close);
  // 목차 링크를 누르면 (모바일) 서랍을 닫는다
  if (sb)  sb.addEventListener('click', function(e){ if (e.target.tagName === 'A') close(); });
  document.addEventListener('keydown', function(e){ if (e.key === 'Escape') close(); });
})();
</script>
"""

HTML_TEMPLATE = """<!DOCTYPE html>
<html lang="ko">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<meta name="color-scheme" content="light">
<title>DX11로 배우는 컴퓨터 그래픽스</title>
<style>__CSS__
__PYGCSS__
</style>
</head>
<body>
<div id="topbar"><button id="navToggle" aria-label="목차 열기">&#9776;</button><span class="t">DX11로 배우는 컴퓨터 그래픽스</span></div>
<div id="backdrop"></div>
<nav id="sidebar"><h2>목차</h2>__TOC__</nav>
<main id="content">
<div class="cover">
  <h1>DX11로 배우는 컴퓨터 그래픽스</h1>
  <p>빈 화면에서 디퍼드 렌더링까지, 한 줄씩 쌓아 올리는 1:1 그래픽스 강의</p>
  <p style="font-size:13px">코드 + 설명을 한 권으로 · Jin02</p>
</div>
__BODY__
</main>
__JS__
</body>
</html>
"""


def main():
    import markdown
    from pygments.formatters import HtmlFormatter

    # --- HTML 책 ---
    md_html = build_markdown(for_html=True).replace("<details>", '<details markdown="1">')
    md = markdown.Markdown(extensions=[
        "extra", "tables", "fenced_code", "codehilite", "toc", "md_in_html",
    ], extension_configs={
        "codehilite": {"guess_lang": False},
        "toc": {"toc_depth": "1-2"},
    })
    body = md.convert(md_html)
    pyg_css = HtmlFormatter().get_style_defs(".codehilite")

    html = (HTML_TEMPLATE
            .replace("__CSS__", CSS)
            .replace("__PYGCSS__", pyg_css)
            .replace("__TOC__", md.toc)
            .replace("__BODY__", body)
            .replace("__JS__", JS))
    out_html = os.path.join(OUT_DIR, "MiniEngine-DX11-Book.html")
    with open(out_html, "w", encoding="utf-8") as f:
        f.write(html)
    print(f"[OK] HTML 책: {out_html}  ({os.path.getsize(out_html)//1024} KB)  (모바일 반응형)")

    # --- 마크다운 책 (GitHub용) ---
    md_book = "# DX11로 배우는 컴퓨터 그래픽스 — 통합본\n\n" \
              "> 모든 레슨을 한 페이지로 엮은 책 버전입니다. (생성: `python book/build_book.py`)\n" \
              + build_markdown(for_html=False)
    out_md = os.path.join(OUT_DIR, "MiniEngine-DX11-Book.md")
    with open(out_md, "w", encoding="utf-8") as f:
        f.write(md_book)
    print(f"[OK] MD 책:   {out_md}  ({os.path.getsize(out_md)//1024} KB)")


if __name__ == "__main__":
    main()

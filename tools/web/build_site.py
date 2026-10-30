"""Genera el sitio de RMP Robotics (GitHub Pages) a partir de tools/web/src.

    python tools/web/build_site.py

- src/index.html          -> index.html (portada)
- src/robots/<id>.html    -> robots/<id>/index.html (ficha de cada robot)

Cada ficha empieza con un comentario HTML que contiene un JSON con los datos del
hero y de las cifras; el resto del archivo son las secciones del informe. El
índice lateral se arma solo a partir de los <h2 id> y <h3 id>.

Solo usa la biblioteca estándar de Python.
"""

import html
import json
import re
from pathlib import Path

from drawings import DRAWINGS

ROOT = Path(__file__).resolve().parents[2]
SRC = Path(__file__).resolve().parent / 'src'

# Orden de las fichas (para los enlaces anterior / siguiente).
ORDER = ['deimonyag', 'unicum', 'lea-paredes', 'aus-kim']

FONTS = ('https://fonts.googleapis.com/css2?family=Chakra+Petch:wght@500;600;700'
         '&family=IBM+Plex+Mono:wght@400;500&family=IBM+Plex+Sans:wght@400;500;600&display=swap')
THREE = 'https://cdn.jsdelivr.net/npm/three@0.160.0/'


def with_drawings(text):
    return re.sub(r'\{\{svg:([\w-]+)\}\}', lambda m: DRAWINGS[m.group(1)], text)


def read_robot(robot_id):
    raw = (SRC / 'robots' / f'{robot_id}.html').read_text(encoding='utf-8')
    match = re.match(r'\s*<!--(.*?)-->', raw, re.S)
    meta = json.loads(match.group(1))
    return meta, raw[match.end():].strip()


def outline(body):
    """Índice de dos niveles a partir de los encabezados con id."""
    items = []
    pattern = r'<section id="([\w-]+)">\s*<h2>(.*?)</h2>|<h3 id="([\w-]+)">(.*?)</h3>'
    for sec_id, sec_label, sub_id, sub_label in re.findall(pattern, body):
        level = '2' if sec_id else '3'
        anchor = sec_id or sub_id
        label = re.sub(r'<[^>]+>', '', sec_label or sub_label)
        if level == '2':
            items.append([anchor, label, []])
        elif items:
            items[-1][2].append((anchor, label))
    out = []
    for anchor, label, children in items:
        sub = ''.join(f'<li><a href="#{a}">{l}</a></li>' for a, l in children)
        sub = f'<ol>{sub}</ol>' if sub else ''
        out.append(f'<li><a href="#{anchor}">{label}</a>{sub}</li>')
    return '<ol>' + ''.join(out) + '</ol>'


def hero_art(meta):
    art = meta['art']
    if 'stl' in art:
        parts = [{'src': f'../../assets/models/{src}', 'color': color} for src, color in art['stl']]
        data = html.escape(json.dumps(parts), quote=True)
        return (f'<div class="viewer" data-stl="{data}" aria-label="{art["label"]}">'
                '<span class="viewer-msg">Cargando modelo 3D…</span></div>'
                '<span class="hint">Arrastrá para girar el modelo</span>')
    if 'svg' in art:
        return DRAWINGS[art['svg']].replace('class="draw"', 'class="draw draw-hero"', 1)
    return ''


def figures(meta):
    cells = ''.join(f'<div class="figure"><dt>{label}</dt><dd>{value}<small>{unit}</small></dd></div>'
                    for label, value, unit in meta['figures'])
    return f'<dl class="figures">{cells}</dl>'


def side_media(meta):
    side = meta.get('side')
    if not side:
        return ''
    fit = ' class="contain"' if side.get('contain') else ''
    return (f'<figure class="side-media"><img src="../../assets/images/{side["img"]}" alt="{side["alt"]}"{fit} loading="lazy">'
            f'<figcaption>{side["caption"]}</figcaption></figure>')


def robot_page(meta, body, prev_meta, next_meta):
    uses_stl = 'stl' in meta['art']
    importmap = (f'<script type="importmap">{{"imports":{{"three":"{THREE}build/three.module.js",'
                 f'"three/addons/":"{THREE}examples/jsm/"}}}}</script>') if uses_stl else ''
    viewer_js = '<script type="module" src="../../assets/js/stl-viewer.js"></script>' if uses_stl else ''
    extra_links = ''.join(f'<a class="btn" href="{href}" target="_blank" rel="noreferrer">{label}</a>'
                          for label, href in meta.get('links', []))
    return f'''<!doctype html>
<html lang="es">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="theme-color" content="#000000">
  <meta name="description" content="{meta['name']}: {meta['title_suffix']}">
  <title>{meta['name']} — RMP Robotics</title>
  <link rel="icon" href="../../assets/images/rmp-logo.webp">
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="{FONTS}" rel="stylesheet">
  <link rel="stylesheet" href="../../assets/css/styles.css">
  {importmap}
</head>
<body>
  <a class="skip" href="#informe">Ir al informe</a>

  <header class="bar">
    <a href="../../"><img src="../../assets/images/rmp-logo.webp" alt="" width="26" height="26"> Robots</a>
    <span class="bar-title">{meta['name']}</span>
    <a class="gh" href="{meta['repo']}" target="_blank" rel="noreferrer">Repositorio</a>
  </header>

  <section class="hero">
    <div class="hero-copy">
      <p class="hero-kicker">{meta['kicker']}</p>
      <h1 style="--chars: {max(len(meta['name']), 6)}">{meta['name']}<span class="ver">{meta['version']}</span></h1>
      <p class="hero-lead">{meta['lead']}</p>
      <div class="hero-actions">
        <a class="btn btn-solid" href="#informe">Leer el informe</a>
        <a class="btn" href="{meta['repo']}" target="_blank" rel="noreferrer">Ver en GitHub</a>
        {extra_links}
      </div>
    </div>
    <div class="hero-art">{hero_art(meta)}</div>
  </section>

  {figures(meta)}

  <div class="report" id="informe">
    <aside class="side">
      {side_media(meta)}
      <nav class="outline" aria-label="Índice del informe">
        <p>Índice</p>
        {outline(body)}
      </nav>
    </aside>

    <article class="doc">
{body}

      <footer class="doc-foot">
        <a href="../{prev_meta['id']}/">← {prev_meta['name']}</a>
        <a href="../../">Todos los robots</a>
        <a href="../{next_meta['id']}/">{next_meta['name']} →</a>
      </footer>
    </article>
  </div>

  <script src="../../assets/js/robot.js" defer></script>
  {viewer_js}
</body>
</html>
'''


def main():
    (ROOT / 'index.html').write_text(with_drawings((SRC / 'index.html').read_text(encoding='utf-8')), encoding='utf-8')
    print('index.html')

    robots = [read_robot(robot_id) for robot_id in ORDER]
    for i, (meta, body) in enumerate(robots):
        prev_meta = robots[i - 1][0]
        next_meta = robots[(i + 1) % len(robots)][0]
        out = ROOT / 'robots' / meta['id'] / 'index.html'
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(robot_page(meta, with_drawings(body), prev_meta, next_meta), encoding='utf-8')
        print(out.relative_to(ROOT).as_posix())


if __name__ == '__main__':
    main()

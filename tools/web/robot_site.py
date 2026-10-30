"""Sitio propio de cada robot: vitrina estilo Atlas + el mismo informe de la ficha.

Lo usa build_site.py. Las páginas viven en el repositorio de cada robot
(srampello.github.io/<repo>/) y toman estilos, scripts y modelos 3D del sitio
central (srampello.github.io/RMP/), que comparte dominio.
"""

import json
import re

CENTRAL = '/RMP/'


def read_site(src, robot_id):
    raw = (src / 'sites' / f'{robot_id}.html').read_text(encoding='utf-8')
    match = re.match(r'\s*<!--(.*?)-->', raw, re.S)
    return json.loads(match.group(1)), raw[match.end():].strip()


def with_viewers(text, viewer):
    """{{stl:pieza.stl#color,otra.stl#color|Etiqueta}} -> visor 3D con modelos del sitio central."""
    def make(match):
        spec, label = match.group(1).split('|')
        parts = [item.split('#') for item in spec.split(',')]
        return viewer([(src, '#' + color) for src, color in parts], label, CENTRAL)
    return re.sub(r'\{\{stl:([^}]+)\}\}', make, text)


def site_links(body, sites):
    """En el informe, los enlaces entre robots apuntan a sus sitios y no a las fichas."""
    for robot_id, site in sites.items():
        body = body.replace(f'href="../{robot_id}/"', f'href="{site["url"]}"')
    return body.replace('href="../../"', f'href="{CENTRAL}"')


def robot_site(meta, site, body, showcase, sites, parts):
    """parts: dict con las funciones compartidas de build_site (hero_art, side_media, outline, fonts, three)."""
    order = list(sites)
    i = order.index(meta['id'])
    prev_site, next_site = sites[order[i - 1]], sites[order[(i + 1) % len(order)]]
    three = parts['three']
    uses_stl = 'stl' in meta['art'] or 'data-stl' in showcase
    importmap = (f'<script type="importmap">{{"imports":{{"three":"{three}build/three.module.js",'
                 f'"three/addons/":"{three}examples/jsm/"}}}}</script>') if uses_stl else ''
    viewer_js = f'<script type="module" src="{CENTRAL}assets/js/stl-viewer.js"></script>' if uses_stl else ''
    nav = ''.join(f'<a href="{href}">{label}</a>' for label, href in site.get('nav', []))
    first = re.search(r'<section class="feat[^"]*" id="([\w-]+)"', showcase).group(1)
    actions = (f'<div class="hero-actions"><a class="btn btn-solid" href="#{first}">Conocer el robot</a>'
               f'<a class="btn" href="#informe">Leer el informe</a></div>')

    if 'logo' in site:
        hero = (f'<section class="v-hero"><h1 class="sr-only">{meta["name"]}</h1>'
                f'<img class="logo" src="{site["logo"]}" alt="{site["logo_alt"]}">'
                f'<p>{site["tagline"]}</p>{actions}</section>')
    else:
        hero = (f'<section class="hero"><div class="hero-copy">'
                f'<p class="hero-kicker">{meta["kicker"]}</p>'
                f'<h1 style="--chars: {max(len(meta["name"]), 6)}">{meta["name"]}<span class="ver">{meta["version"]}</span></h1>'
                f'<p class="hero-lead">{site["tagline"]}</p>{actions}</div>'
                f'<div class="hero-art">{parts["hero_art"](meta, CENTRAL)}</div></section>')

    others = ''.join(f'<a href="{sites[r]["url"]}">{sites[r]["name"]}</a>' for r in order if r != meta['id'])
    statement = site['statement']
    return f'''<!doctype html>
<html lang="es">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="theme-color" content="#000000">
  <meta name="description" content="{meta['name']}: {meta['title_suffix']}">
  <title>{meta['name']} — RMP Robotics</title>
  <link rel="icon" href="{CENTRAL}assets/images/rmp-logo.webp">
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="{parts['fonts']}" rel="stylesheet">
  <link rel="stylesheet" href="{CENTRAL}assets/css/styles.css">
  <style>.sr-only{{position:absolute;width:1px;height:1px;overflow:hidden;clip:rect(0 0 0 0);white-space:nowrap}}</style>
  {importmap}
</head>
<body>
  <a class="skip" href="#informe">Ir al informe</a>

  <header class="bar">
    <a href="{CENTRAL}"><img src="{CENTRAL}assets/images/rmp-logo.webp" alt="" width="26" height="26"> RMP Robotics</a>
    <span class="bar-title">{meta['name']}</span>
    <nav class="bar-nav" aria-label="Secciones">
      <a href="#informe">Informe técnico</a>
      {nav}
      <a class="gh" href="{meta['repo']}" target="_blank" rel="noreferrer">GitHub</a>
    </nav>
  </header>

  {hero}

  <section class="statement">
    <h2>{statement['title']}</h2>
    <p>{statement['text']}</p>
  </section>

{showcase}

  <div class="report-title">
    <h2>Informe técnico</h2>
    <p>Hardware, pinout, firmware y estado del proyecto. También está en la <a href="{CENTRAL}robots/{meta['id']}/">ficha de RMP Robotics</a>.</p>
  </div>

  <div class="report" id="informe">
    <aside class="side">
      {parts['side_media'](meta, CENTRAL)}
      <nav class="outline" aria-label="Índice del informe">
        <p>Índice</p>
        {parts['outline'](body)}
      </nav>
    </aside>

    <article class="doc">
{body}

      <footer class="doc-foot">
        <a href="{prev_site['url']}">← {prev_site['name']}</a>
        <a href="{CENTRAL}">Todos los robots</a>
        <a href="{next_site['url']}">{next_site['name']} →</a>
      </footer>
    </article>
  </div>

  <footer class="site-foot robot-foot">
    <span>{meta['name']} es un proyecto de <a href="{CENTRAL}">RMP Robotics</a></span>
    <nav aria-label="Otros robots">{others}</nav>
  </footer>

  <script src="{CENTRAL}assets/js/robot.js" defer></script>
  {viewer_js}
</body>
</html>
'''

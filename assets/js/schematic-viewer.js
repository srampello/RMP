// Visor de esquemáticos: pestañas, zoom con rueda, pellizco o teclado, y arrastre.
// Marcado esperado:
//   <div class="sch" data-sch>
//     <div role="tablist"> <button role="tab" data-src="..." data-label="..."> ... </div>
//     <div class="sch-stage" tabindex="0"><img class="sch-sheet" alt=""></div>
//     <button data-zoom="in|out|fit"> ... <output data-zoom-level></output>
//     <p data-sch-status></p>
//   </div>

document.querySelectorAll('[data-sch]').forEach(root => {
  const stage = root.querySelector('.sch-stage');
  const sheet = root.querySelector('.sch-sheet');
  const tabs = [...root.querySelectorAll('[role="tab"]')];
  const level = root.querySelector('[data-zoom-level]');
  const status = root.querySelector('[data-sch-status]');
  const MIN = 0.2, MAX = 12;

  let scale = 1, x = 0, y = 0, fitScale = 1;
  const pointers = new Map();
  let pinch = null;

  const apply = () => {
    sheet.style.transform = `translate(${x}px, ${y}px) scale(${scale})`;
    if (level) level.textContent = `${Math.round((scale / fitScale) * 100)} %`;
  };

  const fit = () => {
    const w = sheet.naturalWidth, h = sheet.naturalHeight;
    if (!w || !h) return;
    const pad = 24;
    fitScale = Math.min((stage.clientWidth - pad * 2) / w, (stage.clientHeight - pad * 2) / h);
    scale = fitScale;
    x = (stage.clientWidth - w * scale) / 2;
    y = (stage.clientHeight - h * scale) / 2;
    apply();
  };

  // Zoom manteniendo fijo el punto (px, py) del visor.
  const zoomAt = (factor, px = stage.clientWidth / 2, py = stage.clientHeight / 2) => {
    const next = Math.min(MAX, Math.max(MIN, scale * factor));
    const k = next / scale;
    x = px - (px - x) * k;
    y = py - (py - y) * k;
    scale = next;
    apply();
  };

  const show = tab => {
    tabs.forEach(t => {
      const on = t === tab;
      t.setAttribute('aria-selected', on);
      t.tabIndex = on ? 0 : -1;
    });
    status.textContent = 'Cargando esquemático…';
    sheet.alt = tab.dataset.label;
    sheet.src = tab.dataset.src;
  };

  sheet.addEventListener('load', () => {
    fit();
    status.textContent = 'Rueda o pellizco para acercar, arrastrar para mover, doble clic para encuadrar.';
  });
  sheet.addEventListener('error', () => {
    status.textContent = 'No se pudo cargar el esquemático. Probá abrirlo desde el enlace de abajo.';
  });

  tabs.forEach((tab, i) => {
    tab.addEventListener('click', () => show(tab));
    tab.addEventListener('keydown', e => {
      const step = { ArrowRight: 1, ArrowLeft: -1 }[e.key];
      if (!step) return;
      const next = tabs[(i + step + tabs.length) % tabs.length];
      next.focus();
      show(next);
    });
  });

  root.querySelectorAll('[data-zoom]').forEach(btn => btn.addEventListener('click', () => {
    const action = btn.dataset.zoom;
    if (action === 'fit') fit();
    else zoomAt(action === 'in' ? 1.25 : 0.8);
  }));

  stage.addEventListener('wheel', e => {
    e.preventDefault();
    const r = stage.getBoundingClientRect();
    zoomAt(e.deltaY < 0 ? 1.12 : 0.89, e.clientX - r.left, e.clientY - r.top);
  }, { passive: false });

  stage.addEventListener('dblclick', fit);

  stage.addEventListener('keydown', e => {
    const moves = { ArrowLeft: [60, 0], ArrowRight: [-60, 0], ArrowUp: [0, 60], ArrowDown: [0, -60] };
    if (e.key === '+' || e.key === '=') zoomAt(1.25);
    else if (e.key === '-') zoomAt(0.8);
    else if (e.key === '0') fit();
    else if (moves[e.key]) { x += moves[e.key][0]; y += moves[e.key][1]; apply(); }
    else return;
    e.preventDefault();
  });

  stage.addEventListener('pointerdown', e => {
    stage.setPointerCapture(e.pointerId);
    pointers.set(e.pointerId, { x: e.clientX, y: e.clientY });
    stage.classList.add('dragging');
    if (pointers.size === 2) {
      const [a, b] = [...pointers.values()];
      pinch = Math.hypot(a.x - b.x, a.y - b.y);
    }
  });

  stage.addEventListener('pointermove', e => {
    const prev = pointers.get(e.pointerId);
    if (!prev) return;
    pointers.set(e.pointerId, { x: e.clientX, y: e.clientY });
    if (pointers.size === 2 && pinch) {
      const [a, b] = [...pointers.values()];
      const dist = Math.hypot(a.x - b.x, a.y - b.y);
      const r = stage.getBoundingClientRect();
      zoomAt(dist / pinch, (a.x + b.x) / 2 - r.left, (a.y + b.y) / 2 - r.top);
      pinch = dist;
    } else if (pointers.size === 1) {
      x += e.clientX - prev.x;
      y += e.clientY - prev.y;
      apply();
    }
  });

  const release = e => {
    pointers.delete(e.pointerId);
    if (pointers.size < 2) pinch = null;
    if (!pointers.size) stage.classList.remove('dragging');
  };
  stage.addEventListener('pointerup', release);
  stage.addEventListener('pointercancel', release);

  new ResizeObserver(() => { if (sheet.complete && sheet.naturalWidth) fit(); }).observe(stage);

  show(tabs.find(t => t.getAttribute('aria-selected') === 'true') || tabs[0]);
});

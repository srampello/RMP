// Visor 3D de STL para las fichas de robot.
// Uso: <div class="viewer" data-stl='[{"src":"...stl","color":"#3a3a42"}]'></div>
// Las piezas ya vienen alineadas entre sí (mismo origen de coordenadas que en el CAD).

import * as THREE from 'three';
import { OrbitControls } from 'three/addons/controls/OrbitControls.js';

const reduceMotion = window.matchMedia('(prefers-reduced-motion: reduce)').matches;

function parseBinarySTL(buffer) {
  const view = new DataView(buffer);
  const triangles = view.getUint32(80, true);
  const positions = new Float32Array(triangles * 9);
  let offset = 84;
  for (let t = 0; t < triangles; t++) {
    offset += 12; // la normal se recalcula
    for (let v = 0; v < 9; v++) {
      positions[t * 9 + v] = view.getFloat32(offset, true);
      offset += 4;
    }
    offset += 2;
  }
  const geometry = new THREE.BufferGeometry();
  geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
  geometry.computeVertexNormals();
  return geometry;
}

async function mount(el) {
  const parts = JSON.parse(el.dataset.stl);
  const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true });
  renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
  el.appendChild(renderer.domElement);

  const scene = new THREE.Scene();
  const camera = new THREE.PerspectiveCamera(32, 1, 1, 5000);
  scene.add(new THREE.HemisphereLight(0xffffff, 0x1a1626, 1.6));
  const key = new THREE.DirectionalLight(0xffffff, 2.2);
  key.position.set(120, 220, 160);
  scene.add(key);
  const rim = new THREE.DirectionalLight(0xa77cff, 1.4);
  rim.position.set(-160, 80, -200);
  scene.add(rim);

  const group = new THREE.Group();
  scene.add(group);

  const geometries = await Promise.all(parts.map(async part => {
    const response = await fetch(part.src);
    if (!response.ok) throw new Error(`No se pudo cargar ${part.src}`);
    return parseBinarySTL(await response.arrayBuffer());
  }));

  geometries.forEach((geometry, i) => {
    const color = new THREE.Color(parts[i].color || '#3a3a42');
    group.add(new THREE.Mesh(geometry, new THREE.MeshStandardMaterial({ color, roughness: .62, metalness: .08 })));
    const edges = new THREE.EdgesGeometry(geometry, 28);
    group.add(new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color: 0x8a8597, transparent: true, opacity: .55 })));
  });

  // centrar y encuadrar
  const box = new THREE.Box3().setFromObject(group);
  const center = box.getCenter(new THREE.Vector3());
  const size = box.getSize(new THREE.Vector3()).length();
  group.position.sub(center);
  camera.position.set(size * .95, size * .8, size * 1.15);
  camera.lookAt(0, 0, 0);

  const controls = new OrbitControls(camera, renderer.domElement);
  controls.enableDamping = true;
  controls.enablePan = false;
  controls.minDistance = size * .7;
  controls.maxDistance = size * 3;
  controls.autoRotate = !reduceMotion;
  controls.autoRotateSpeed = 1.1;
  renderer.domElement.addEventListener('pointerdown', () => { controls.autoRotate = false; });

  const resize = () => {
    const { clientWidth: w, clientHeight: h } = el;
    renderer.setSize(w, h, false);
    camera.aspect = w / h;
    camera.updateProjectionMatrix();
  };
  new ResizeObserver(resize).observe(el);
  resize();

  let visible = true;
  new IntersectionObserver(([entry]) => { visible = entry.isIntersecting; }).observe(el);
  renderer.setAnimationLoop(() => {
    if (!visible) return;
    controls.update();
    renderer.render(scene, camera);
  });
  el.querySelector('.viewer-msg')?.remove();
}

document.querySelectorAll('.viewer[data-stl]').forEach(el => {
  mount(el).catch(error => {
    const msg = el.querySelector('.viewer-msg');
    if (msg) msg.textContent = 'No se pudo cargar el modelo 3D.';
    console.error(error);
  });
});

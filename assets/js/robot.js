// Índice lateral: resalta la sección y la subsección que se están leyendo.
const links = [...document.querySelectorAll('.outline a[href^="#"]')];
const items = links
  .map(link => ({ link, target: document.getElementById(link.getAttribute('href').slice(1)) }))
  .filter(item => item.target);

let ticking = false;
const update = () => {
  ticking = false;
  const line = 130;
  let current = items[0];
  for (const item of items) {
    if (item.target.getBoundingClientRect().top <= line) current = item;
  }
  links.forEach(link => link.classList.remove('active', 'parent'));
  if (!current) return;
  current.link.classList.add('active');
  const parent = current.link.parentElement.parentElement.closest('li')?.querySelector(':scope > a');
  if (parent) parent.classList.add('parent');
};

window.addEventListener('scroll', () => {
  if (!ticking) { ticking = true; requestAnimationFrame(update); }
}, { passive: true });
update();

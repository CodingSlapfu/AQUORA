/* AQUORA v0.6.5: automatic, control-free homepage screen rotation. */
(() => {
  "use strict";
  const screen = document.getElementById("hero-screen-image");
  if (!screen) return;
  const slides = [
    ["play-065.png", "PLAY page with performance controls"],
    ["sound-065.png", "SOUND editor page"],
    ["evolve-065.png", "EVOLVE arrangement page"],
    ["mixer-065.png", "Eight-track mixer page"],
    ["mix-strip-065.png", "Mixer channel controls"],
    ["drums-065.png", "Drum-kit controls"],
    ["air-065.png", "AIR effects settings"],
    ["appearance-065.png", "Display appearance settings"]
  ];
  const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)");
  let index = 0;
  let timer = null;
  let switching = false;
  const animated = new Set(["sound", "evolve", "mixer", "mix-strip"]);
  const imagePath = n => "assets/" + slides[n][0].replace(/\.png$/, !reducedMotion.matches && animated.has(slides[n][0].replace("-065.png", "")) ? ".apng" : ".png");
  function syncGallery() {
    document.querySelectorAll("img[data-animated]").forEach(img => {
      if (!img.dataset.still) img.dataset.still = img.getAttribute("src");
      img.src = reducedMotion.matches ? img.dataset.still : img.dataset.animated;
      img.onerror = () => { img.onerror = null; img.src = img.dataset.still; };
    });
    screen.src = imagePath(index);
    screen.onerror = () => { screen.onerror = null; screen.src = "assets/" + slides[index][0]; };
  }
  reducedMotion.addEventListener("change", syncGallery);
  syncGallery();
  function preload(n) {
    const image = new Image();
    image.src = imagePath(n);
  }
  function advance() {
    if (document.hidden || switching) return;
    switching = true;
    if (!reducedMotion.matches) screen.classList.add("is-switching");
    window.setTimeout(() => {
      index = (index + 1) % slides.length;
      screen.src = imagePath(index);
      screen.alt = "Actual AQUORA v0.6.5 " + slides[index][1] + ".";
      preload((index + 1) % slides.length);
      window.requestAnimationFrame(() => {
        screen.classList.remove("is-switching");
        switching = false;
      });
    }, reducedMotion.matches ? 0 : 220);
  }
  function syncTimer() {
    if (timer !== null) window.clearInterval(timer);
    timer = document.hidden ? null : window.setInterval(advance, 4000);
  }
  preload(1);
  document.addEventListener("visibilitychange", syncTimer);
  syncTimer();
})();

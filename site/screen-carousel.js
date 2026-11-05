/* AQUORA v0.6.3: automatic, control-free homepage screen rotation. */
(() => {
  "use strict";
  const screen = document.getElementById("hero-screen-image");
  if (!screen) return;
  const slides = [
    ["play-063.png", "PLAY page with performance controls"],
    ["sound-063.png", "SOUND editor page"],
    ["evolve-063.png", "EVOLVE arrangement page"],
    ["mixer-063.png", "Eight-track mixer page"],
    ["mix-strip-063.png", "Mixer channel controls"],
    ["drums-063.png", "Drum-kit controls"],
    ["air-063.png", "AIR effects settings"],
    ["appearance-063.png", "Display appearance settings"],
    ["original-063.png", "Original visualizer screen"],
    ["polar-063.png", "Polar Bloom visualizer screen"]
  ];
  const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)");
  let index = 0;
  let timer = null;
  let switching = false;
  const imagePath = n => "assets/" + slides[n][0];
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
      screen.alt = "Actual AQUORA v0.6.3 " + slides[index][1] + ".";
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

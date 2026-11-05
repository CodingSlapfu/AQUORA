/* AQUORA v0.6.2: automatic, control-free homepage screen rotation. */
(() => {
  "use strict";
  const screen = document.getElementById("hero-screen-image");
  if (!screen) return;
  const slides = [
    ["play-062.png", "PLAY page with performance controls"],
    ["sound-062.png", "SOUND editor page"],
    ["evolve-062.png", "EVOLVE arrangement page"],
    ["mixer-062.png", "Eight-track mixer page"],
    ["mix-strip-062.png", "Mixer channel controls"],
    ["drums-062.png", "Drum-kit controls"],
    ["air-062.png", "AIR effects settings"],
    ["appearance-062.png", "Display appearance settings"],
    ["original-062.png", "Original visualizer screen"],
    ["polar-062.png", "Polar Bloom visualizer screen"]
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
      screen.alt = "Actual AQUORA v0.6.2 " + slides[index][1] + ".";
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

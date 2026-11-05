/* AQUORA public showcase: approved v0.6.2 still-screen tour (no device rendering). */
(() => {
  "use strict";
  const root = document.getElementById("hero-screen-carousel");
  const screen = document.getElementById("hero-screen-image");
  const toolbar = document.getElementById("hero-screen-toolbar");
  const name = document.getElementById("hero-screen-name");
  const counter = document.getElementById("hero-screen-count");
  const previous = document.getElementById("hero-screen-previous");
  const next = document.getElementById("hero-screen-next");
  const pause = document.getElementById("hero-screen-pause");
  if (!root || !screen || !toolbar || !name || !counter || !previous || !next || !pause) return;

  const slides = [
    ["play-062.png", "PLAY", "PLAY page with performance controls"],
    ["sound-062.png", "SOUND editor", "SOUND editor page"],
    ["evolve-062.png", "EVOLVE", "EVOLVE arrangement page"],
    ["mixer-062.png", "Eight-track mixer", "Eight-track mixer page"],
    ["mix-strip-062.png", "Focused channel", "Mixer channel controls"],
    ["drums-062.png", "DRUMS", "Drum-kit controls"],
    ["air-062.png", "AIR FX", "AIR effects settings"],
    ["appearance-062.png", "Appearance", "Display appearance settings"],
    ["original-062.png", "Original Flow", "Original visualizer screen"],
    ["polar-062.png", "Polar Bloom", "Polar Bloom visualizer screen"]
  ];
  const reducedMotion = window.matchMedia("(prefers-reduced-motion: reduce)");
  let index = 0;
  let timer = null;
  let switching = false;
  let paused = reducedMotion.matches;
  const imagePath = (n) => "assets/" + slides[(n + slides.length) % slides.length][0];

  function preload(n) {
    const image = new Image();
    image.src = imagePath(n);
  }

  function render(n) {
    index = n;
    screen.src = imagePath(n);
    screen.alt = "Actual AQUORA v0.6.2 " + slides[n][2] + ".";
    name.textContent = slides[n][1];
    counter.textContent = String(n + 1).padStart(2, "0") + " / " + slides.length;
    preload(n + 1);
  }

  function move(step) {
    if (switching) return;
    const destination = (index + step + slides.length) % slides.length;
    switching = true;
    if (!reducedMotion.matches) screen.classList.add("is-switching");
    window.setTimeout(() => {
      render(destination);
      window.requestAnimationFrame(() => {
        screen.classList.remove("is-switching");
        switching = false;
      });
    }, reducedMotion.matches ? 0 : 220);
  }

  function schedule() {
    if (timer !== null) window.clearInterval(timer);
    timer = null;
    if (paused || document.hidden || root.matches(":hover") || root.matches(":focus-within")) return;
    timer = window.setInterval(() => move(1), 4500);
  }

  function updatePauseButton() {
    pause.textContent = paused ? "Play" : "Pause";
    pause.setAttribute("aria-label", paused ? "Resume automatic screen rotation" : "Pause automatic screen rotation");
    pause.setAttribute("aria-pressed", String(paused));
  }

  previous.addEventListener("click", () => { move(-1); schedule(); });
  next.addEventListener("click", () => { move(1); schedule(); });
  pause.addEventListener("click", () => { paused = !paused; updatePauseButton(); schedule(); });
  root.addEventListener("mouseenter", schedule);
  root.addEventListener("mouseleave", schedule);
  root.addEventListener("focusin", schedule);
  root.addEventListener("focusout", () => window.requestAnimationFrame(schedule));
  document.addEventListener("visibilitychange", schedule);
  if (typeof reducedMotion.addEventListener === "function") {
    reducedMotion.addEventListener("change", (event) => {
      if (event.matches) paused = true;
      updatePauseButton();
      schedule();
    });
  }

  render(0);
  updatePauseButton();
  toolbar.hidden = false;
  schedule();
})();

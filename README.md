



<br />
<div align="center">
  <a href="https://github.com/IASXP/AirportVisuals">
    <img src="https://img.icons8.com/?size=128&id=dr5aRtKymdbz&format=png" alt="Logo"/>
  </a>

<h3 align="center">AirportVisuals</h3>

  <p align="center">
    Basic standalone gtk app that utilizes cairo and the x-plane apt.dat to display an airport map given an ICAO code. Plans to add SID/STAR viewing capabilities along with frequency lists. To be implemented as a X-Plane app (closed-source, payware).
    <br />
    <br />
    <b>ALPHA 0.2, STILL NEEDS A LOT OF WORK<b>
      <br />
    <br />
    <a ><img align="center" width="500" src="https://i.ibb.co/94KLjmb/Screenshot-2024-05-29-at-11-15-06-PM.png"></a>
    <br />
    <br />
    <br />
    ·
    <a href="https://github.com/IASXP/AirportVisuals/issues">Report Bug</a>
    ·
    <a href="https://github.com/IASXP/AirportVisuals/issues">Request Feature</a>
  </p>
</div>


<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#frameworks">Frameworks</a></li>
        <li><a href="#includes">Includes</a></li>
      </ul>
    </li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>

  </ol>
</details>


<!-- ABOUT THE PROJECT -->

## About The Project

### Built With

- [Cairo](https://www.cairographics.org/)
- [GTK](https://www.gtk.org/)

<p align="right">(<a href="#top">back to top</a>)</p>


<!-- GETTING STARTED -->

## Getting Started

Currently this project is only tested on mac using X-Code

### Frameworks

1. libcairo.dylib
2. libgobject.dylib
3. libgtk.dylib

### Includes

1. gtk
2. cairo
3. glib
4. pango
5. harfbuzz
6. gdk-pixbuf
7. atk

```gcc main.c drawing.c parser.c -o nd_display `pkg-config --cflags --libs gtk+-3.0 cairo` && ./nd_display```

<p align="right">(<a href="#top">back to top</a>)</p>



<p align="right">(<a href="#top">back to top</a>)</p>

## Contact

Cactus2456 - Discord & X-Plane.org fourms







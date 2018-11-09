VLD3D11 (This project is still in progress)
========================

* Tested on: Windows 10, i7-7700K @ 4.20GHz 16.0GB, GTX 1080 15.96GB

* Based on GPU Pro 5: Volumetric Light Effects in Killzone: Shadow Fall and [NVDIA Volumetric Lighting Slides](https://developer.nvidia.com/sites/default/files/akamai/gameworks/downloads/papers/NVVL/Fast_Flexible_Physically-Based_Volumetric_Light_Scattering.pdf)

Features
======================

## I. Basic Renderer Functions

### overview

![](img/0.JPG)

### details

* Render Texture

* Orbit Camera

* Gizmos

* Texture Mapping

* Basic Geometry

---

## II. Volume Fog

### overview

|     cube     |    sphere    |     cone     | 
|:------------:|:------------:|:------------:|
|![](img/1.JPG)|![](img/2.JPG)|![](img/3.JPG)|

### details

When rendering the geometry, for each fragment of it, I calculate a ray from the camera to that fragment. Then I use the ray to intersect the geometry in the fragment shader and find the enter and exit position. Along the vector from enter to exit, find a fixed number of positions and use them to sample a 3D noise function as transmittance. Finally, plug the transmittance in to the scattering phase function to get the final scattering factor.

---

## III. Volume Light

### overview

In progress

### details

In progress
---
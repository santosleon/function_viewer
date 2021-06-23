# Function Viewer
This app allows to print 3D functions in a easy way, powered by OpenGL.

The functions could be normal or differential. Then, be creative! :wink:

## Screenshots
Lorentz Attractor (dots)

<img src="https://github.com/santosleon/function_viewer/blob/main/screenshots/lorentz_attractor_dot.png" height="400"/>

Lorentz Attractor (lines)

<img src="https://github.com/santosleon/function_viewer/blob/main/screenshots/lorentz_attractor_line.png" height="400"/>

Cosine (lines)

<img src="https://github.com/santosleon/function_viewer/blob/main/screenshots/cosine_line.png" alt="connection_screenshot" height="400"/>

## How to use?
You can create your own function with x, y, z and dt attributes:
```sh
const float S = 10.0, R = 28.0, B = 8/3;
inline void lorentzAttractor(float& x, float& y, float& z, float dt) {
    float dx = dt * (S * (y - x));
    float dy = dt * (x * (R - z) - y);
    float dz = dt * (x * y - B * z);
    x += dx;
    y += dy;
    z += dz;
}
```
Then, you can edit the following variables to adapt the software to your needs:
- how many points will be printed
```sh
const int pointsQuantity = 5000;
```
- start point
```sh
float x = 0.1f, y = 0.1f, z = 0.1f, dt = 0.01f;
```
- custom function
```sh
for (int i = 0; i < pointsQuantity; i++) {
  vertices[i * 3 + 0] = (float)x;
  vertices[i * 3 + 1] = (float)y;
  vertices[i * 3 + 2] = (float)z;
  lorentzAttractor(x,y,z,dt);  // it could be a custom function
}
```
- points size
```sh
glPointSize((GLfloat)3.0f);
```
- print of points or lines
```sh
glDrawArrays(GL_POINTS, 0, pointsQuantity); // use GL_LINE_STRIP to print lines or GL_POINTS to print points
```

## Controls
|      Action      |     Input    |
|:----------------:|:------------:|
| Directional Move |   WASD keys  |
|   Camera LookAt  |     Mouse    |
|       Zoom       | Mouse Scroll |

## Licence
GLPv3, [read more](https://github.com/santosleon/function_viewer/blob/main/LICENSE)

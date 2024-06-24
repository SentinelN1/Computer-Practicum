import taichi as ti
import taichi.math as tm
import taichi_glsl as ts
from base_shader import BaseShader


red = tm.vec3(1., 0., 0.)
green = tm.vec3(0., 1., 0.)
blue = tm.vec3(0., 0., 1.)
black = tm.vec3(0.)
white = tm.vec3(1.)

background_color = tm.vec3(1 / 255, 109 / 255, 191 / 255)


@ti.func
def sd_circle(p, r):
    return p.norm() - r


@ti.dataclass
class Circle:
    center: tm.vec2
    radius: ti.f32

    @ti.func
    def signed_distance(self, uv) -> ti.f32:
        return sd_circle(uv - self.center, self.radius)


class Shader(BaseShader):
    def __init__(self,
                 title: str,
                 res: tuple[int, int] | None = None,
                 gamma: float = 2.2,
                 count: int = 5):
        super().__init__(title, res=res, gamma=gamma)
        self.circles = Circle.field(shape=(count,))
        self.count = count
        for i in range(self.count):
            self.circles[i].center = tm.vec2(-0.5 + i * 0.2, 0.)
            self.circles[i].radius = 0.1 + i * 0.05

    @ti.func
    def main_image(self, uv, t):
        
        d = 10000.
        idx = 0
        for i in range(self.count):
            cur_d = self.circles[i].signed_distance(uv)
            if cur_d < d:
                d = cur_d
                idx = i

        col = tm.vec3(0.)

        if d < 0.:
            col = tm.mix(green, red, idx / self.count)

        return col


if __name__ == "__main__":
    ti.init(arch=ti.gpu)

    shader = Shader(
        "Circles",
        count=3,
        gamma=1.0)

    shader.main_loop()

import taichi as ti
import taichi.math as tm
import time


@ti.data_oriented
class BaseShader:
    def __init__(self,
                 title: str,
                 res: tuple[int, int] | None = None,
                 gamma: float = 2.2):
        self.title = title
        self.res = res if res is not None else (1280, 720)
        self.resf = tm.vec2(*self.res)
        self.pixels = ti.Vector.field(3, dtype=ti.f32, shape=self.res)
        self.gamma = gamma

    @ti.kernel
    def render(self, t: ti.f32):
        for fragCoord in ti.grouped(self.pixels):
            uv = (fragCoord - 0.5 * self.resf) / self.resf.y
            col = self.main_image(uv, t)
            if self.gamma > 0.0:
                col = col ** (1 / self.gamma)
            self.pixels[fragCoord] = tm.clamp(col, 0., 1.)

    @ti.func
    def main_image(self, uv, t):
        col = tm.vec3(0.)
        col.rg = uv + 0.5
        return col

    def main_loop(self):
        gui = ti.GUI(self.title, res=self.res, fast_gui=True)
        start = time.time()

        while gui.running:  # основной цикл
            if gui.get_event(ti.GUI.PRESS):  # для закрытия приложения по нажатию на Esc
                if gui.event.key == ti.GUI.ESCAPE:
                    break

            t = time.time() - start  # пересчет времени, прошедшего с первого кадра
            self.render(t)  # расчет цветов пикселей
            gui.set_image(self.pixels)  # перенос пикселей из поля pixels в буфер кадра
            gui.show()

        gui.close()


if __name__ == "__main__":
    ti.init(arch=ti.opengl)
    shader = BaseShader("Base shader")
    shader.main_loop()

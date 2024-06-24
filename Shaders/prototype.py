import taichi as ti
import taichi_glsl as ts
import taichi.math as tm
import time

# Инициализация Taichi с использованием GPU архитектуры
ti.init(arch=ti.gpu)

# Установление пропорций экрана и разрешения
aspect_ratio = 16 / 9
screen_height = 720
screen_width = int(aspect_ratio * screen_height)
resolution = screen_width, screen_height

# Определение поля пикселей
pixels = ti.Vector.field(3, dtype=ti.f32, shape=resolution)

@ti.func
def circle_sdf(a, b):
    return (a - b).norm()


@ti.func
def circle_sdf(point, center = ts.vec2(0.0, 0.0), radius=1.0):
    """
    Вычисляет знаковое расстояние до окружности.

    Аргументы:
        point (ts.vec2): Координаты точки.
        radius (float): Радиус окружности.

    Возвращает:
        float: Знаковое расстояние до окружности.
    """
    return (center - point).norm() - radius


@ti.func
def myhash(x):
    """
    Генерирует псевдослучайное число на основе входного значения.

    Аргументы:
        n (float): Входное значение.

    Возвращает:
        float: Псевдослучайное число.
    """
    return ts.fract(ti.sin(x) * 43758.5453)


@ti.func
def smoothmin(a, b, k=1.0):
    h = max(k - abs(a - b), 0.) / k
    return min(a, b) - h * h * k * 0.25


@ti.func
def smoothmax(a, b, k=1.0):
    return smoothmin(a, b, -k)

@ti.kernel
def main_image(t: ti.f32):
    """
    Рендерит изображение на текущий момент времени t.

    Аргументы:
        t (float): Время в секундах.
    """

    # Белый цвет в RGB
    WHITE = ts.vec3(1.0, 1.0, 1.0)
    BLACK = ts.vec3(0.0, 0.0, 0.0)
    RED = ts.vec3(1.0, 0.0, 0.0)
    GREEN = ts.vec3(0.0, 1.0, 0.0)
    BLUE = ts.vec3(0.0, 0.0, 1.0)
    YELLOW = ts.vec3(1.0, 1.0, 0.0)
    PURPLE = ts.vec3(1.0, 0.0, 1.0)
    CYAN = ts.vec3(0.0, 1.0, 1.0)
    GREY = ts.vec3(0.5, 0.5, 0.5)

    # Устанавливаем цвет фона
    # background_color = ts.vec3(1 / 255, 109 / 255, 191 / 255)

    # Проходимся циклом по всем пикселям
    for pixel in ti.grouped(pixels):
        # Переводим положение пикселя в UV-координаты
        point = pixel / screen_height - ts.vec2(aspect_ratio / 2, 1 / 2)

        # Устанавливаем цвет пикселя
        pixel_color = BLACK

        # suN:
        # sun_center = ts.vec2(0.0, 0.0)
        sun_center = ts.vec3(0.0, 0.0, 0.0)
        sun_radius = 0.08
        distance = circle_sdf(point, sun_center, sun_radius)

        if distance <= sun_radius:
            pixel_color = WHITE

        # else:
        #     # intensity = 10 * sun_radius**2
        #     # pixel_color = ts.mix(BLACK, YELLOW, ts.exp(-distance / intensity))
        #     # pixel_color = BLACK

        # a = 0.5
        # b = 0.2
        # angular_velocity = 1
        # phi = angular_velocity * t
        # earth_center = ts.vec2(a * ts.cos(phi), b * ts.sin(phi))
        # # earth_center = ts.vec2(orbit_radius * ts.cos(phi), orbit_radius * ts.sin(phi))
        # earth_radius = 0.01
        # earth_color = GREEN

        # distance = circle_sdf(point, earth_center, earth_radius)
        # if distance <= earth_radius:
        #     # alpha
        #     # v1 = (sun_center - earth_center).normalized()
        #     # v2 = (point - earth_center).normalized()
        
        #     # alpha = max(0, tm.dot(v1, v2))
        #     pixel_color = BLACK

        pixels[pixel] = ts.clamp(pixel_color, 0.0, 1.0)


if __name__ == "__main__":
    gui = ti.GUI("Orbitum", res=resolution, fast_gui=True)
    start = time.time()
    while gui.running:
        if gui.get_event(ti.GUI.PRESS):
            if gui.event.key == ti.GUI.ESCAPE:
                break

        t = time.time() - start
        main_image(t)
        gui.set_image(pixels)
        gui.show()
    gui.close()

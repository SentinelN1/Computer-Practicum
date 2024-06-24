import taichi as ti
import taichi_glsl as ts
import time

ti.init(arch=ti.gpu)

# Параметры экрана
aspect_ratio = 16 / 9
screen_height = 720
screen_width = int(aspect_ratio * screen_height)
resolution = screen_width, screen_height
resolution_float = ts.vec2(float(screen_width), float(screen_height))

pixels = ti.Vector.field(3, dtype=ti.f32, shape=resolution)

@ti.func
def signed_distance_circle(point, radius):
    """
    Вычисляет знаковое расстояние до окружности.

    Аргументы:
        point (ts.vec2): Координаты точки.
        radius (float): Радиус окружности.

    Возвращает:
        float: Знаковое расстояние до окружности.
    """
    return point.norm() - radius

@ti.func
def hash_1to1(n):
    """
    Генерирует псевдослучайное число на основе входного значения.

    Аргументы:
        n (float): Входное значение.

    Возвращает:
        float: Псевдослучайное число.
    """
    return ts.fract(ti.sin(n) * 43758.5453)

@ti.func
def planet_position(angle, distance, eccentricity):
    """
    Вычисляет позицию планеты на орбите.

    Аргументы:
        angle (float): Угол в радианах.
        distance (float): Среднее расстояние от солнца.
        eccentricity (float): Эксцентриситет орбиты.

    Возвращает:
        ts.vec2: Координаты планеты.
    """
    return ts.vec2(distance * ts.cos(angle), distance * ts.sin(angle) * (1 - eccentricity**2)**0.5)

@ti.func
def smoothstep(edge0, edge1, x):
    """
    Функция сглаживания.

    Аргументы:
        edge0 (float): Нижняя граница.
        edge1 (float): Верхняя граница.
        x (float): Входное значение.

    Возвращает:
        float: Сглаженное значение.
    """
    t = ts.clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3 - 2 * t)

@ti.func
def mix_colors(color1, color2, factor):
    """
    Смешивает два цвета.

    Аргументы:
        color1 (ts.vec3): Первый цвет.
        color2 (ts.vec3): Второй цвет.
        factor (float): Коэффициент смешивания.

    Возвращает:
        ts.vec3: Смешанный цвет.
    """
    return ts.mix(color1, color2, factor)

@ti.kernel
def render(t: ti.f32):
    # Определение цветов
    SUN_COLOR = ts.vec3(1.0, 0.8, 0.0)
    PLANET_COLORS = [
        ts.vec3(0.5, 0.5, 0.5),  # Меркурий
        ts.vec3(1.0, 0.9, 0.6),  # Венера
        ts.vec3(0.0, 0.5, 1.0),  # Земля
        ts.vec3(1.0, 0.0, 0.0),  # Марс
        ts.vec3(0.8, 0.5, 0.3),  # Юпитер
        ts.vec3(0.8, 0.8, 0.5),  # Сатурн
        ts.vec3(0.4, 0.6, 0.8),  # Уран
        ts.vec3(0.3, 0.3, 0.8)   # Нептун
    ]
    BACKGROUND_COLOR = ts.vec3(0.0, 0.0, 0.1)

    # Параметры орбит
    ORBIT_DISTANCES = [0.2, 0.3, 0.4, 0.5, 0.7, 0.9, 1.1, 1.3]
    ORBIT_ECCENTRICITIES = [0.2, 0.1, 0.017, 0.093, 0.048, 0.056, 0.047, 0.009]

    for pixel in ti.grouped(pixels):
        uv = (pixel / resolution_float - 0.5) * 2.0
        current_color = BACKGROUND_COLOR

        # Рисование солнца
        sun_distance = signed_distance_circle(uv, 0.1)
        if sun_distance < 0:
            current_color = mix_colors(SUN_COLOR, current_color, smoothstep(0.1, 0.0, sun_distance))

        # Рисование планет
        for i in range(8):
            angle = t * (1.0 + 0.1 * i)
            pos = planet_position(angle, ORBIT_DISTANCES[i], ORBIT_ECCENTRICITIES[i])
            distance = signed_distance_circle(uv - pos, 0.03)
            if distance < 0:
                current_color = mix_colors(PLANET_COLORS[i], current_color, smoothstep(0.03, 0.0, distance))

        pixels[pixel] = ts.clamp(current_color, 0.0, 1.0)

# Создание GUI для отображения
gui = ti.GUI("Solar System", res=resolution, fast_gui=True)
start = time.time()

# Основной цикл программы
while gui.running:
    if gui.get_event(ti.GUI.PRESS):
        if gui.event.key == ti.GUI.ESCAPE:
            break

    t = time.time() - start
    render(t)
    gui.set_image(pixels)
    gui.show()

gui.close()

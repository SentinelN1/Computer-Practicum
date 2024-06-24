"""
Анимация состоит из:
- несколько слоев облаков
- каждый слой имеет свой уровень на экране
- каждый слов состоит из круглых облаков
- цвет облака зависит от слоя (линейная интерполяция между цветом фона и белым цветом)
- каждое облако двигается со своей постоянной скоростью v(t) = U_0 + v * t:
    https://graphtoy.com/?f1(x,t)=0%20+%201%20*%20x&v1=true&f2(x,t)=&v2=false&f3(x,t)=&v3=false&f4(x,t)=&v4=false&f5(x,t)=&v5=false&f6(x,t)=&v6=false&grid=1&coords=0.3784981445800506,-0.027070996853054252,4.62651947315441
    U_0 -- начальное положение облака
    v -- скорость движения облака
- облака двигаются по синусоиде V(t) = V_0 + A * Sin(\omega * t + \varphi):
    https://graphtoy.com/?f1(x,t)=0%20+%201%20*%20sin(1%20*%20x%20+%200)&v1=true&f2(x,t)=&v2=false&f3(x,t)=&v3=false&f4(x,t)=&v4=false&f5(x,t)=&v5=false&f6(x,t)=&v6=false&grid=1&coords=0.3784981445800506,-0.027070996853054252,4.62651947315441
    V_0 -- начальная высота облака
    A -- амплитуда колебаний
    \omega -- частота колебаний
    \varphi -- начальный сдвиг фазы
- каждое облако обладает своим радиусом
- облака на ближних слоях перекрывают облака на дальних слоях
- если облако выходит за границы экрана, оно появляется с другой стороны
"""

import taichi as ti
import taichi_glsl as ts
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
def height_quad(layer, layers, v_min=-0.5,  v_max=0.5):
    """
    Вычисляет высоту для заданного слоя с использованием квадратичного веса.

    Аргументы:
        layer (int): Номер текущего слоя (начиная с 0).
        layers (int): Общее количество слоев.
        v_min (float, optional): Минимальное значение высоты. По умолчанию -0.5.
        v_max (float, optional): Максимальное значение высоты. По умолчанию 0.5.

    Возвращает:
        float: Высота для данного слоя, интерполированная между v_min и v_max с использованием квадратичного веса.

    https://graphtoy.com/?f1(x,t)=lerp(-0.5,%200.5,%20(x%20-%201)%5E2)&v1=true&f2(x,t)=&v2=false&f3(x,t)=&v3=false&f4(x,t)=&v4=false&f5(x,t)=&v5=false&f6(x,t)=&v6=false&grid=1&coords=0.5498211684248091,-0.04731363127532326,0.9106846071534273
    """
    weight = (layer / layers - 1)**2
    return ts.mix(v_min, v_max, weight)


@ti.kernel
def main_image(t: ti.f32):
    """
    Рендерит изображение на текущий момент времени t.

    Аргументы:
        t (float): Время в секундах.
    """

    # Белый цвет в RGB
    WHITE = ts.vec3(1.0, 1.0, 1.0)

    # Устанавливаем цвет фона
    background_color = ts.vec3(1 / 255, 109 / 255, 191 / 255)

    # Проходимся циклом по всем пикселям
    for pixel in ti.grouped(pixels):
        # Переводим положение пикселя в UV-координаты
        uv = pixel / screen_height - ts.vec2(aspect_ratio / 2, 1 / 2)

        # Устанавливаем цвет пикселя
        current_color = background_color

        # Выбираем количество слоёв с облаками
        layers = 6
        v_min = -0.5
        v_max = 0.1
        for layer in range(layers):
            v_0 = height_quad(layer, layers, v_min, v_max)

            # Количество облаков на каждый слой
            clouds = 8
            for cloud in range(2 * clouds):
                rand = hash_1to1(float(cloud + 2 * clouds * layer))

                # Начальное положение облака
                alpha = cloud / (2 * clouds)
                u_0 = ts.mix(-aspect_ratio, aspect_ratio, alpha) + ts.mix(-0.05, 0.05, rand)

                # Скорость движения облака
                rand = hash_1to1(rand)
                velocity = ts.mix(0.02, 0.025, rand) + 0.015 * layer # Скорость зависит от слоя для достиженя эффекта параллакса

                # Амплитуда колебаний облака
                rand = hash_1to1(rand)
                amplitude = ts.mix(0.01, 0.025, rand)

                # Частота колебаний облака
                rand = hash_1to1(rand)
                frequency = ts.mix(0.5, 1.5, rand) 

                # Начальный сдвиг фазы
                rand = hash_1to1(rand)
                phi_0 = ts.mix(0, 2 * ts.pi, rand)

                # Радиус облака
                rand = hash_1to1(rand)
                radius = ts.mix(0.15, 0.3, rand)

                # Обновляем координаты центра облака
                u = u_0 - velocity * t
                v = v_0 + amplitude * ts.sin(frequency * t + phi_0)

                # Делаем так, чтобы облако врзвращалось с другого края экрана
                u = ts.fract((u + aspect_ratio) / (2 * aspect_ratio))
                u = u * 2 * aspect_ratio
                u = u - aspect_ratio

                center = ts.vec2(u, v)
                point = center - uv

                # Считаем знаковое расстояние от пикселя до центра облака
                distance = signed_distance_circle(point, radius)

                # Красим пиксель в соответствующий цвет
                if distance <= 0:
                    current_color = ts.mix(background_color, WHITE, (layer + 1) / (layers + 1))

        pixels[pixel] = ts.clamp(current_color, 0.0, 1.0)


if __name__ == "__main__":
    gui = ti.GUI("Clouds", res=resolution, fast_gui=True)
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

import numpy as np
import vispy.app
import vispy.scene
from vispy.geometry import Rect

screen_width = 1920
screen_height = 1080
screen_size = (screen_width, screen_height)
aspect_ratio = screen_width / screen_height

x_range = (0, screen_width)
y_range = (0, screen_height)
radius_range = (0, 256)

vispy.app.use_app('pyglet')

canvas = vispy.scene.SceneCanvas(show=True, size=screen_size)
view = canvas.central_widget.add_view()


view.camera = vispy.scene.PanZoomCamera(
    rect=Rect(0, 0, screen_width, screen_height))


# def create_obstacles(number, position_range, radius_range):
for i in range(100):
    x_min, x_max = x_range
    y_min, y_max = y_range
    r_min, r_max = r_range
    x = np.random.uniform(low=x_min, high=x_max)
    y = np.random.uniform(low=y_min, high=y_max)
    radius = np.random.uniform(low=radius_min, high=y_max)
    center = np.array([x, y])
    color = (1, 1, 1, 1)
    vispy.scene.visuals.Ellipse(center=center, color=color, radius=radius, parent=view.scene)


def update(event):
    canvas.update()


if __name__ == "__main__":
    timer = vispy.app.Timer(interval=0,
                            start=True,
                            connect=update)
    canvas.measure_fps()
    vispy.app.run()

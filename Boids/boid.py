import numpy as np
import vispy.app
import vispy.scene
from vispy.geometry import Rect
from numba import njit, prange

number_of_obstacles = 20
number_of_agents = 1000

screen_width = 1280
screen_height = 1280
screen_size = (screen_width, screen_height)

position_range = (0, 0, screen_width, screen_height)
x_range = (0, screen_width)
y_range = (0, screen_height)
radius_range = (screen_width * 0.01, screen_width * 0.02)
velocity_range = (0, 128)
acceleration_range = (0, 64)
dt = 0.001

noise_coefficient = 10
alignment_coefficient = 0.5
cohesion_coefficient = 0.05
separation_coefficient = 5
evasion_coefficient = 500

alignment_radius = 64
cohesion_radius = 64
separation_radius = 64

vispy.app.use_app('pyglet')

canvas = vispy.scene.SceneCanvas(show=True, size=screen_size)
view = canvas.central_widget.add_view()

aspect_ratio = screen_width / screen_height

view.camera = vispy.scene.PanZoomCamera(
    rect=Rect(0, 0, screen_width, screen_height))


@njit(cache=True)
def set_magnitude(vector: np.ndarray,
                  magnitude: float) -> np.ndarray:
    result = np.copy(vector)
    norm = np.linalg.norm(result)
    if norm > 0:
        result *= magnitude / norm
    return result


@njit(cache=True)
def normalize(vector: np.ndarray) -> np.ndarray:
    return set_magnitude(vector, 1)


@njit(cache=True)
def limit(vector: np.ndarray,
          max_magnitude: float) -> np.ndarray:
    result = np.copy(vector)
    norm = np.linalg.norm(result)
    if norm > max_magnitude:
        result *= max_magnitude / norm
    return result


def create_obstacles(number_of_obstacles: int,
                     x_range: tuple[float, float],
                     y_range: tuple[float, float],
                     radius_range: tuple[float, float]) -> np.ndarray:
    obstacles = np.zeros(shape=(number_of_obstacles, 3))

    x_min, x_max = x_range
    y_min, y_max = y_range
    radius_min, radius_max = radius_range

    for i in range(number_of_obstacles):
        radius = np.random.uniform(low=radius_min, high=radius_max)
        x = np.random.uniform(low=x_min + 2 * radius,
                              high=x_max - 2 * radius)
        y = np.random.uniform(low=y_min + 2 * radius,
                              high=y_max - 2 * radius)
        center = np.array([x, y])

        valid = False
        while not valid:
            valid = True
            for j in range(i - 1):
                if np.linalg.norm(center - obstacles[j, 0:2]) <= 1 * (obstacles[j, 2] + radius):
                    radius = np.random.uniform(low=radius_min, high=radius_max)
                    x = np.random.uniform(low=x_min + 2 * radius,
                                          high=x_max - 2 * radius)
                    y = np.random.uniform(low=y_min + 2 * radius,
                                          high=y_max - 2 * radius)
                    center = np.array([x, y])
                    valid = False
                    break

        obstacles[i, 0] = x
        obstacles[i, 1] = y
        obstacles[i, 2] = radius

    return obstacles


@njit(parallel=True)
def initialize_random_agents(number_of_agents: int,
                             position_range: tuple[float, float, float, float],
                             velocity_range: tuple[float, float],
                             acceleration_range: tuple[float, float],
                             obstacles: np.ndarray) -> np.ndarray:

    agents = np.zeros(shape=(number_of_agents, 6))

    number_of_obstacles = obstacles.shape[0]
    for i in range(number_of_agents):
        x_min, y_min, x_max, y_max = position_range

        position_x = np.random.uniform(low=x_min, high=x_max)
        position_y = np.random.uniform(low=y_min, high=y_max)
        position = np.array([position_x, position_y])

        valid = False
        while not valid:
            valid = True
            for j in prange(number_of_obstacles):
                center = obstacles[j, 0:2]
                radius = obstacles[j, 2]
                distance = np.linalg.norm(position - center)
                if (distance <= radius):
                    position_x = np.random.uniform(low=x_min,
                                                   high=x_max)
                    position_y = np.random.uniform(low=y_min,
                                                   high=y_max)
                    position = np.array([position_x, position_y])
                    valid = False
                    break

        agents[i, 0] = position_x
        agents[i, 1] = position_y

        velocity_min, velocity_max = velocity_range
        velocity_magnitude = np.sqrt(np.random.uniform(low=velocity_min,
                                                       high=velocity_max))
        velocity_angle = np.random.uniform(low=0,
                                           high=2 * np.pi)
        velocity_x = velocity_magnitude * np.cos(velocity_angle)
        velocity_y = velocity_magnitude * np.sin(velocity_angle)
        agents[i, 2] = velocity_x
        agents[i, 3] = velocity_y

    return agents


obstacles = create_obstacles(number_of_obstacles=number_of_obstacles,
                             x_range=x_range,
                             y_range=y_range,
                             radius_range=radius_range)


for obstacle in obstacles:
    center = obstacle[0:2]
    radius = obstacle[2]
    vispy.scene.visuals.Ellipse(center=center,
                                color=(1, 1, 1, 1),
                                radius=radius,
                                parent=view.scene)


agents = initialize_random_agents(number_of_agents=number_of_agents,
                                  position_range=position_range,
                                  velocity_range=acceleration_range,
                                  acceleration_range=acceleration_range,
                                  obstacles=obstacles)


@njit(parallel=True)
def get_agent_arrows(agents, dt):
    size = agents.shape[0]
    arrows = np.zeros(shape=(size, 4))

    for i in prange(size):
        agent = agents[i]

        position_x = agent[0]
        position_y = agent[1]
        velocity_x = agent[2]
        velocity_y = agent[3]

        arrows[i, 0] = position_x
        arrows[i, 1] = position_y
        arrows[i, 2] = position_x + velocity_x * dt
        arrows[i, 3] = position_y + velocity_y * dt

    return arrows


arrows = vispy.scene.Arrow(arrows=get_agent_arrows(agents, dt),
                           arrow_color=(0.25, 1, 0.75, 1),
                           arrow_size=16,
                           connect='segments',
                           parent=view.scene)


@njit(parallel=True)
def update_position(agents, position_range, dt):
    agents[:, 0:2] += agents[:, 2:4] * dt


@njit(parallel=True)
def update_velocity(agents, velocity_range, dt):
    size = agents.shape[0]
    velocity_min, velocity_max = velocity_range
    for i in prange(size):
        agents[i, 2:4] = limit(agents[i, 2:4] + agents[i, 4:6] * dt,
                               velocity_max)


@njit(parallel=True, cache=True)
def distance_matrix(agents):
    size = agents.shape[0]
    matrix = np.zeros(shape=(size, size))
    for i in prange(size):
        for j in range(0, i - 1):
            distance = np.linalg.norm(agents[i, 0:2] - agents[j, 0:2])
            matrix[i, j] = distance
            matrix[j, i] = distance

    return matrix


@njit(parallel=True)
def noise_acceleration(agents, acceleration_range):
    size = agents.shape[0]
    noise = np.zeros(shape=(size, 2))
    acceleration_min, acceleration_max = acceleration_range
    for i in prange(size):
        magnitude = np.sqrt(np.random.uniform(low=acceleration_min,
                                              high=acceleration_max))
        angle = np.random.uniform(low=0,
                                  high=2 * np.pi)
        noise[i] = magnitude * np.array([np.cos(angle), np.sin(angle)])
    return noise


@njit(parallel=True)
def alignment_acceleration(agents,
                           velocity_range,
                           acceleration_range,
                           distances,
                           radius):
    size = agents.shape[0]
    alignment = np.zeros((size, 2))

    velocity_min, velocity_max = velocity_range

    for i in prange(size):
        neighbor_average_velocity = np.zeros(2)
        neighbor_count = 0
        for j in prange(size):
            if (i != j and distances[i, j] < radius):
                neighbor_count += 1
                neighbor_average_velocity += agents[j, 2:4]

        if neighbor_count > 0:
            neighbor_average_velocity /= neighbor_count

        alignment[i] = set_magnitude(neighbor_average_velocity,
                                     velocity_max) - agents[i, 2:4]

    return alignment


@njit(parallel=True)
def cohesion_acceleration(agents,
                          distances,
                          radius):
    size = agents.shape[0]
    cohesion = np.zeros((size, 2))

    for i in prange(size):
        neighbor_average_position = np.zeros(2)
        neighbor_count = 0
        for j in prange(size):
            if (i != j and distances[i, j] < radius):
                neighbor_average_position += agents[j, 0:2]
                neighbor_count += 1

        if neighbor_count > 0:
            neighbor_average_position /= neighbor_count

        cohesion[i] = neighbor_average_position - agents[i, 0:2]

    return cohesion


@njit(parallel=True)
def separation_acceleration(agents,
                            distances,
                            radius):
    size = agents.shape[0]
    separation = np.zeros((size, 2))
    for i in prange(size):
        for j in prange(size):
            if (i != j and distances[i, j] > 0 and distances[i, j] < radius):
                separation[i] += (agents[i, 0:2] - agents[j, 0:2]) \
                    / distances[i, j]

    return separation


@njit(cache=True)
def step(x, x_range):
    x_min, x_max = x_range
    x = (x - x_min) / (x_max - x_min)
    if x <= 0:
        return 0
    elif x >= 1:
        return 1
    return x


@njit(cache=True)
def smoothstep(x, x_range):
    x_min, x_max = x_range
    x = (x - x_min) / (x_max - x_min)
    if x <= 0:
        x = 0
    elif x >= 1:
        x = 1
    return x * x * (3.0 - 2.0 * x)


@njit(parallel=True)
def evasion_acceleration(agents, position_range, obstacles, parameter):
    number_of_agents = agents.shape[0]
    evasion = np.zeros(shape=(number_of_agents, 2))
    x_min, y_min, x_max, y_max = position_range

    for i in prange(number_of_agents):
        position = agents[i, 0:2]
        velocity = agents[i, 2:4]

        left_edge = -min(0, velocity[0]) / np.abs(position[0] - x_min)
        right_edge = min(0, -velocity[0]) / np.abs(x_max - position[0])
        bottom_edge = -min(0, velocity[1]) / np.abs(position[1] - y_min)
        top_edge = min(0, -velocity[1]) / np.abs(y_max - position[1])

        evasion[i, 0] = left_edge + right_edge
        evasion[i, 1] = bottom_edge + top_edge

        number_of_obstacles = obstacles.shape[0]
        for j in prange(number_of_obstacles):
            center = obstacles[j, 0:2]
            radius = obstacles[j, 2]
            difference = agents[i, 0:2] - center
            direction = normalize(difference)
            distance = np.linalg.norm(difference) - radius
            evasion[i] -= direction * radius * min(0, np.dot(velocity, direction)) / distance**2
            # evasion[i] -= direction * min(np.dot(velocity, direction), 0) * smoothstep(distance, (radius, 0))

    return evasion


@njit(parallel=True, cache=True)
def update_acceleration(agents,
                        position_range,
                        acceleration_range,
                        noise_coefficient,
                        alignment_coefficient,
                        alignment_radius,
                        cohesion_coefficient,
                        cohesion_radius,
                        separation_coefficient,
                        separation_radius,
                        evasion_coefficient,
                        obstacles):
    number_of_agents = agents.shape[0]
    distances = distance_matrix(agents)
    noise = noise_coefficient * noise_acceleration(agents=agents,
                                                   acceleration_range=acceleration_range)
    alignment = alignment_coefficient * alignment_acceleration(agents=agents,
                                                               velocity_range=velocity_range,
                                                               acceleration_range=acceleration_range,
                                                               distances=distances,
                                                               radius=alignment_radius)
    cohesion = cohesion_coefficient * cohesion_acceleration(agents=agents,
                                                            distances=distances,
                                                            radius=cohesion_radius)
    separation = separation_coefficient * separation_acceleration(agents=agents,
                                                                  distances=distances,
                                                                  radius=separation_radius)
    evasion = evasion_coefficient * evasion_acceleration(agents=agents,
                                                         position_range=position_range,
                                                         obstacles=obstacles,
                                                         parameter=0.2)
    acceleration_min, acceleration_max = acceleration_range

    for i in prange(number_of_agents):
        acceleration = noise[i] + alignment[i] + \
            cohesion[i] + separation[i] + evasion[i]

        agents[i, 4:6] = limit(acceleration, acceleration_max)


def update_agents(agents,
                  position_range,
                  velocity_range,
                  acceleration_range,
                  noise_coefficient,
                  alignment_coefficient,
                  alignment_radius,
                  cohesion_coefficient,
                  cohesion_radius,
                  separation_coefficient,
                  separation_radius,
                  evasion_coefficient,
                  obstacles):
    update_position(agents, position_range, dt)
    update_velocity(agents, velocity_range, dt)
    update_acceleration(agents=agents,
                        position_range=position_range,
                        acceleration_range=acceleration_range,
                        noise_coefficient=noise_coefficient,
                        alignment_coefficient=alignment_coefficient,
                        alignment_radius=alignment_radius,
                        cohesion_coefficient=cohesion_coefficient,
                        cohesion_radius=cohesion_radius,
                        separation_coefficient=separation_coefficient,
                        separation_radius=separation_radius,
                        evasion_coefficient=evasion_coefficient,
                        obstacles=obstacles)


def update(event):
    update_agents(agents=agents,
                  position_range=position_range,
                  velocity_range=velocity_range,
                  acceleration_range=acceleration_range,
                  noise_coefficient=noise_coefficient,
                  alignment_coefficient=alignment_coefficient,
                  alignment_radius=alignment_radius,
                  cohesion_coefficient=cohesion_coefficient,
                  cohesion_radius=cohesion_radius,
                  separation_coefficient=separation_coefficient,
                  separation_radius=separation_radius,
                  evasion_coefficient=evasion_coefficient,
                  obstacles=obstacles)
    arrows.set_data(arrows=get_agent_arrows(agents, dt))
    canvas.update()


if __name__ == "__main__":
    timer = vispy.app.Timer(interval=0,
                            start=True,
                            connect=update)
    canvas.measure_fps()
    vispy.app.run()

import numpy as np
import matplotlib.pyplot as plt

# Ellipsoid parameters
a, b, c = 2, 3, 4

# Create ellipsoid mesh using spherical angles
u = np.linspace(0, 2 * np.pi, 100)
v = np.linspace(0, np.pi, 100)
x = a * np.outer(np.cos(u), np.sin(v))
y = b * np.outer(np.sin(u), np.sin(v))
z = c * np.outer(np.ones_like(u), np.cos(v))

# Line segments: [x1 y1 z1 x2 y2 z2]
lines = [
    ([ 5,  0,  0, -5,  0,  0], "red"),
    ([ 0,  5,  0,  0, -5,  0], "darkcyan"),
    ([ 0,  0,  5,  0,  0, -5], "limegreen"),
    ([-1,  0,  4,  1,  0,  4], "darkorange"),
    ([ 1,  1,  1,  0,  0,  1], "darkviolet"),
    ([1.5, 2.5, 0, 1.5, 2.5, 1], "maroon")
]

# Set up figure with 2x2 subplots
fig = plt.figure(figsize=(14, 10))
views = [
    ("Isometric", 30, 30),
    ("Top View", 90, -90),
    ("Front View", 0, 0),
    ("Side View", 0, 90)
]

for i, (title, elev, azim) in enumerate(views, 1):
    ax = fig.add_subplot(2, 2, i, projection='3d')
    ax.plot_surface(x, y, z, alpha=0.2, edgecolor='none', color='grey')

    for j, (line, color) in enumerate(lines, 1):
        x_vals = [line[0], line[3]]
        y_vals = [line[1], line[4]]
        z_vals = [line[2], line[5]]
        # add legend labels only once
        if i == 1:
            ax.plot(x_vals, y_vals, z_vals, label=f"Line {j}", color=color, linewidth=2)
        else:
            ax.plot(x_vals, y_vals, z_vals, color=color, linewidth=2)
        ax.scatter(x_vals, y_vals, z_vals, color=color)

        # If it's Line 5 or 6, add an extended dashed line
        if j == 5 or j == 6:
            # Convert to numpy arrays for vector math
            p1 = np.array(line[:3])
            p2 = np.array(line[3:])
            direction = p2 - p1
            direction = direction / np.linalg.norm(direction)  # normalize

            # Extend the line in both directions
            extension_length = 5
            extended_start = p1 - direction * extension_length
            extended_end = p2 + direction * extension_length

            # Plot the extended dashed line (without endpoints)
            ax.plot(
                [extended_start[0], extended_end[0]],
                [extended_start[1], extended_end[1]],
                [extended_start[2], extended_end[2]],
                linestyle='--',
                color=color,
                linewidth=1.5,
                alpha=0.6
            )


    ax.set_title(title, y=0.95, fontweight="bold", fontsize=18)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.view_init(elev=elev, azim=azim)
    ax.set_box_aspect([1, 1, 1])
    ax.grid(True)

fig.legend(loc="center")
plt.tight_layout()
plt.savefig("ellipsoid_multiview.png", dpi=200, bbox_inches='tight')
plt.show()

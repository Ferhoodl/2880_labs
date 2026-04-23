import time
import socket
import tkinter as tk
import threading
import numpy as np
import matplotlib

matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Globals
gui_send_message = "wait\n"
angles = []
distances = []

# Robot state
robot_x = 0
robot_y = 0
robot_theta = 90  # facing "up" initially (degrees)

obstacles = []
debris = []

# Field size (cm)
FIELD_X = 426
FIELD_Y = 245

# Make map large enough to move a full field in any direction
MAP_LIMIT = max(FIELD_X, FIELD_Y)

def main():
    global window, Last_command_Label, ax, canvas, map_ax, map_canvas
    global turn_entry, drive_entry

    window = tk.Tk()
    window.title("CyBot Scanner")

    # === LEFT FRAME (existing UI) ===
    left_frame = tk.Frame(window)
    left_frame.pack(side="left")

    Last_command_Label = tk.Label(left_frame, text="Last Command Sent:")
    Last_command_Label.pack()

    tk.Button(left_frame, text="Scan", command=send_scan).pack()
    tk.Button(left_frame, text="Quit", command=send_quit).pack()

    # Polar plot
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.set_title("CyBot Scan")
    ax.set_thetamin(0)
    ax.set_thetamax(180)

    canvas = FigureCanvasTkAgg(fig, master=left_frame)
    canvas.get_tk_widget().pack()

    # === RIGHT FRAME (NEW MAP UI) ===
    right_frame = tk.Frame(window)
    right_frame.pack(side="right")

    # Map plot
    map_fig, map_ax = plt.subplots()
    map_ax.set_title("Field Map")

    map_canvas = FigureCanvasTkAgg(map_fig, master=right_frame)
    map_canvas.get_tk_widget().pack()

    # Controls
    tk.Label(right_frame, text="Turn (degrees):").pack()
    turn_entry = tk.Entry(right_frame)
    turn_entry.pack()
    tk.Button(right_frame, text="Turn", command=send_turn).pack()

    tk.Label(right_frame, text="Drive (cm):").pack()
    drive_entry = tk.Entry(right_frame)
    drive_entry.pack()
    tk.Button(right_frame, text="Drive", command=send_drive).pack()

    tk.Button(right_frame, text="Mark Obstacle", command=mark_obstacle).pack()
    tk.Button(right_frame, text="Mark Debris", command=mark_debris).pack()

    update_map()

    threading.Thread(target=socket_thread, daemon=True).start()

    window.mainloop()

# === COMMANDS ===

def send_scan():
    global gui_send_message
    gui_send_message = "s\n"

def send_quit():
    global gui_send_message, window
    gui_send_message = "quit\n"
    time.sleep(0.5)
    window.destroy()

def send_turn():
    global gui_send_message, robot_theta
    try:
        val = float(turn_entry.get())
        msg = f"TURN:{val:+.0f}\n"
        gui_send_message = msg

        robot_theta += val
        update_map()

    except:
        print("Invalid turn input")

def send_drive():
    global gui_send_message, robot_x, robot_y, robot_theta
    try:
        val = float(drive_entry.get())
        msg = f"DRIVE:{val:.0f}\n"
        gui_send_message = msg

        # Update position
        rad = np.deg2rad(robot_theta)
        robot_x += val * np.cos(rad)
        robot_y += val * np.sin(rad)

        update_map()

    except:
        print("Invalid drive input")

def mark_obstacle():
    global obstacles, robot_x, robot_y, robot_theta

    rad = np.deg2rad(robot_theta)
    ox = robot_x + 5 * np.cos(rad)
    oy = robot_y + 5 * np.sin(rad)

    obstacles.append((ox, oy))
    update_map()

def mark_debris():
    global debris, robot_x, robot_y, robot_theta

    rad = np.deg2rad(robot_theta)
    dx = robot_x + 5 * np.cos(rad)
    dy = robot_y + 5 * np.sin(rad)

    debris.append((dx, dy))
    update_map()

# === SOCKET THREAD ===

def socket_thread():
    global gui_send_message, angles, distances

    HOST = "192.168.1.1"
    PORT = 288

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))
    cybot = sock.makefile("rbw", buffering=0)

    cybot.write("H\n".encode())

    send_message = ""

    while send_message != "quit\n":

        window.after(0, lambda msg=send_message:
            Last_command_Label.config(text="Last Command Sent: " + msg))

        if send_message == "s\n":
            angles.clear()
            distances.clear()

            while True:
                line = cybot.readline().decode().strip()

                if line == "BEGINSCAN":
                    continue
                if line == "ENDSCAN":
                    break

                try:
                    angle_str, dist_str = line.split(":")
                    angle = float(angle_str)
                    distance = float(dist_str)

                    if angle < 0 or angle > 178 or angle % 2 != 0:
                        continue

                    angles.append(np.deg2rad(angle))
                    distances.append(distance)

                    window.after(0, update_plot)

                except:
                    print("Bad data:", line)

        while gui_send_message == "wait\n":
            time.sleep(0.1)

        send_message = gui_send_message
        gui_send_message = "wait\n"

        cybot.write(send_message.encode())

    cybot.close()
    sock.close()

# === PLOTTING ===

def update_plot():
    ax.clear()
    ax.set_title("CyBot Scan")
    ax.set_thetamin(0)
    ax.set_thetamax(180)

    if distances:
        ax.plot(angles, distances, color='red')
        ax.scatter(angles, distances, c='blue', s=10)
        ax.set_rmax(max(distances) + 10)

    canvas.draw()

def update_map():
    map_ax.clear()
    map_ax.set_title("Field Map")

    # Set bounds centered at (0,0)
    map_ax.set_xlim(-MAP_LIMIT, MAP_LIMIT)
    map_ax.set_ylim(-MAP_LIMIT, MAP_LIMIT)

    # Draw robot (30 cm diameter)
    robot_circle = plt.Circle((robot_x, robot_y), 15, color='green')
    map_ax.add_patch(robot_circle)

    # Draw heading line
    rad = np.deg2rad(robot_theta)
    map_ax.plot(
        [robot_x, robot_x + 20 * np.cos(rad)],
        [robot_y, robot_y + 20 * np.sin(rad)],
        color='green'
    )

    # Draw obstacles
    if obstacles:
        ox, oy = zip(*obstacles)
        map_ax.scatter(ox, oy, c='red')

    if debris:
        dx, dy = zip(*debris)
        map_ax.scatter(dx, dy, c='blue', s=20)

    map_ax.set_aspect('equal')
    map_canvas.draw()

# Run
main()
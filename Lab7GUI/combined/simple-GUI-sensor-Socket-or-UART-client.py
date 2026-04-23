import time
import socket
import tkinter as tk
import threading
import numpy as np
import matplotlib

# Use Tkinter-compatible backend
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# Globals
gui_send_message = "wait\n"
angles = []
distances = []

def main():
    global window, Last_command_Label, ax, canvas

    window = tk.Tk()
    window.title("CyBot Scanner")

    # Label
    Last_command_Label = tk.Label(window, text="Last Command Sent:")
    Last_command_Label.pack()

    # Buttons
    tk.Button(window, text="Scan", command=send_scan).pack()
    tk.Button(window, text="Quit", command=send_quit).pack()

    # Create polar plot
    fig, ax = plt.subplots(subplot_kw={'projection': 'polar'})
    ax.set_title("CyBot Scan")
    ax.set_thetamin(0)
    ax.set_thetamax(180)

    # Embed plot in Tkinter
    canvas = FigureCanvasTkAgg(fig, master=window)
    canvas.get_tk_widget().pack()

    # Start socket thread
    threading.Thread(target=socket_thread, daemon=True).start()

    window.mainloop()

def send_scan():
    global gui_send_message
    gui_send_message = "s\n"

def send_quit():
    global gui_send_message, window
    gui_send_message = "quit\n"
    time.sleep(0.5)
    window.destroy()

def socket_thread():
    global gui_send_message, angles, distances

    HOST = "192.168.1.1"
    PORT = 288

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))
    cybot = sock.makefile("rbw", buffering=0)

    # Initial message
    cybot.write("H\n".encode())

    send_message = ""

    while send_message != "quit\n":

        # Update label safely
        window.after(0, lambda msg=send_message: Last_command_Label.config(
            text="Last Command Sent: " + msg))

        if send_message == "s\n":
            print("Starting scan...")

            angles.clear()
            distances.clear()

            while True:
                line = cybot.readline().decode().strip()

                if line == "BEGINSCAN":
                    continue

                if line == "ENDSCAN":
                    print("Scan complete")
                    break

                try:
                    angle_str, dist_str = line.split(":")
                    angle = float(angle_str)
                    distance = float(dist_str)

                    # Enforce expected range and increments (optional safety)
                    if angle < 0 or angle > 178 or angle % 2 != 0:
                        print("Unexpected angle:", angle)
                        continue

                    angles.append(np.deg2rad(angle))
                    distances.append(distance)

                    # Update plot live (thread-safe)
                    window.after(0, update_plot)

                except:
                    print("Bad data:", line)

        # Wait for GUI command
        while gui_send_message == "wait\n":
            time.sleep(0.1)

        send_message = gui_send_message
        gui_send_message = "wait\n"

        cybot.write(send_message.encode())

    cybot.close()
    sock.close()

def update_plot():
    global ax, canvas, angles, distances

    ax.clear()
    ax.set_title("CyBot Scan")

    # Limit to 0–180 degrees
    ax.set_thetamin(0)
    ax.set_thetamax(180)

    if len(distances) > 0:
        ax.scatter(angles, distances, c='blue', s=10)
        ax.plot(angles, distances, color='red', linewidth=1)
        ax.set_rmax(max(distances) + 10)

    canvas.draw()

# Run program
main()
from flask import Flask, render_template, Response
import io, rpc, serial, serial.tools.list_ports, socket, sys, struct
# import cv2

app = Flask(__name__)

# Print all ports
for port, desc, hwid in serial.tools.list_ports.comports():
    print("{} : {} [{}]".format(port, desc, hwid))

camport = "/dev/ttyACM1"

def get_frame_buffer_call_back(interface, pixformat_str, framesize_str, cutthrough, silent):
    if not silent: print("Getting Remote Frame...")

    result = interface.call("jpeg_image_snapshot", "%s,%s" % (pixformat_str, framesize_str))
    if result is not None:

        size = struct.unpack("<I", result)[0]
        img = bytearray(size)

        if cutthrough:
            # Fast cutthrough data transfer with no error checking.

            # Before starting the cut through data transfer we need to sync both the master and the
            # slave device. On return both devices are in sync.
            result = interface.call("jpeg_image_read")
            if result is not None:

                # GET BYTES NEEDS TO EXECUTE NEXT IMMEDIATELY WITH LITTLE DELAY NEXT.

                # Read all the image data in one very large transfer.
                interface.get_bytes(img, 5000) # timeout

        else:
            # Slower data transfer with error checking.

            # Transfer 32 KB chunks.
            chunk_size = (1 << 15)

            if not silent: print("Reading %d bytes..." % size)
            for i in range(0, size, chunk_size):
                ok = False
                for j in range(3): # Try up to 3 times.
                    result = interface.call("jpeg_image_read", struct.pack("<II", i, chunk_size))
                    if result is not None:
                        img[i:i+chunk_size] = result # Write the image data.
                        if not silent: print("%.2f%%" % ((i * 100) / size))
                        ok = True
                        break
                    if not silent: print("Retrying... %d/2" % (j + 1))
                if not ok:
                    if not silent: print("Error!")
                    return None

        return img

    else:
        if not silent: print("Failed to get Remote Frame!")

    return None

def gen_frames():  # generate frame by frame from camera
    interface = rpc.rpc_usb_vcp_master(port=camport)
    while True:
        sys.stdout.flush()
        img = get_frame_buffer_call_back(interface, "sensor.RGB565", "sensor.QQVGA", cutthrough=True, silent=True)
        if img is None:
            break
        else:
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + img + b'\r\n')  # concat frame one by one and show result


@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')


@app.route('/')
def index():
    """Video streaming home page."""
    return render_template('index.html')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port='5000', debug=True)
    